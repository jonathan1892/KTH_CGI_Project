#include <iostream>
#include <iomanip>
#include "glm/glm.hpp"
#include <random>
#include <fstream>
#include <thread>
#include <vector>

#include "TestModel.h"

using namespace std;
using glm::vec3;
using glm::mat3;

// ----------------------------------------------------------------------------
// HYPER PARAMETERS
#define SCREEN_WIDTH 300 // MUST BE A MULTIPLE OF NUMBER_THREADS
#define SCREEN_HEIGHT 300 // MUST BE A MULTIPLE OF NUMBER_THREADS
#define NUMBER_THREADS 10

// ----------------------------------------------------------------------------
// GLOBAL VARIABLES

// Camera parameters
float focalLength = SCREEN_WIDTH;
vec3 cameraPos(0, 0, -3);
mat3 R;
float yaw = 0;

// Pathtracer parameters
const float GAMMA = 2.2f;
const int MAX_DEPTH = 3;
const int N_PIXEL_AVERAGE = 40000;
const float INTERSECTION_EPSILON = 0.001; // Offset used when computing the new intersection of a light ray
                                          // Prevents the ray of intersecting its start surface

// Scene to render
vector<Triangle> sceneTriangles;

// Random number generation
std::default_random_engine generator;
std::normal_distribution<float> distribution(0.0,1.0);

// Data structure storing the screen representation
float screen[SCREEN_HEIGHT][SCREEN_WIDTH][3];

// ----------------------------------------------------------------------------
// FUNCTIONS

void Update();
void Draw();
bool ClosestIntersection(
                         vec3 start,
                         vec3 dir,
                         const vector<Triangle>& triangles,
                         Intersection& closestIntersection
                         );
vec3 TracePath(vec3 start, vec3 dir, int depth);
void uniformSamplerHemisphere(const vec3& normal, vec3& dir);
void writeToFile();
float clamp(const float &lo, const float &hi, const float &v);
void drawThread(int x, int y);

// ----------------------------------------------------------------------------
// SOURCE CODE

int main( int argc, char* argv[] )
{
    LoadTestModel(sceneTriangles);
    Draw();
    return 0;
}

// Computes the color of every pixel in the screen, and stores it the screen buffer
void Draw()
{
    for( int y=0; y<SCREEN_HEIGHT; ++y )
    {
        for( int x=0; x<SCREEN_WIDTH; x+=NUMBER_THREADS)
        {
            std::vector<thread*> threads;
            for(int t=0; t<NUMBER_THREADS; ++t)
            {
                threads.push_back(new std::thread(drawThread, x+t, y));
            }
            
            for(int t=0; t<NUMBER_THREADS; ++t)
            {
                threads[t]->join();
                delete(threads[t]);
            }
        }

        cout << setw(7) << left <<(y + 1.0) / SCREEN_HEIGHT * 100 <<  " % done" << endl;
    }

    writeToFile();
}

// Computes the color of an individual pixel to the screen buffer
//      x is the x coordinate of the pixel to render
//      y is the y coordinate of the pixel to render
void drawThread(int x, int y)
{
    // Direction of the traced ray
    vec3 dir(x - SCREEN_WIDTH/2, y - SCREEN_HEIGHT/2, focalLength);
    dir = R * dir;
    dir = glm::normalize(dir);

    // Default color for the pixel
    vec3 color(0, 0, 0);

    // Path trace
    for(int i=0; i<N_PIXEL_AVERAGE; ++i)
    {
        color += TracePath(cameraPos, dir, 0);
    }
    
    color /= N_PIXEL_AVERAGE;
    screen[y][x][0] = color.r;
    screen[y][x][1] = color.g;
    screen[y][x][2] = color.b;
}

// Performs the recursive path tracing operation (the rendering equation is handled here)
//      returns a vector representing the color of the lightray
//      start is the starting position of the lightray
//      dir is a vector representing the direction of the lightray
//      depth represents the recursion depth
vec3 TracePath(vec3 start, vec3 dir, int depth)
{
    vec3 black(0.0f, 0.0f, 0.0f);

    // If we reach the max depth, we return the color black
    if(depth >= MAX_DEPTH)
    {
        return black;
    }

    // Compute the lightray's intersection, if there is none, return black
    Intersection intersection;
    if(!ClosestIntersection(start, dir, sceneTriangles, intersection))
    {
        return black;
    }

    // Get the emitted color, the normal and the intersection point
    Triangle currentT = sceneTriangles[intersection.triangleIndex];
    vec3 reflectance = currentT.reflectance;
    vec3 emittance = currentT.emittance;
    vec3 normal = currentT.normal;
    vec3 newStart = intersection.position;

    // Emit new rays at random (uniform distribution) and average the received color
    vec3 indirectDiffuse(0.0f, 0.0f, 0.0f);

    vec3 newDir;
    uniformSamplerHemisphere(normal, newDir);
    float cos_theta = glm::dot(normal, newDir);
    vec3 BRDF = 2 * cos_theta * reflectance;
    indirectDiffuse += (TracePath(newStart + newDir * INTERSECTION_EPSILON, newDir, depth+1) * BRDF);

    // If the surface is ideally specular, trace the reflected light ray
    vec3 specularLight(0.0f, 0.0f, 0.0f);
    if(currentT.reflect > 0)
    {
        vec3 reflectedDir = dir - (2 * glm::dot(normal, dir)) * normal;
        specularLight = TracePath(newStart + reflectedDir * INTERSECTION_EPSILON, reflectedDir, depth);
    }

    return emittance + indirectDiffuse + currentT.reflect * specularLight;
}

// Computes the intersection with the closest triangle that intersects the lightray (a lightray is
//      uni-directional vector).
//      Returns true if an intersection has been found, false otherwise.
//      start is the starting position of the lightray
//      dir is a vector representing the direction of the lightray
//      triangles is the collection of triangles that we want to test for the intersetion
//      closestIntersection stores the result of the computations
bool ClosestIntersection(vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection)
{
    const float kEpsilon = 1e-8;
    bool intersectionFound = false;
    closestIntersection.distance = std::numeric_limits<float>::max();

    for(int i=0; i<triangles.size(); ++i)
    {
        // Compute intersection with plane
        vec3 v0 = triangles[i].v0;
        vec3 v1 = triangles[i].v1;
        vec3 v2 = triangles[i].v2;

        vec3 e1 = v1 - v0;
        vec3 e2 = v2 - v0;
        vec3 pvec = glm::cross(dir, e2);
        float det = glm::dot(e1, pvec);

        // ray and triangle are parallel if det is close to 0
        if (fabs(det) < kEpsilon) continue;
    
        float invDet = 1.0 / det;
        vec3 tvec = start - v0;
        float u = glm::dot(tvec, pvec) * invDet;

        if (u < 0 || u > 1) continue;

        vec3 qvec = glm::cross(tvec, e1); 
        float v = glm::dot(dir, qvec) * invDet;
        if (v < 0 || u + v > 1) continue;

        float t = glm::dot(e2, qvec) * invDet; 
        if(t < kEpsilon) continue;

        intersectionFound = true;
        vec3 position = start + t*dir;
        float distance = glm::distance(start, position);

        // Store the closest intersection
        if(distance<closestIntersection.distance)
        {
            closestIntersection.distance = distance;
            closestIntersection.triangleIndex = i;
            closestIntersection.position = position;
        }
    }

    return intersectionFound;
}

// Computes a random vector with uniform probability within the hemishpere specified by the normal vector
//      normal is the normal vector defining the hemisphere
//      dir is the resulting randomly chosen vector belonging the hemisphere
void uniformSamplerHemisphere(const vec3& normal, vec3& dir)
{
    // Randomly generate a point on the sphere by taking three independent gaussian distributions
    // and normalize the vector
    dir.x = distribution(generator);
    dir.y = distribution(generator);
    dir.z = distribution(generator);

    dir = glm::normalize(dir);

    // If not in the hemisphere, simply invert the vector
    if(glm::dot(dir, normal) < 0)
    {
        dir = -dir;
    }
}

// Writes the screen buffer to a .ppm file, with gamma correction
void writeToFile()
{
    std::ofstream ofs; 
    ofs.open("out.ppm"); 
    ofs << "P6\n" << SCREEN_WIDTH << " " << SCREEN_HEIGHT << "\n255\n"; 
    for (uint32_t i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) 
    {
        int x = i % SCREEN_WIDTH;
        int y = i / SCREEN_WIDTH;
        char r = (char)(255 * clamp(0, 1, powf(screen[y][x][0], 1.0/GAMMA))); 
        char g = (char)(255 * clamp(0, 1, powf(screen[y][x][1], 1.0/GAMMA))); 
        char b = (char)(255 * clamp(0, 1, powf(screen[y][x][2], 1.0/GAMMA))); 
        ofs << r << g << b; 
    } 
    ofs.close(); 

}

// Bounds the value of a float
//      Returns the value v if lo < v < hi, lo if v < lo and hi if v > hi
//      lo is the lower bound float
//      hi is the higher bound float
//      v is the original value
float clamp(const float &lo, const float &hi, const float &v) 
{ 
    return std::max(lo, std::min(hi, v)); 
} 

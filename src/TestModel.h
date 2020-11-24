// THE FOLLOWING CODE IS BASED ON THE CODE PROVIDED FOR LABS 2 AND 3 OF THE RENDERING TRACK

#ifndef TEST_MODEL_CORNEL_BOX_H
#define TEST_MODEL_CORNEL_BOX_H

// Defines a simple test model: The Cornel Box

#include "glm/glm.hpp"
#include <vector>

int lightWidth = 250;
float LIGHT_POWER = 5;

// Used to describe a triangular surface:
class Triangle
{
public:
	glm::vec3 v0;
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 normal;
	glm::vec3 reflectance;      // Represents the reflected color by the surface
    glm::vec3 emittance;        // Representes the emitted light by the surface (if a light source)
    float reflect;				// Represents the "reflectiveness" of an ideally specular surface

    Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 reflectance, glm::vec3 emittance, 
    	double reflect =0.0)
		: v0(v0), v1(v1), v2(v2), reflectance(reflectance), emittance(emittance), reflect(reflect)
	{
		ComputeNormal();
	}

	void ComputeNormal()
	{
		glm::vec3 e1 = v1-v0;
		glm::vec3 e2 = v2-v0;
		normal = glm::normalize( glm::cross(e2, e1));
	}
};

struct Intersection
{
    glm::vec3 position;
    float distance;
    int triangleIndex;
};

void setRotationMatrix(float z, glm::mat3& R)
{
    R[0][0] = cos(z);
    R[0][1] = 0;
    R[0][2] = sin(z);
    R[1][0] = 0;
    R[1][1] = 1;
    R[1][2] = 0;
    R[2][0] = -sin(z);
    R[2][1] = 0;
    R[2][2] = cos(z);
}

// Loads the Cornell Box. It is scaled to fill the volume:
// -1 <= x <= +1
// -1 <= y <= +1
// -1 <= z <= +1
void LoadTestModel(std::vector<Triangle>& triangles)
{
	using glm::vec3;

	// Defines colors:
	vec3 red(    1.00f, 0.00f, 0.00f );
	vec3 yellow( 0.75f, 0.75f, 0.15f );
	vec3 green(  0.00f, 1.00f, 0.00f );
	vec3 cyan(   0.15f, 0.75f, 0.75f );
	vec3 blue(   0.15f, 0.15f, 0.75f );
	vec3 purple( 0.75f, 0.15f, 0.75f );
	vec3 white(  1.00f, 1.00f, 1.00f );
    vec3 whiteBr(1.00f, 1.00f, 1.00f );
	whiteBr = LIGHT_POWER * whiteBr;
    vec3 black(  0.00f, 0.00f, 0.00f );

	triangles.clear();
	triangles.reserve( 5*2*3 );

	// ---------------------------------------------------------------------------
	// Room

	float L = 555;			// Length of Cornell Box side.

	vec3 A(L,0,0);
	vec3 B(0,0,0);
	vec3 C(L,0,L);
	vec3 D(0,0,L);

	vec3 E(L,L,0);
	vec3 F(0,L,0);
	vec3 G(L,L,L);
	vec3 H(0,L,L);

	// Floor:
	triangles.push_back(Triangle(C, B, A, white, black, 1));
	triangles.push_back(Triangle(C, D, B, white, black, 1));

	// Left wall
	triangles.push_back(Triangle(A, E, C, green, black));
	triangles.push_back(Triangle(C, E, G, green, black));

	// Right wall
	triangles.push_back(Triangle(F, B, D, red, black));
	triangles.push_back(Triangle(H, F, D, red, black));

	// Ceiling
	triangles.push_back(Triangle(E, F, G, white, black));
	triangles.push_back(Triangle(F, H, G, white, black));

	// Back wall
	triangles.push_back(Triangle(G, D, C, white, black));
	triangles.push_back(Triangle(G, H, D, white, black));

	// ---------------------------------------------------------------------------
	// Short block

    glm::mat3 R;
    setRotationMatrix(-0.6, R);
    vec3 translation(400, 80, 160);
    
    A = (R * vec3(80,-80,-80)) + translation;
    B = (R * vec3(-80,-80,-80)) + translation;
    C = (R * vec3(80,-80,80)) + translation;
    D = (R * vec3(-80,-80,80)) + translation;
    
    E = (R * vec3(80,80,-80)) + translation;
    F = (R * vec3(-80,80,-80)) + translation;
    G = (R * vec3(80,80,80)) + translation;
    H = (R * vec3(-80,80,80)) + translation;
    
    // Front
	triangles.push_back(Triangle(E, B, A, white, black));
	triangles.push_back(Triangle(E, F, B, white, black));

	// Front
	triangles.push_back(Triangle(F, D, B, white, black));
	triangles.push_back(Triangle(F, H, D, white, black));

	// BACK
	triangles.push_back(Triangle(H, C, D, white, black));
	triangles.push_back(Triangle(H, G, C, white, black));

	// LEFT
	triangles.push_back(Triangle(G, E, C, white, black));
	triangles.push_back(Triangle(E, A, C, white, black));

	// TOP
	triangles.push_back(Triangle(G, F, E, white, black));
	triangles.push_back(Triangle(G, H, F, white, black));

	// ---------------------------------------------------------------------------
	// Tall block
    
    setRotationMatrix(0.4, R);
    translation.x = 200;
    translation.y = 160;
    translation.z = 360;

	A = (R * vec3(80,-160,-80)) + translation;
	B = (R * vec3(-80,-160,-80)) + translation;
	C = (R * vec3(80,-160,80)) + translation;
	D = (R * vec3(-80,-160,80)) + translation;
    
    E = (R * vec3(80,160,-80)) + translation;
    F = (R * vec3(-80,160,-80)) + translation;
    G = (R * vec3(80,160,80)) + translation;
    H = (R * vec3(-80,160,80)) + translation;
    
	// Front
	triangles.push_back(Triangle(E, B, A, white, black));
	triangles.push_back(Triangle(E, F, B, white, black));

	// Front
	triangles.push_back(Triangle(F, D, B, white, black));
	triangles.push_back(Triangle(F, H, D, white, black));

	// BACK
	triangles.push_back(Triangle(H, C, D, white, black));
	triangles.push_back(Triangle(H, G, C, white, black));

	// LEFT
	triangles.push_back(Triangle(G, E, C, white, black));
	triangles.push_back(Triangle(E, A, C, white, black));

	// TOP
	triangles.push_back(Triangle(G, F, E, white, black));
	triangles.push_back(Triangle(G, H, F, white, black));

    
    // Light
    
    A = vec3(L/2 + lightWidth/2, L-1, L/2 - lightWidth/2);
    B = vec3(L/2 - lightWidth/2, L-1, L/2 - lightWidth/2);
    C = vec3(L/2 + lightWidth/2, L-1, L/2 + lightWidth/2);
    D = vec3(L/2 - lightWidth/2, L-1, L/2 + lightWidth/2);
    
    triangles.push_back(Triangle(A, B, C, white, whiteBr));
    triangles.push_back(Triangle(B, D, C, white, whiteBr));
    
	// ----------------------------------------------
	// Scale to the volume [-1,1]^3

	for(size_t i=0; i<triangles.size(); ++i)
	{
		triangles[i].v0 *= 2/L;
		triangles[i].v1 *= 2/L;
		triangles[i].v2 *= 2/L;

		triangles[i].v0 -= vec3(1,1,1);
		triangles[i].v1 -= vec3(1,1,1);
		triangles[i].v2 -= vec3(1,1,1);

		triangles[i].v0.x *= -1;
		triangles[i].v1.x *= -1;
		triangles[i].v2.x *= -1;

		triangles[i].v0.y *= -1;
		triangles[i].v1.y *= -1;
		triangles[i].v2.y *= -1;

		triangles[i].ComputeNormal();
	}
}

#endif

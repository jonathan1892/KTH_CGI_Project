# Presentation

This repository contains the source code and the report for the Computer Graphics and Interactions course at KTH.
The project consists of implementing a pathtracer (see the report for details) and comparing this method to other common computer graphics method.

# Running the code

All the source code for this project, as well as the required glm library can be found in the `/src` folder. This code also requires the SDL library (https://www.libsdl.org/) for tracing. Please make sure that this library is installed on your computer before attempting to run the code.

The code can then be compiled by using the provided makefile (that is type `Make` in a terminal while being in the `/src` folder), which will generate a runable instance of the program.

# Results

Ths section showcases an "eye-candy" result that can be found in the report. Please refer to that document for more results and a discussion regarding the behavior of the implemented pathtracer, as well as more details regarding the implementation.

![first example of Cornell Box generated with the pathtracer](/report/big_100000.png)


# Report

The full report for this project can be found at `/report.pdf`.
It discusses the theoretical aspects of a pathtracer, implementation choices and details for this project, and compares and tries to explain different results obtained with this pathtracer and other tracing methods.

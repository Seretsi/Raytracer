#include <iostream>
#include "include\FreeImage.h"
#include <string>
//#include "vector.h"
//#include "matrix.h"
#include "Transform.h"

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

// glm provides vector, matrix classes like glsl
// Typedefs to make code more readable 

typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;

// include transform class
// include object classes
std::vector<vec3> plane = { vec3(-1.0f, - 1.0f, 0.0f), vec3(1.0f, - 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f), vec3(-1.0f, 1.0f, 0.0f) };
// include parser
// include tracer
    // num bounces
int numBounces = 1;

/*
    Dev plan:
    get line-plane intersection
        create camera class
        instantiate hardcoded instance
        create plane object
        create light object
        transform scene
            implement transforms
            implement stack
            move sphere by member transform then dump stack change
        render
            - all elements need to be transform in the right place here
    get line-sphere intersection
        implement tracing change

        */

const int w = 50, h = 50;
std::string fname = "outfile.png";

void windowReframe();

int main()
{
    //open cmd line args
    // open and parse scene file
    std::cout << "_______________WELCOME TO TAS RAYTRACER_______________\n";
    std::cout << "version 0.1 dev\n";
    int pixelCount = w * h;
    BYTE *frame = new BYTE[3*pixelCount];
    FreeImage_Initialise();

    vec3 eye = vec3(-4, - 4, 4);
    vec3 center = vec3(1, 0, 0);
    vec3 up = vec3(0, 1, 0);
    float fov = 30.0f;
    
    // generate view mat
    mat4 modelview = Transform::lookat(eye, center, up);
    mat4 projection; // = Transform::projection(near, far, aspect, fov);
    mat4 mvp = projection * modelview;

    // transform objects by model transforms into world coords
    // transform everything by modelview 
    // get materials together
    
    // rendering
    // begin raytracing
    for (int i = 0; i < w * h; i++)
    {
        int i3 = i * 3;
        frame[i3] = frame[i3 +1] = frame[i3+2] = CHAR_MAX;
    }

    // save frame
    FIBITMAP* img = FreeImage_ConvertFromRawBits(frame, w, h, w * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);
    FreeImage_Save(FIF_PNG, img, fname.c_str(), 0);
    
    // clean up
    FreeImage_DeInitialise();
    std::cout << "Hello World!\n";
    //system("pause");
}

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include <iostream>
#include "include\FreeImage.h"
#include <string>
#include <limits>
//#include "vector.h"
//#include "matrix.h"
#include "Transform.h"
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
std::vector<vec3> plane = {
    vec3(-1.0f, - 1.0f, 0.0f),
    vec3(1.0f, - 1.0f, 0.0f),
    vec3(1.0f, 1.0f, 0.0f),
    vec3(-1.0f, 1.0f, 0.0f)
};

// include parser
// include tracer
    // num bounces
const int NUMBOUNCES = 1;
const float EPSILON = 5e-5;

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

const int w = 100, h = 100;
std::string fname = "outfile.png";

struct Intersection
{
    float mindist = std::numeric_limits<float>::max();
    bool hitObject;
};

//camera
vec3 eye = vec3(-4, -4, 4);
vec3 center = vec3(1, 0, 0);
vec3 up = vec3(0, 1, 0);
float fov = 30.0f;

void windowReframe();
bool readPixels(int w, int h, std::vector<float>& data, std::vector<BYTE>& raw_pixels);
vec3 rayDir(vec3 eye, int i, int j);
Intersection findIntersection(vec3 ray);
void intersectsTriPlane(int ind_x, int ind_y, int ind_z, const vec3 rayDir, Intersection& intersection);
vec3 findColour(Intersection hit);

int main()
{
    //open cmd line args
    // open and parse scene file
    std::cout << "_______________WELCOME TO T.A.S RAYTRACER_______________\n";
    std::cout << "version 0.1 dev\n";

    int pixelCount = w * h;
    std::vector<float> framebuf;
    framebuf.reserve(pixelCount);
    FreeImage_Initialise();
    
    // generate view mat
    mat4 modelview = Transform::lookat(eye, center, up);
    //mat4 projection; // = Transform::projection(near, far, aspect, fov);
    //mat4 mvp = projection * modelview;

    // transform objects by model transforms into world coords
    // transform everything by modelview 
    // get materials together
    
    // rendering
    // begin raytracing
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            // get direction
            vec3 ray = rayDir(eye, i, j);
            // get intersection data
            Intersection hit = findIntersection(ray);
            // get colour
            vec3 colour = vec3(1.0f, 0.0f, 0.0f); // findColour(hit);

            //framebuf[i * w * 3 + j * 3] = colour[0];
            framebuf.push_back(colour[0]);
            framebuf.push_back(colour[1]);
            framebuf.push_back(colour[2]);
            //std::cout << sizeof(unsigned char);
        }
//        std::cout << std::endl;
    }

    // save frame
    std::vector<BYTE> rawFrame;
    //rawFrame.resize(3 * pixelCount);
    readPixels(w, h, framebuf, rawFrame); // add typedef <> thing
    FIBITMAP* img = FreeImage_ConvertFromRawBits(rawFrame.data(), w, h, w * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, true);
    FreeImage_Save(FIF_PNG, img, fname.c_str(), 0);
    
    // clean up
    FreeImage_DeInitialise();
    std::cout << "TRACING COMPLETE\n";
    system("pause");
}

// lots of data loss operations 
bool readPixels(int w, int h, std::vector<float>& data, std::vector<BYTE>& raw_pixels)
{
    int index = 0;
    std::vector<BYTE> pixel; pixel.resize(3);
    while (index < data.size())
    {
        // create pixel
        pixel[index % 3] = 255*data[index];
        if (index % 3 == 2)
        {
            raw_pixels.push_back(pixel[2]);
            raw_pixels.push_back(pixel[1]);
            raw_pixels.push_back(pixel[0]);
        }
        index++;
    }
    return true;
}

vec3 rayDir(vec3 eye, int i, int j)
{
    float rFOV = fov * glm::pi<float>() / 180.0f;
    float a = tan(rFOV / 2.0f);
    a *= (j - w / 2.0f) / (w / 2.0f);
    float b = a; // fovy == fovx here
    b *= (h / 2.0f - i) / (h / 2.0f);

    vec3 imagePlaneDir = vec3(0.0f, 0.0f, 1.0f);
    vec3 u = glm::normalize(glm::cross(imagePlaneDir, up));
    vec3 v = glm::normalize(glm::cross(u, imagePlaneDir));

    vec3 dir = glm::normalize(a * u + b * v - imagePlaneDir);

    vec3 ret = eye + dir;

    return ret;
}

Intersection findIntersection(vec3 ray) // scene is global var for now
{
    Intersection intersection;
/*
tri 0 1 2
tri 0 2 3
*/
    intersectsTriPlane(0, 1, 2, ray, intersection);
    intersectsTriPlane(0, 2, 3, ray, intersection);

    return intersection;
}

vec3 findColour(Intersection hit)
{
    if (hit.hitObject)
        return vec3(1.0f, 0.0f, 0.0f);
    //nudge hit location lightward
    return vec3(0.0f, 0.0f, 1.0f);
}

void intersectsTriPlane(int ind_x, int ind_y, int ind_z, const vec3 rayDir, Intersection& intersection)
{
    //find the normal
    vec3 A = plane[ind_x];
    vec3 B = plane[ind_y];
    vec3 C = plane[ind_z];
    // normal via cross prod
    vec3 AC = C - A;
    vec3 AB = B - A;
    vec3 n = glm::cross(AC, AB);

    //early check
    float potentialIntersection = glm::dot(rayDir, n);
    if (potentialIntersection < 0.0f + EPSILON)
        return;

    // fill ray-plane intersection
    float hitPoint = glm::dot(A, n) - glm::dot(eye, n);
    hitPoint /= glm::dot(rayDir, n);
    //if (hitPoint < 0) return;
    
    vec3 P = eye + rayDir * hitPoint;
    
    vec3 AP = P - A;
    vec3 nACP = glm::cross(AC, AP);
    vec3 nAPB = glm::cross(AP, AB);
    float u = glm::length(nACP) / 2.0f;
    float v = glm::length(nAPB) / 2.0f;
    float w = 1.0f - u - v;

    if (w < 1.0f && w > 0.0f) // inside tri
    {
        if (hitPoint > 0.0f && hitPoint < intersection.mindist)
        {
            intersection.mindist = hitPoint;
            intersection.hitObject = true;
        }
    }
}
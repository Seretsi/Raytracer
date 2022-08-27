
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
std::vector<vec3> planex = {
    vec3(-1.0f, - 1.0f, 0.0f),
    vec3(1.0f, - 1.0f, 0.0f),
    vec3(1.0f, 1.0f, 0.0f),
    vec3(-1.0f, 1.0f, 0.0f)
};
std::vector<vec3> plane = {
    vec3(-0.1f, -0.1f, 0.0f),
    vec3(0.1f, -0.1f, 0.0f),
    vec3(0.1f, 0.1f, 0.0f),
    vec3(-0.1f, 0.1f, 0.0f)
};
// include parser
// include tracer
    // num bounces
const int NUMBOUNCES = 1;
const float EPSILON = 5e-5f;

/*
    Dev plan:
    get line-plane intersection
        create camera class
            create camera-to-world mat
            add rot + trans mats to move camera
        instantiate hardcoded instance
        create plane object
        create light object
        transform scene
            implement transforms
            implement stack
            move sphere by member transform then dump stack change
        render
            - all elements need to be transform in the right place here
            - use cam-world mat to trans eye and image plane point to world
            - get norm direction
            - trace with it!
    get line-sphere intersection
        implement tracing change

        */

const int w = 100, h = 100;
std::string fname = "outfile.png";

struct Intersection
{
    float mindist = std::numeric_limits<float>::max();
    bool hitObject = false;
};

struct Camera
{
    //camera
    vec3 eye = vec3(0.0f, 0.0f, 4.0f);
    vec3 center = vec3(0.0f, 0.0f, 0.0f);
    vec3 up = vec3(0.0f, 1.0f, 0.0f);
};

float fov = 30.0f;

bool readPixels(int w, int h, std::vector<float>& data, std::vector<BYTE>& raw_pixels);
vec3 rayDir(const Camera& cam, float i, float j);
Intersection findIntersection(const vec3 ray, const Camera& cam);
void intersectSphere(const vec3& rayDir, const vec3& sphereCenter, const float& sphereRadius, const Camera& cam, Intersection& intersection);
void intersectsTri(int ind_x, int ind_y, int ind_z, const vec3 rayDir, Intersection& intersection, const Camera& cam);
void intersectPlane(const vec3& normal, const vec3& planePos, const vec3& rayDir, const Camera& cam, Intersection& intersection);
vec3 findColour(const Intersection& hit);

int main()
{
    //open cmd line args
    // open and parse scene file
    std::cout << "_______________WELCOME TO T.A.S RAYTRACER_______________\n";
    std::cout << "version 0.1 dev\n";

    Camera cam;
    int pixelCount = w * h;
    std::vector<float> framebuf;
    framebuf.reserve(pixelCount);
    FreeImage_Initialise();
    
    // generate view mat
    mat4 modelview = Transform::lookat(cam.eye, cam.center, cam.up);
    //mat4 projection; // = Transform::projection(near, far, aspect, fov);
    //mat4 mvp = projection * modelview;

    // transform objects by model transforms into world coords
    // transform everything by modelview 
    // get materials together
    
    // rendering -0.246164337 -0.246164337 -0.937446654 last render
    // begin raytracing
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            vec3 ray = rayDir(cam, i, j);
            //ray = vec3(0.0f, 0.0f, -1.0f);
            Intersection hit = findIntersection(ray, cam);
            vec3 colour = findColour(hit);

            framebuf.push_back(colour[0]);
            framebuf.push_back(colour[1]);
            framebuf.push_back(colour[2]);
            //std::cout << "\r" << ((i+1)*h + (j+1)) / totalPixels * 100.0f;
        }
    }

    // save frame
    std::vector<BYTE> rawFrame;
    rawFrame.reserve(framebuf.size());
    //rawFrame.resize(3 * pixelCount);
    readPixels(w, h, framebuf, rawFrame); // add typedef <> thing
    FIBITMAP* img = FreeImage_ConvertFromRawBits(rawFrame.data(), w, h, w * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, true);
    FreeImage_Save(FIF_PNG, img, fname.c_str(), 0);
    
    // clean up
    FreeImage_DeInitialise();
    std::cout << " TRACING COMPLETE\n";
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

vec3 rayDir(const Camera& cam, float i, float j)
{
    // cam will contain its camtoworld mat
    float aspectRatio = w / h;
    float rFOV = fov * glm::pi<float>() / 180.0f;
    float a = tan(rFOV / 2.0f);
    float b = a; // fovy == fovx here
    a *= (((j + 0.5f) - (w / 2.0f)) / (w / 2.0f)) * aspectRatio;
    b *= (h / 2.0f - (i + 0.5f)) / (h / 2.0f);

    vec3 imagePlaneDir = vec3(0.0, 0.0f, -1.0f); // cam.center - cam.eye; // (1, 0, -4)
    vec3 u = glm::normalize(glm::cross(cam.up, imagePlaneDir));
    vec3 v = glm::normalize(glm::cross(imagePlaneDir, u));

    vec3 dir = glm::normalize(a*u + b*v + imagePlaneDir);

    return dir; // fix aberrations in render. Is plane centered?
}

Intersection findIntersection(const vec3 ray, const Camera& cam) // scene is global var for now
{
    Intersection intersection;
/*
tri 0 1 2
tri 0 2 3
*/
    //intersectsTri(0, 1, 2, ray, intersection, cam);
    //intersectsTriPlane(0, 2, 3, ray, intersection, cam);
    //intersectSphere(ray, vec3(0.0f), 0.50f, cam, intersection);
    intersectPlane(vec3(0.0f, 0.0f, 1.0f), vec3(0.0f), ray, cam, intersection);
    return intersection;
}

vec3 findColour(const Intersection& hit)
{
    if (hit.hitObject)
        return vec3(1.0f, 0.0f, 0.0f);
    //nudge hit location lightward
    return vec3(0.01f, 0.01f, 0.01f);
}

void intersectSphere(const vec3& rayDir, const vec3& sphereCenter, const float& sphereRadius, const Camera& cam, Intersection& intersection)
{
    // |ray eqn - center loc|^2 - radius^2 => implicit sphere with ray eqn inside. C off-center sphere
    // |O + Dt-C|^2 - R^2
    // early hit detection with determinate
    float a = glm::dot(rayDir, rayDir);
    vec3 eyeToSphere = cam.eye - sphereCenter;
    float b = 2 * glm::dot(rayDir, eyeToSphere);
    float c = glm::dot(eyeToSphere, eyeToSphere) - (sphereRadius * sphereRadius);

    float determinate = b * b - 4.0f * a * c;
    if (determinate < 0.0f) { return; }
    if (determinate == 0.0f)
    {
        intersection.hitObject = true;
        intersection.mindist = -0.5f * b /  a;
    }
    else
    {
        intersection.hitObject = true;
        float dist = (b > 0) ?
            -0.5f * (b + sqrtf(determinate)) / a:
            -0.5f * (b - sqrtf(determinate)) / a;
        if (dist < c / dist) {
            if (dist < 0) {
                if (c / dist < 0) {
                    return;
                }
                else {
                    intersection.hitObject = true;
                    intersection.mindist = c / dist;
                }
            }
            else {
                intersection.hitObject = true;
                intersection.mindist = dist;
            }
        }
        else {
            if (c / dist < 0) {
                if (dist < 0) {
                    return;
                }
                else {
                    intersection.hitObject = true;
                    intersection.mindist = dist;
                }
            }
            else {
                intersection.hitObject = true;
                intersection.mindist = c / dist;
            }
        }
    }
}

void intersectPlane(const vec3& normal, const vec3& planePos, const vec3& rayDir, const Camera& cam, Intersection& intersection)
{
    float denom = glm::dot(normal, rayDir);
    if (denom < EPSILON && denom > -EPSILON) return;

    vec3 planePosEyeVec = planePos - cam.eye;
    float eyePlaneNormalDot = glm::dot(planePosEyeVec, normal);
    if (eyePlaneNormalDot < EPSILON && eyePlaneNormalDot > -EPSILON) return;

    float hit = eyePlaneNormalDot / denom;
    vec3 hitlocation = cam.eye + hit * rayDir;
    float maxX = std::max(plane[0].x, std::max(plane[1].x, std::max(plane[2].x, plane[3].x)));
    float minX = std::min(plane[0].x, std::min(plane[1].x, std::min(plane[2].x, plane[3].x)));
    float maxY = std::max(plane[0].y, std::max(plane[1].y, std::max(plane[2].y, plane[3].y)));
    float minY = std::min(plane[0].y, std::min(plane[1].y, std::min(plane[2].y, plane[3].y)));
    float maxZ = std::max(plane[0].z, std::max(plane[1].z, std::max(plane[2].z, plane[3].z)));
    float minZ = std::min(plane[0].z, std::min(plane[1].z, std::min(plane[2].z, plane[3].z)));
    if (hitlocation.x <= maxX &&
        hitlocation.x >= minX &&
        hitlocation.y <= maxY &&
        hitlocation.y >= minY &&
        hitlocation.z <= maxZ &&
        hitlocation.z >= minZ)
    {
        intersection.hitObject = true;
        intersection.mindist = std::min(intersection.mindist, hit);
    }
}

void intersectsTri(int ind_x, int ind_y, int ind_z, const vec3 rayDir, Intersection& intersection, const Camera& cam)
{
    // find the normal
    // normal via cross prod
    vec3 A = plane[ind_x];
    vec3 B = plane[ind_y];
    vec3 C = plane[ind_z];
    vec3 AC = C - A;
    vec3 AB = B - A;
    vec3 n = glm::normalize(glm::cross(AC, AB)); 

    // n in same dir to raydir
    //early check
    float potentialIntersection = glm::dot(rayDir, n);
    if (potentialIntersection < EPSILON &&
        potentialIntersection > -EPSILON)
        return;

    // complete ray-plane intersection
    float hitPoint = glm::dot(A, n) - glm::dot(cam.eye, n);
    hitPoint /= potentialIntersection;
    
    vec3 P = cam.eye + rayDir * hitPoint;
    
    vec3 AP = P - A;
    vec3 nACP = glm::cross(AC, AP);
    vec3 nAPB = glm::cross(AP, AB);

    float triArea = n.length() / 2.0f;
    float u = glm::length(nACP) / 2.0f;
    u /= triArea;

    float v = glm::length(nAPB) / 2.0f;
    v /= triArea;

    float w = 1.0f - u - v;

    if (w <= 1.0f && w >= 0.0f) // inside tri
    {
        if (hitPoint > 0.0f && hitPoint < intersection.mindist)
        {
            //std::cout << "hit!!\n";
            intersection.mindist = hitPoint;
            intersection.hitObject = true;
        }
    }
}
#include "sphere.h"
#include "ray.h"
#include <iostream>
#include <math.h>
using namespace std;
Sphere::Sphere(const Parse* parse, std::istream& in)
{
    in>>name>>center>>radius;
}

// Determine if the ray intersects with the sphere
Hit Sphere::Intersection(const Ray& ray, int part) const
{
    double dist = -1; // initialize to no intersect
    Hit hit(dist, -1, {});
    
    vec3 centerMinusEnd = this->center - ray.endpoint;
    double centerMinusEndMag = centerMinusEnd.magnitude();
    
    // calc ray length needed to be orth with sphere center
    // by using dot product (center - endpoint) dot (ray direction)
    double rayBelowCenter = dot(centerMinusEnd, ray.direction);
    // if dot product is < 0, it means the sphere is behind the ray (the angle between them > 90 degrees) 
    if(rayBelowCenter < 0)
		return hit;
    
    // Dist from sphere center to ray calculation (a^2 = c^2-b^2)
    double centerToRay = sqrt(centerMinusEndMag*centerMinusEndMag - rayBelowCenter*rayBelowCenter);
    // sphere center to ray distance must be <= radius to intersect
    if(centerToRay >= this->radius)
		return hit;
    
    // At this point guaranteed >=1 intersect
    // calc dist from ray center to intersect by (b^2 = c^2-a^2)
    double centerToIntersect = sqrt(this->radius*this->radius - centerToRay*centerToRay);
    // calc ray end point to intersect dist and save to hit
    hit.dist = rayBelowCenter - centerToIntersect;
    
    return hit;
}

vec3 Sphere::Normal(const Ray& ray, const Hit& hit) const
{
    vec3 normal;
    TODO; // compute the normal direction
    return normal;
}

std::pair<Box,bool> Sphere::Bounding_Box(int part) const
{
    return {{center-radius,center+radius},false};
}

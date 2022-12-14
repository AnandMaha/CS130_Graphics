#include "plane.h"
#include "hit.h"
#include "ray.h"
#include <cfloat>
#include <limits>
#include <iostream>
using namespace std;

Plane::Plane(const Parse* parse,std::istream& in)
{
    in>>name>>x>>normal;
    normal=normal.normalized();
}

// Intersect with the half space defined by the plane.  The plane's normal
// points outside.  If the ray starts on the "inside" side of the plane, be sure
// to record a hit with t=0 as the first entry in hits.
Hit Plane::Intersection(const Ray& ray, int part) const
{
    double dist = -1; // initialize to no intersect
    Hit hit(dist, -1, {});
    double rayNormDot = dot(ray.direction, normal);
    hit.dist = dot(x - ray.endpoint, normal) / rayNormDot;
    if(hit.dist < 0)
		hit.dist = -1;
    return hit;
}

vec3 Plane::Normal(const Ray& ray, const Hit& hit) const
{
    return normal;
}

std::pair<Box,bool> Plane::Bounding_Box(int part) const
{
    Box b;
    b.Make_Full();
    return {b,true};
}

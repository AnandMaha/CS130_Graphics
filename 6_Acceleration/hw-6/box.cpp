#include <limits>
#include <float.h>
#include "box.h"
using namespace std;
// Return whether the ray intersects this box.
pair<bool, double> Box::Intersection(const Ray& ray) const
{
    double close_intersect = -1 * DBL_MAX;
    double far_intersect = DBL_MAX;
	vec3 ray_dist_low = (lo - ray.endpoint) / ray.direction;
	vec3 ray_dist_hi = (hi - ray.endpoint) / ray.direction;
	
	for (int i = 0; i < 3; i++) {
		close_intersect = max(close_intersect, min(ray_dist_low[i], ray_dist_hi[i]));
		far_intersect = min(far_intersect, max(ray_dist_low[i], ray_dist_hi[i]));
    }
    
    if (close_intersect > far_intersect) {
        return {false, -1};
    } else {
        if (close_intersect < 0){
            close_intersect = far_intersect;
        }
        return {close_intersect >= 0, close_intersect};
    }
}

// Compute the smallest box that contains both *this and bb.
Box Box::Union(const Box& bb) const
{
    Box box = *this;
    // compare lo hi for both boxes in each dimension, choose lower lo higher high
    box.Include_Point(bb.lo);
    box.Include_Point(bb.hi);
    return box;
}

// Compute the smallest box that contains both *this and bb.
Box Box::Intersection(const Box& bb) const
{
    Box box;
    TODO;
    return box;
}

// Enlarge this box (if necessary) so that pt also lies inside it.
void Box::Include_Point(const vec3& pt)
{
    // cycle through dimensions, if pt < lo or pt > hi, then change box
    for(int i = 0; i < 3; i++){
		if(pt[i] < lo[i]){
			lo[i] = pt[i];
		}
		if(pt[i] > hi[i]){
			hi[i] = pt[i];
		}
	}
}

// Create a box to which points can be correctly added using Include_Point.
void Box::Make_Empty()
{
    lo.fill(DBL_MAX);
    hi=-lo;
}

// Create a box that contains everything.
void Box::Make_Full()
{
    hi.fill(DBL_MAX);
    lo=-hi;
}

bool Box::Test_Inside(const vec3& pt) const
{
    for(int i=0;i<3;i++){
        if(pt[i]<lo[i] || pt[i]>hi[i]){
            return false;
		}
	}
    return true;
}

// Useful for debugging
std::ostream& operator<<(std::ostream& o, const Box& b)
{
    return o << "(lo: " << b.lo << "; hi: " << b.hi << ")";
}

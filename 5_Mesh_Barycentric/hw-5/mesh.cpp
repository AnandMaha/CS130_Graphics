#include "mesh.h"
#include <fstream>
#include <limits>
#include <string>
#include <algorithm>
#include <float.h>
#include <iostream>
using namespace std;
// Consider a triangle to intersect a ray if the ray intersects the plane of the
// triangle with barycentric weights in [-weight_tolerance, 1+weight_tolerance]
static const double weight_tolerance = 1e-4;

Mesh::Mesh(const Parse* parse, std::istream& in)
{
    std::string file;
    in>>name>>file;
    Read_Obj(file.c_str());
}

// Read in a mesh from an obj file.  Populates the bounding box and registers
// one part per triangle (by setting number_parts).
void Mesh::Read_Obj(const char* file)
{
    std::ifstream fin(file);
    if(!fin)
    {
        exit(EXIT_FAILURE);
    }
    std::string line;
    ivec3 e, t;
    vec3 v;
    vec2 u;
    while(fin)
    {
        getline(fin,line);

        if(sscanf(line.c_str(), "v %lg %lg %lg", &v[0], &v[1], &v[2]) == 3)
        {
            vertices.push_back(v);
        }

        if(sscanf(line.c_str(), "f %d %d %d", &e[0], &e[1], &e[2]) == 3)
        {
            for(int i=0;i<3;i++) e[i]--;
            triangles.push_back(e);
        }

        if(sscanf(line.c_str(), "vt %lg %lg", &u[0], &u[1]) == 2)
        {
            uvs.push_back(u);
        }

        if(sscanf(line.c_str(), "f %d/%d %d/%d %d/%d", &e[0], &t[0], &e[1], &t[1], &e[2], &t[2]) == 6)
        {
            for(int i=0;i<3;i++) e[i]--;
            triangles.push_back(e);
            for(int i=0;i<3;i++) t[i]--;
            triangle_texture_index.push_back(t);
        }
    }
    num_parts=triangles.size();
}

// Check for an intersection in the range [small_t,inf).  If there are
// multiple intersections, return the closest one.  If there are no
// intersections, return a Hit structure with dist<0.  If possible
// also compute hit.uv.  If part>=0, intersect only against part of the
// primitive.  This is only used for meshes, where part is the triangle
// index.  If part<0, intersect against all triangles.  For other
// primitives, part is ignored.
Hit Mesh::Intersection(const Ray& ray, int part) const
{
    // check part >= 0, return hit using part as triangle index
    if(part >= 0){
		return Intersect_Triangle(ray, part);
	} 
	else{ // part < 0, check intersect with all triangles, return lowest dist hit
		Hit hit(DBL_MAX, -1, {});
		for(unsigned i = 0; i < triangles.size(); i++){
			Hit tmpHit = Intersect_Triangle(ray, i);
			// replace with closest triangle found
			if(tmpHit.dist < hit.dist && tmpHit.dist > 0) 
				hit = tmpHit;
		}
		// if no intersection
		if(hit.dist == DBL_MAX)
			hit.dist = -1;
		return hit;
	}
}

// This is a helper routine whose purpose is to simplify the implementation
// of the Intersection routine.  It should test for an intersection between
// the ray and the triangle with index tri.  If an intersection exists,
// record the distance and return true.  Otherwise, return false.
// This intersection should be computed by determining the intersection of
// the ray and the plane of the triangle.  From this, determine (1) where
// along the ray the intersection point occurs (dist) and (2) the barycentric
// coordinates within the triangle where the intersection occurs.  The
// triangle intersects the ray if dist>small_t and the barycentric weights are
// larger than -weight_tolerance.  The use of small_t avoid the self-shadowing
// bug, and the use of weight_tol prevents rays from passing in between
// two triangles.
Hit Mesh::Intersect_Triangle(const Ray& ray, int tri) const
{
    // Get vertex coords
    ivec3 triangle = triangles[tri];
    vec3 a_ver = vertices[triangle[0]];
    vec3 b_ver = vertices[triangle[1]];
    vec3 c_ver = vertices[triangle[2]];
	
	// Get CB, CA, ray endpoint -> C
	vec3 CB = b_ver - c_ver;
	vec3 CA = a_ver - c_ver; 
	vec3 endC = ray.endpoint - c_ver;
	
	// Calc alpha, beta, gamma
	double alpha = dot(cross(ray.direction, CB), endC) / dot(cross(ray.direction, CB), CA); 
	double beta = dot(cross(ray.direction, CA), endC) / dot(cross(ray.direction, CA), CB);
	double gamma = 1 - alpha - beta;
	

	Hit hit(DBL_MAX, tri, {alpha,beta});
	hit.dist = (-1 * dot(cross(CB,CA), endC)) / dot(cross(CB,CA),ray.direction);
	if(alpha < -1*weight_tol || beta < -1*weight_tol || gamma < -1*weight_tol || hit.dist < small_t){
		hit.dist = -1;
		hit.triangle = -1;
		hit.uv = {};
	}
    return hit;
}

// Compute the normal direction for the triangle with index part.
vec3 Mesh::Normal(const Ray& ray, const Hit& hit) const
{
    assert(hit.triangle>=0);
    // Get vertex coords
    ivec3 triangle = triangles[hit.triangle];
    vec3 a_ver = vertices[triangle[0]];
    vec3 b_ver = vertices[triangle[1]];
    vec3 c_ver = vertices[triangle[2]];
    // Normal is CA cross CB 
    vec3 normal = cross( a_ver - c_ver , b_ver - c_ver  ).normalized();
    return normal;
}


std::pair<Box,bool> Mesh::Bounding_Box(int part) const
{
    if(part<0)
    {
        Box box;
        box.Make_Empty();
        for(const auto& v:vertices)
            box.Include_Point(v);
        return {box,false};
    }

    ivec3 e=triangles[part];
    vec3 A=vertices[e[0]];
    Box b={A,A};
    b.Include_Point(vertices[e[1]]);
    b.Include_Point(vertices[e[2]]);
    return {b,false};
}

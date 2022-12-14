#include "render_world.h"
#include "flat_shader.h"
#include "reflective_shader.h"
#include "object.h"
#include "light.h"
#include "ray.h"
#include "hit.h"
#include <float.h>
#include <iostream>
using namespace std;
extern bool enable_acceleration;

Render_World::~Render_World()
{
    for(auto a:all_objects) delete a;
    for(auto a:all_shaders) delete a;
    for(auto a:all_colors) delete a;
    for(auto a:lights) delete a;
}

// Find and return the Hit structure for the closest intersection.  Be careful
// to ensure that hit.dist>=small_t.
std::pair<Shaded_Object,Hit> Render_World::Closest_Intersection(const Ray& ray) const
{
	pair<Shaded_Object, Hit> hitPair;
	hitPair.second.dist = DBL_MAX; // guarantee first object 
    for(unsigned i = 0; i < objects.size(); i++){
		// send -1 part (meshes) to check intersection with all triangles
		Hit hitTemp = (this->objects[i]).object->Intersection(ray, -1);
		if((hitTemp.dist > 0) && (hitTemp.dist < hitPair.second.dist)){
			hitPair.first = this->objects[i];
			hitPair.second = hitTemp;
		}
	}
	if(hitPair.second.dist == DBL_MAX)
		hitPair.second.dist = -1;
    return hitPair;
}

// set up the initial view ray and call
void Render_World::Render_Pixel(const ivec2& pixel_index)
{
    // compute ray -- endpoint is camera position, direction is z - endpoint
    vec3 e = camera.position; 
    vec3 z = camera.World_Position(pixel_index);
    Ray ray(e, z - e); 

    // shade ray
    vec3 color=Cast_Ray(ray,recursion_depth_limit); 
	
    // set the r,g,b from [0,1]
    camera.Set_Pixel(pixel_index,Pixel_Color(color)); 
}

void Render_World::Render()
{
	if(enable_acceleration){
		// Acceleration -- Add all objects to its structure
		for(unsigned i = 0; i < objects.size(); i++){
			acceleration.Add_Object(objects[i].object, i);
		}
		acceleration.Initialize();
	}
    for(int j=0;j<camera.number_pixels[1];j++)
        for(int i=0;i<camera.number_pixels[0];i++)
            Render_Pixel(ivec2(i,j));
}

// cast ray and return the color of the closest intersected surface point,
// or the background color if there is no object intersection
vec3 Render_World::Cast_Ray(const Ray& ray,int recursion_depth) const
{
	if(recursion_depth == 0){
		vec3 black(0,0,0);
		return black;
	}
	
	//intersect ray with object
	if(enable_acceleration){
		pair  <int, Hit> hitPair = acceleration.Closest_Intersection(ray);
		vec3 rayPoint = ray.Point(hitPair.second.dist);
	
		if(hitPair.second.dist > 0)	{
			vec3 normal = objects[hitPair.first].object->Normal(ray, hitPair.second); // must calc normal with actual object intersected
			vec3 color = objects[hitPair.first].shader->Shade_Surface(*this,ray,hitPair.second,rayPoint,normal,recursion_depth); // get color of object intersected
			 
			return color;
		} else{ // no intersection
			vec3 normal(0,1,0);
			if(background_shader != nullptr) {//make sure background_shader isn't nullptr
				return background_shader->Shade_Surface(*this,ray,hitPair.second,rayPoint,normal,recursion_depth);
			}
			else{
				vec3 black(0,0,0);
				return black;
			}
		}
	} else{
		pair  <Shaded_Object, Hit> hitPair = this->Closest_Intersection(ray);
		vec3 rayPoint = ray.Point(hitPair.second.dist);
	
		if(hitPair.second.dist > 0)	{
			vec3 normal = hitPair.first.object->Normal(ray, hitPair.second); // must calc normal with actual object intersected
			vec3 color = hitPair.first.shader->Shade_Surface(*this,ray,hitPair.second,rayPoint,normal,recursion_depth); // get color of object intersected
			 
			return color;
		} else{ // no intersection
			vec3 normal(0,1,0);
			if(background_shader != nullptr) {//make sure background_shader isn't nullptr
				return background_shader->Shade_Surface(*this,ray,hitPair.second,rayPoint,normal,recursion_depth);
			}
			else{
				vec3 black(0,0,0);
				return black;
			}
		}
	}
	
}

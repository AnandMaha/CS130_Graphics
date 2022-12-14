#include "transparent_shader.h"
#include "parse.h"
#include "ray.h"
#include "render_world.h"
#include <math.h>
#include <iostream>
using namespace std;

Transparent_Shader::
Transparent_Shader(const Parse* parse,std::istream& in)
{
    in>>name>>index_of_refraction>>opacity;
    shader=parse->Get_Shader(in);
    assert(index_of_refraction>=1.0);
}

// Use opacity to determine the contribution of this->shader and the Schlick
// approximation to compute the reflectivity.  This routine shades transparent
// objects such as glass.  Note that the incoming and outgoing indices of
// refraction depend on whether the ray is entering the object or leaving it.
// You may assume that the object is surrounded by air with index of refraction
// 1.
vec3 Transparent_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
	vec3 color;
	
	vec3 color_surface = shader->Shade_Surface(render_world, ray, hit, intersection_point, normal, recursion_depth);

    // calculate reflection ray 
    vec3 reflect_dir = 2 * dot(ray.direction * -1, normal) * normal - (ray.direction * -1);
    
    
    const double AIR_IDX_REFRACT = 1.0;
    double idx_dest, idx_src, cos_view_norm;
    bool use_transparent = false;
    vec3 refract_vector;
    if(dot(ray.direction, normal) < 0){ // ray outside object
		idx_src = AIR_IDX_REFRACT; idx_dest = index_of_refraction; 
		
		double refract_ratio = idx_src / idx_dest;
		cos_view_norm = dot(normal, ray.direction * -1);
		double tempCalc = 1.0 - (refract_ratio*refract_ratio*(1-cos_view_norm*cos_view_norm));
		
		if(tempCalc < 0){
			use_transparent = false;
		} else{
			use_transparent = true;
			tempCalc = sqrt(tempCalc);
			refract_vector = (refract_ratio * cos_view_norm - tempCalc) * normal - 
				refract_ratio * ray.direction * -1;
		}
		
	} else { // ray inside object
		idx_src = index_of_refraction; idx_dest = AIR_IDX_REFRACT; 
		
		double refract_ratio = idx_src / idx_dest;
		cos_view_norm = dot(normal*-1, ray.direction);
		double tempCalc = 1 - (refract_ratio*refract_ratio*(1-cos_view_norm*cos_view_norm));
		
		if(tempCalc < 0){
			use_transparent = false;
		} else{
			use_transparent = true;
			tempCalc = sqrt(tempCalc);
			refract_vector = refract_ratio * (ray.direction - normal * -1 * cos_view_norm) - 
				tempCalc * normal * -1;
		}
		
		//check no internal reflection
		if(use_transparent)
			cos_view_norm = dot(normal.normalized(), refract_vector);
		else{ // complete internal reflection
			Ray reflect_ray = Ray(intersection_point + (reflect_dir.normalized()) * small_t, reflect_dir);
			color = opacity * color_surface + (1 - opacity) * render_world.Cast_Ray(reflect_ray, recursion_depth-1);
			
			return color;
		}
	}
	
	// Both reflect and transparent
    double r_0 = pow( (idx_src - idx_dest) / (idx_dest + idx_src) , 2);
    double schlick = r_0 + (1 - r_0) * pow(1 - cos_view_norm, 5);
    
    Ray reflect_ray = Ray(intersection_point + (reflect_dir.normalized()) * small_t, reflect_dir);
    // transparent ray color
    Ray transparent_ray = Ray(intersection_point + (refract_vector.normalized()) * small_t, refract_vector);
	color = opacity * color_surface + (1 - opacity) * schlick * render_world.Cast_Ray(reflect_ray, recursion_depth-1);
	if(use_transparent){
		color = opacity * color_surface + (1 - opacity) * (schlick * render_world.Cast_Ray(reflect_ray, recursion_depth-1) + (1 - schlick) * render_world.Cast_Ray(transparent_ray, recursion_depth-1));
	}
   
    return color;
}

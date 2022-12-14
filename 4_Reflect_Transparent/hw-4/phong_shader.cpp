#include "light.h"
#include "parse.h"
#include "vec.h"
#include "object.h"
#include "phong_shader.h"
#include "ray.h"
#include <math.h>
#include "render_world.h"
#include <iostream>
using namespace std;

Phong_Shader::Phong_Shader(const Parse* parse,std::istream& in)
{
    in>>name;
    color_ambient=parse->Get_Color(in);
    color_diffuse=parse->Get_Color(in);
    color_specular=parse->Get_Color(in);
    in>>specular_power;
}

vec3 Phong_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    vec3 color;
    // add ambient light for all ambient lights
    // add diffuse + specular for all point lights 
    if(render_world.ambient_color != nullptr){ // ambient light has no name so check 0 length name
		color += color_ambient->Get_Color(hit.uv) * render_world.ambient_color->Get_Color(hit.uv) * render_world.ambient_intensity;
	} 
    
    for(unsigned i = 0; i < render_world.lights.size(); i++){
		const Light* light = render_world.lights[i];
		vec3 lightVec = light->position - intersection_point;
		Ray rayLight(intersection_point + ((lightVec.normalized())*small_t), lightVec.normalized());
		
		double intersectDist = render_world.Closest_Intersection(rayLight).second.dist;
		
		// check if shadows are enabled or not
		// if yes, Can the intersection point see the light? Or is an object blocking it?
		// if there's an object farther than the light source , or no object intersect
		if(render_world.enable_shadows && ( (intersectDist > 0) && (intersectDist <= lightVec.magnitude()) )){}
		else{
			vec3 lightVecNorm = lightVec.normalized();
			color += color_diffuse->Get_Color({}) * light->Emitted_Light(lightVec) * max(dot(normal.normalized(), lightVec.normalized()), 0.0) + 
				color_specular->Get_Color({}) * light->Emitted_Light(lightVec) * 
				pow(max(dot((ray.direction*-1).normalized(), (2*dot(lightVecNorm,normal.normalized())*normal.normalized()-lightVecNorm).normalized()), 0.0), specular_power);
		}
	
    }
    return color;
}

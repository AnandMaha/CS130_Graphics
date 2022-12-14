#include "reflective_shader.h"
#include "parse.h"
#include "ray.h"
#include "vec.h"
#include "render_world.h"
#include <iostream>
using namespace std;

Reflective_Shader::Reflective_Shader(const Parse* parse,std::istream& in)
{
    in>>name;
    shader=parse->Get_Shader(in);
    in>>reflectivity;
    reflectivity=std::max(0.0,std::min(1.0,reflectivity));
}

vec3 Reflective_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    vec3 color;
    vec3 color_surface = shader->Shade_Surface(render_world, ray, hit, intersection_point, normal, recursion_depth);
    
    // calculate reflection ray color
    vec3 reflect_dir = 2 * dot(ray.direction * -1, normal) * normal - (ray.direction * -1);
    Ray reflect_ray = Ray(intersection_point + (reflect_dir.normalized()) * small_t, reflect_dir);
	color = (1 - reflectivity) * color_surface + reflectivity * render_world.Cast_Ray(reflect_ray, recursion_depth - 1);
   
    return color;
}

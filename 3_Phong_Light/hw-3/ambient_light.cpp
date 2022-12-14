#include "ambient_light.h"
#include "parse.h"
#include "color.h"

Ambient_Light::Ambient_Light(const Parse* parse,std::istream& in)
{
    color=parse->Get_Color(in);
    in>>modifier;
}

vec3 Ambient_Light::Emitted_Light(const vec3& vector_to_light) const
{
    return color->Get_Color({})*modifier;
}

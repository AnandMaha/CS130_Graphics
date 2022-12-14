#ifndef __AMBIENT_LIGHT_H__
#define __AMBIENT_LIGHT_H__

#include <math.h>
#include <vector>
#include <iostream>
#include <limits>
#include "vec.h"
#include "light.h"

class Color;

class Ambient_Light : public Light
{
public:
    const Color* color = nullptr; // RGB color components
    double modifier = 0;

    Ambient_Light(const Parse* parse,std::istream& in);
    virtual ~Ambient_Light() = default;

    virtual vec3 Emitted_Light(const vec3& vector_to_light) const override;

    static constexpr const char* parse_name = "ambient_light";
};
#endif

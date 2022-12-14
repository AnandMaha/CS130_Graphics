#ifndef __COMMON__
#define __COMMON__

#include "vec.h"
#include <fstream>
#include <string>

struct Curve
{
    vec2 A,B,C,D;
};

vec2 Evaluate(const Curve& c, double t, Curve* sub_a = nullptr,
    Curve* sub_b = nullptr);

void Dump_Curve(const std::string& filename, const Curve& c, int n);

void Dump_Subdivision(const std::string& filename, const Curve& c, double t);

// Assumes everything is contained in the box [0,1] x [0,1].
class Dump_Eps
{
    std::ofstream fout;
public:

    Dump_Eps(const std::string& filename);
    ~Dump_Eps();

    void Print_Curve(const Curve& c);
    void Print_Point(const vec2& pt);
    void Print_Segment(const vec2& A, const vec2& B);

    void Set_Thickness(double t);
    void Set_Color(double r, double g, double b);

private:
    void Emit(const vec2& pt, const char* str=" ");
};




#endif

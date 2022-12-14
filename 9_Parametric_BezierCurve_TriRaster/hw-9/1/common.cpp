#include "common.h"

const static double scale=100;

Dump_Eps::Dump_Eps(const std::string& filename)
    :fout(filename)
{
    fout<<"%!PS-Adobe-3.0 EPSF-3.0"<<std::endl;
    fout<<"%%BoundingBox: 0 0 "<<scale<<" "<<scale<<std::endl;
    Set_Thickness(0.002*scale);
    Set_Color(0,0,0);
}

Dump_Eps::~Dump_Eps()
{
    fout<<"showpage"<<std::endl;
}

void Dump_Eps::Emit(const vec2& pt, const char* str)
{
    fout<<pt[0]*scale<<" "<<pt[1]*scale<<str;
}

void Dump_Eps::Print_Curve(const Curve& c)
{
    Emit(c.A," moveto ");
    Emit(c.B);
    Emit(c.C);
    Emit(c.D," curveto stroke\n");
}

void Dump_Eps::Print_Point(const vec2& pt)
{
    fout<<"newpath ";
    Emit(pt," ");
    fout<<0.005*scale<<" 0 360 arc closepath fill\n";
}

void Dump_Eps::Print_Segment(const vec2& A, const vec2& B)
{
    Emit(A," moveto ");
    Emit(B," lineto stroke\n");
}

void Dump_Eps::Set_Thickness(double t)
{
    fout<<t<<" setlinewidth"<<std::endl;
}

void Dump_Eps::Set_Color(double r, double g, double b)
{
    fout<<r<<" "<<g<<" "<<b<<" setrgbcolor"<<std::endl;
}

void Dump_Curve(const std::string& filename, const Curve& c, int n)
{
    Dump_Eps e(filename);
    e.Print_Curve(c);
    e.Set_Color(0,0,1);
    e.Print_Point(c.A);
    e.Print_Point(c.B);
    e.Print_Point(c.C);
    e.Print_Point(c.D);
    e.Print_Segment(c.A,c.B);
    e.Print_Segment(c.B,c.C);
    e.Print_Segment(c.C,c.D);
    e.Set_Color(1,0,0);
    for(int i=0;i<=n;i++)
        e.Print_Point(Evaluate(c,(double)i/n));
}

void Dump_Subdivision(const std::string& filename, const Curve& c, double t)
{
    Curve a,b;
    vec2 P=Evaluate(c,t,&a,&b);

    Dump_Eps e(filename);
    e.Set_Thickness(0.006*scale);
    e.Print_Point(P);    

    e.Set_Color(0,0,1);
    e.Print_Curve(c);
    e.Set_Thickness(0.002*scale);

    e.Set_Color(1,0,0);
    e.Print_Curve(a);
    e.Print_Point(c.A);

    e.Set_Color(0,1,0);
    e.Print_Curve(b);
    e.Print_Point(c.D);
}

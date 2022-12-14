#include <vector>
#include <string>
#include <iostream>
#include <random>
#include "vec.h"

static std::random_device rd;
static std::mt19937 mt(rd());

static std::uniform_real_distribution<> dist(0, 1);
inline double get_rand(){return dist(mt);}

static std::uniform_int_distribution<std::mt19937::result_type> idist(0, std::mt19937::max());
inline unsigned rand_int(){return idist(mt);}

template<int n> void fill_rand(vec<double,n>& u,double a,double b)
{
    for(int i=0;i<n;i++) u[i]=get_rand()*(b-a)+a;
}

struct Triangle
{
    vec4 A,B,C;
};

inline void fill_rand(Triangle& t)
{
    fill_rand(t.A,-1,1);
    fill_rand(t.B,-1,1);
    fill_rand(t.C,-1,1);
}

std::vector<Triangle> Clip(const Triangle& tri);

inline bool in(const vec4& p)
{
    for(int i=0;i<3;i++)
        if(std::abs(p[i])>1.00001*p[3])
            return false;
    return true;
}

inline vec3 to_vec3(const vec4& p)
{
    return vec3(p[0],p[1],p[2])/p[3];
}

inline bool pt_in_tri(const Triangle& t, const vec4& p)
{
    vec3 ta=to_vec3(t.A),u=to_vec3(t.B)-ta,v=to_vec3(t.C)-ta,w=to_vec3(p)-ta;
    vec3 n=cross(u,v);
    double a=dot(w,cross(v,n))/dot(n,n);
    double b=dot(w,cross(n,u))/dot(n,n);
    double c=std::abs(dot(n,w));
    return c<1e-4 && a>=-1e-4 && b>=-1e-4 && a+b<=1+1e-4;
}

int main(int argc, char* argv[])
{
    bool pass = true;
    for(int i=0;i<1000;i++)
    {
        Triangle it;
        fill_rand(it);

        auto ct=Clip(it);

        // Make sure the clipped triangles lie entirely inside the clipping volume.
        for(const auto& t:ct)
        {
            if(!in(t.A) || !in(t.B) || !in(t.C))
            {
                std::cout << "vertex of clipped triangle is outside clipping volume" << std::endl;
                std::cout << "input "  << it.A << " " << it.B << " " << it.C << std::endl;
                std::cout << "output "  << t.A << " " << t.B << " " << t.C << std::endl;
                pass = false;
                break;
            }
        }
        if(!pass) break;
        
        // Sample a bunch of points on the original triangle and discard the
        // ones that are not in the clipping volume.  Verify that the rest lie
        // inside some clipped triangle.
        for(int j=0;j<100;j++)
        {
            double a=get_rand()*.9+0.05,b=(get_rand()*.9+0.05)*(1-a);
            vec4 pt=it.A+a*(it.B-it.A)+b*(it.C-it.A);
            if(!in(pt)) continue;
            bool ok=false;
            for(const auto& t:ct)
                if(pt_in_tri(t,pt))
                    ok=true;
            if(!ok)
            {
                std::cout << "point " << pt << " is inside original triangle and inside clipping volume but not inside any of the clipped triangles."<<std::endl;
                std::cout << "input "  << it.A << " " << it.B << " " << it.C << std::endl;
                pass = false;
                break;
            }
        }
        if(!pass) break;
    }

    if(pass) std::cout<<"PASS"<<std::endl;
    else std::cout<<"FAIL"<<std::endl;

    // Detect early termination.
    if(argv[1]) std::cout<<"TOKEN "<<argv[1]<<std::endl;
    
    return 0;
}

#include <cassert>
#include <random>
#include "common.h"

static std::random_device rd;
static std::mt19937 mt(rd());

static std::uniform_real_distribution<> dist(0, 1);
inline double get_rand(){return dist(mt);}

static std::uniform_int_distribution<std::mt19937::result_type> idist(0, std::mt19937::max());
inline unsigned rand_int(){return idist(mt);}

template<int d>
inline void fill_random(vec<double,d>& v)
{
    for(int i=0;i<d;i++) v[i]=get_rand();
}

inline void fill_random(Curve& c)
{
    fill_random(c.A);
    fill_random(c.B);
    fill_random(c.C);
    fill_random(c.D);
}

bool Test_Equal(const vec2& u, const vec2& v, double tol=1e-8)
{
    return (u-v).magnitude_squared()<tol*tol;
}

int main(int argc, char* argv[])
{
    bool pass = true;

    // Check that the Bezier curves satisfy some of their basic properties.
    for(int i=0;i<1000000 && pass;i++)
    {
        Curve c;
        fill_random(c);

        double t=get_rand();

        // Bezier curves are symmetrical.
        Curve rev={c.D,c.C,c.B,c.A};
        if(!Test_Equal(Evaluate(c,t),Evaluate(rev,1-t)))
        {
            std::cout<<"fail symmetry test"<<std::endl;
            Dump_Curve("symmetry.eps",rev,10);
            pass=false;
        }

        // Make sure the Bezier curve ends at its endpoints.
        if(!Test_Equal(Evaluate(c,0),c.A) || !Test_Equal(Evaluate(c,1),c.D))
        {
            std::cout<<"fail endpoint test"<<std::endl;
            pass=false;
        }

        // Test the derivative at the endpoints by (0) approximating it from the
        // definition of a derivative and (1) evaluating it from the control
        // points.
        double eps=1e-6;
        vec2 d0=(Evaluate(c,eps)-c.A)/eps;
        vec2 d1=(c.B-c.A)*3;
        if(!Test_Equal(d0,d1,1e-4))
        {
            std::cout<<"fail derivative"<<std::endl;
            pass=false;
        }

        Curve a,b;
        vec2 p=Evaluate(c,t,&a,&b);

        // Make sure the subdivided Bezier curves have correct endpoints.
        if(!Test_Equal(a.A,c.A) || !Test_Equal(a.D,b.A) ||
            !Test_Equal(b.D,c.D) || !Test_Equal(a.D,p))
        {
            std::cout<<"fail subdivision endpoint test"<<std::endl;
            pass=false;
        }

        double s=get_rand();
        // Sample the subdivided curves and make sure they match the original
        if(!Test_Equal(Evaluate(c,t*s),Evaluate(a,s)) ||
            !Test_Equal(Evaluate(c,t+(1-t)*s),Evaluate(b,s)))
        {
            std::cout<<"fail subdivision test"<<std::endl;
            pass=false;
        }

        // Helpful for debugging to see whether Evaluate is working.
        if(!pass)
        {
            Dump_Curve("curve.eps",c,10);
            Dump_Subdivision("subdivision.eps",c,t);
        }
    }

    if(pass) std::cout<<"PASS"<<std::endl;
    else std::cout<<"FAIL"<<std::endl;

    // Detect early termination.
    if(argv[1]) std::cout<<"TOKEN "<<argv[1]<<std::endl;
    
    return 0;
}

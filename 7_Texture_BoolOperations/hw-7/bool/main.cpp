#include <cassert>
#include <random>
#include "common.h"

static std::random_device rd;
static std::mt19937 mt(rd());

static std::uniform_real_distribution<> dist(0, 1);
inline double get_rand(){return dist(mt);}

static std::uniform_int_distribution<std::mt19937::result_type> idist(0, std::mt19937::max());
inline unsigned rand_int(){return idist(mt);}

int main(int argc, char* argv[])
{
	/*
	vector<Hit> a,b;
	cout << "here" << endl;
	a.push_back({"A",0});
	a.push_back({"A",1});
	a.push_back({"A",2});
	//a.push_back({"A",4});
	b.push_back({"A",1.5});
	b.push_back({"A",3});
	b.push_back({"A",5});
	cout << "here" << endl;
	vector<Hit> combine = Combine(a,b,type_union);
	cout << "here" << endl;
	for(unsigned i = 0; i < combine.size(); i++){
		cout << combine[i] << endl;
	}
	*///*
    bool pass = true;

    // Check to see if Test_Inside is at least potentially working before we use
    // it for further tests.
    for(int i=0;i<100 && pass;i++)
    {
        double t=get_rand();
        std::vector<Hit> hits;

        // No point lies in the empty interval.
        if(Test_Inside(hits,t))
        {
            std::cout<<"fail on () interval: "<<hits<<" vs "<<t<<std::endl;
            pass = false;
        }

        // The point t is in interval (a) when t>=a.
        hits.push_back({"A",get_rand()});
        if(Test_Inside(hits,t) != (t>=hits[0].dist))
        {
            std::cout<<"fail on (a) interval: "<<hits<<" vs "<<t<<std::endl;
            pass = false;
        }

        // The point t is in interval (a,b) when t>=a and t<=b.
        hits.push_back({"A",get_rand()});
        sort(hits);
        if(Test_Inside(hits,t) != (t>=hits[0].dist && t<=hits[1].dist))
        {
            std::cout<<"fail on (a,b) interval: "<<hits<<" vs "<<t<<std::endl;
            pass = false;
        }
    }

    // Negative distances can never be on a ray.
    for(int i=0;i<100 && pass;i++)
    {
        double t=-get_rand();
        std::vector<Hit> hits;
        for(int j=0;j<6;j++)
        {
            if(Test_Inside(hits,t))
            {
                std::cout<<"negative distance should never pass: "<<hits<<" vs "<<t<<std::endl;
                pass = false;
            }
            hits.push_back({"A",get_rand()});
            sort(hits);
        }
    }

    // Run the following tests lots of times to make sure we catch things
    for(int r=0;r<100 && pass;r++)
    {
        // We can test that the booleans are correct by sampling at 100
        // different values and performing the booleans on the sample points.
        int num_samples = 100;
        std::vector<double> dists;
        for(int i=0;i<num_samples;i++)
            dists.push_back(get_rand());

        struct inter_list
        {
            std::string name; // for debugging
            std::vector<Hit> hits; // intersection hit list for this object
            std::vector<bool> samples; // whether each sample point is inside object
        };

        // Maintain a list of objects.  Initially, we create 8 simple objects (6
        // intervals, two infinite intervals) named A-H.
        std::vector<inter_list> ts;
        for(int i=0;i<8;i++)
        {
            char buff[]="A";
            buff[0]='A'+i;
            inter_list il={buff};
            il.hits.push_back({buff,get_rand()});
            if(i<6)
                il.hits.push_back({buff,get_rand()});
            sort(il.hits);
            for(auto t:dists)
                il.samples.push_back(Test_Inside(il.hits,t));
            ts.push_back(il);
        }

        // Next, we will run a bunch of tests.  Each time, we randomly select
        // two objects and also randomly select which type of boolean to apply.
        // By comparing hits with samples, we can determine whether the result
        // is likely correct.  We then add the new object to the list, which
        // allows us to build up more complicated objects for later tests.
        for(int i=0;i<100;i++)
        {
            const auto& a=ts[rand_int()%ts.size()];
            const auto& b=ts[rand_int()%ts.size()];
            auto type=(bool_type)(rand_int()%3);
            inter_list il;
            if(type==type_intersect) il.name="intersect("+a.name+","+b.name+")";
            else if(type==type_union) il.name="union("+a.name+","+b.name+")";
            else if(type==type_difference) il.name="difference("+a.name+","+b.name+")";
            il.hits=Combine(a.hits,b.hits,type);
            for(int j=0;j<num_samples;j++)
            {
                bool sa=a.samples[j],sb=b.samples[j],sc=false;
                if(type==type_intersect) sc=sa&&sb;
                else if(type==type_union) sc=sa||sb;
                else if(type==type_difference) sc=sa&&!sb;
                il.samples.push_back(sc);
                if(sc != Test_Inside(il.hits,dists[j]))
                {
                    std::cout<<"fail on object "<<il.name<<"   list: "<<il.hits<<std::endl;
                    std::cout<<"A: "<<a.name<<"   list: "<<a.hits<<std::endl;
                    std::cout<<"B: "<<b.name<<"   list: "<<b.hits<<std::endl;
                    pass = false;
                    exit(1);
                }
            }
            ts.push_back(il);
        }
    }    

    if(pass) std::cout<<"PASS"<<std::endl;
    else std::cout<<"FAIL"<<std::endl;

    // Detect early termination.
    if(argv[1]) std::cout<<"TOKEN "<<argv[1]<<std::endl;
    //*/
    return 0;
}

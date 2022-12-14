#ifndef __COMMON__
#define __COMMON__

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
using namespace std;
struct Hit
{
    std::string object;
    double dist;
};

enum bool_type{type_intersect,type_union,type_difference};

std::vector<Hit> Combine(
    const std::vector<Hit>& a,
    const std::vector<Hit>& b,
    bool_type type);

bool Test_Inside(const std::vector<Hit>& a, double t);

inline bool hitDist(const Hit& a, const Hit& b) { return a.dist<b.dist;  }

inline void sort(std::vector<Hit>& hits)
{
    sort(hits.begin(), hits.end(), hitDist);
}

inline std::ostream& operator<<(std::ostream& o, const Hit& h)
{
    return o << "(" << h.object << " " << h.dist << ")";
}

inline std::ostream& operator<<(std::ostream& o, const std::vector<Hit>& h)
{
    o << "( ";
    for(unsigned i = 0; i < h.size(); i++) o << h[i] << " ";
    return o << ")";
}

#endif

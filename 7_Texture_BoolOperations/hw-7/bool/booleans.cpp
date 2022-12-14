#include "common.h"
#include <vector>
#include <algorithm>
#include <float.h>
#include <iostream>
using namespace std;
// enum bool_type{type_intersect,type_union,type_difference};
inline bool hit_pair_less(const pair<Hit,Hit>& a, const pair<Hit,Hit>& b) { return a.first.dist<b.first.dist;  }
inline bool hit_int_less(const pair<Hit,int>& a, const pair<Hit,int>& b) { return a.first.dist<=b.first.dist;  }
std::vector<Hit> Combine(
    const std::vector<Hit>& a,
    const std::vector<Hit>& b,
    bool_type type)
{
	vector<Hit> combined;
	
	// INTERSECT
    if(type == type_intersect){
		// get both a and b sorted together
		vector<pair<Hit,int>> diffVec; // 0 for a, 1 for b
		Hit prev;
		for(unsigned i = 0; i < a.size(); i++){
			Hit temp = {to_string(i), a[i].dist};
			if(i == 0 || temp.dist != prev.dist){
				prev = temp;
				diffVec.push_back(make_pair(temp,0));
			}
			else if(i > 0 && temp.dist == prev.dist){
				diffVec.pop_back(); // no dup enter/leave
				if(diffVec.size() > 0)
					prev = diffVec[diffVec.size()-1].first;
			}
			
			if(i == a.size() -1 && a.size() % 2 == 1){
				temp = {to_string(i+1), DBL_MAX};
				diffVec.push_back(make_pair(temp,1));
			}
		}
		for(unsigned i = 0; i < b.size(); i++){
			Hit temp = {to_string(i), b[i].dist};
			if(i == 0 || temp.dist != prev.dist){
				prev = temp;
				diffVec.push_back(make_pair(temp,1));
			}
			else if(i > 0 && temp.dist == prev.dist){
				diffVec.pop_back(); // no dup enter/leave
				if(diffVec.size() > 0 && diffVec[diffVec.size()-1].second == 1)
					prev = diffVec[diffVec.size()-1].first;
			}
			
			if(i == b.size() -1 && b.size() % 2 == 1){
				temp = {to_string(i+1), DBL_MAX};
				diffVec.push_back(make_pair(temp,1));
			}
		}
		sort(diffVec.begin(), diffVec.end(), hit_int_less);
		int a_track = 0;
		int b_track = 0;
		for(unsigned i = 0; i < diffVec.size(); i++){
			// check equal distances
			if(i < diffVec.size()-1 && diffVec[i].first.dist == diffVec[i+1].first.dist){
				// manip diffVec so the leave comes before the enter 
				bool first_enter = (stoi(diffVec[i].first.object) % 2 == 0);
				bool second_leave = (stoi(diffVec[i+1].first.object) % 2 == 1);
				if(first_enter && second_leave){
					iter_swap(diffVec.begin() + i, diffVec.begin() + i + 1);
				}
			}
			if(diffVec[i].second == 0){
				bool isEnter = (stoi(diffVec[i].first.object) % 2 == 0);
				if(isEnter){
					a_track++;
					if(b_track == 1){
						combined.push_back(diffVec[i].first);
					}
				} else{
					a_track--;
					if(b_track == 1){
						combined.push_back(diffVec[i].first);
					}
				}
			} else{
				bool isEnter = (stoi(diffVec[i].first.object) % 2 == 0);
				if(isEnter){
					b_track++;
					if(a_track == 1){
						combined.push_back(diffVec[i].first);
					}
				} else{
					b_track--;
					if(a_track == 1){
						combined.push_back(diffVec[i].first);
					}
				}
			}
		}
		while(combined.size() > 0 && combined.back().dist == DBL_MAX)
			combined.pop_back();
		for(unsigned i = 0; i < combined.size(); i++){
			combined[i].object = "A";
		}
	} 
	
	// UNION
	else if(type == type_union){
		vector<pair<Hit,Hit>> hit_pair;
		unsigned int i = 0;
		while(i < a.size()){
			if(a.size() % 2 == 0){
				hit_pair.push_back(make_pair(a[i], a[i+1]));
				i+=2;
			} else{ // inf. at the end
				if(i == a.size() - 1){
					Hit temp = {"A",DBL_MAX};
					hit_pair.push_back(make_pair(a[i], temp));
					i++;
				} else{
					hit_pair.push_back(make_pair(a[i], a[i+1]));
					i+=2;
				}
			}
		}
		i = 0;
		while(i < b.size()){
			if(b.size() % 2 == 0){
				hit_pair.push_back(make_pair(b[i], b[i+1]));
				i+=2;
			} else{ // inf. at the end
				if(i == b.size() - 1){
					Hit temp = {"A",DBL_MAX};
					hit_pair.push_back(make_pair(b[i], temp));
					i++;
				} else{
					hit_pair.push_back(make_pair(b[i], b[i+1]));
					i+=2;
				}
			}
		}
		sort(hit_pair.begin(), hit_pair.end(), hit_pair_less);
		vector<pair<Hit,Hit>> combine_pair;
		for(unsigned i = 0; i < hit_pair.size(); i++){
			if(!combine_pair.empty() && (hit_pair[i].first.dist <= combine_pair.back().second.dist)){
				combine_pair.back().second.dist = max(hit_pair[i].second.dist, combine_pair.back().second.dist);
			} else{
				combine_pair.push_back(hit_pair[i]);
			}
		}
		for(unsigned i = 0; i < combine_pair.size(); i++){
			combined.push_back(combine_pair[i].first);
			combined.push_back(combine_pair[i].second);
		}
		while(combined.size() > 0 && combined.back().dist == DBL_MAX)
			combined.pop_back();
	} 
	
	// DIFFERENCE
	else if(type == type_difference){
		// get both a and b sorted together
		vector<pair<Hit,int>> diffVec; // 0 for a, 1 for b
		
		Hit prev;
		for(unsigned i = 0; i < a.size(); i++){
			Hit temp = {to_string(i), a[i].dist};
			if(i == 0 || temp.dist != prev.dist){
				prev = temp;
				diffVec.push_back(make_pair(temp,0));
			}
			else if(i > 0 && temp.dist == prev.dist){
				diffVec.pop_back(); // no dup enter/leave
				if(diffVec.size() > 0)
					prev = diffVec[diffVec.size()-1].first;
			}
			
			if(i == a.size() -1 && a.size() % 2 == 1){
				temp = {to_string(i+1), DBL_MAX};
				diffVec.push_back(make_pair(temp,1));
			}
		}
		for(unsigned i = 0; i < b.size(); i++){
			Hit temp = {to_string(i), b[i].dist};
			if(i == 0 || temp.dist != prev.dist){
				prev = temp;
				diffVec.push_back(make_pair(temp,1));
			}
			else if(i > 0 && temp.dist == prev.dist){
				diffVec.pop_back(); // no dup enter/leave
				if(diffVec.size() > 0 && diffVec[diffVec.size()-1].second == 1)
					prev = diffVec[diffVec.size()-1].first;
			}
			
			if(i == b.size() -1 && b.size() % 2 == 1){
				temp = {to_string(i+1), DBL_MAX};
				diffVec.push_back(make_pair(temp,1));
			}
		}
		sort(diffVec.begin(), diffVec.end(), hit_int_less);
		int a_track = 0;
		int b_track = 0;
		for(unsigned i = 0; i < diffVec.size(); i++){
			// check equal distances
			if(i < diffVec.size()-1 && diffVec[i].first.dist == diffVec[i+1].first.dist){
				// manip diffVec so the leave comes before the enter 
				bool first_enter = (stoi(diffVec[i].first.object) % 2 == 0);
				bool second_leave = (stoi(diffVec[i+1].first.object) % 2 == 1);
				if(first_enter && second_leave){
					iter_swap(diffVec.begin() + i, diffVec.begin() + i + 1);
				}
			}
			if(diffVec[i].second == 0){
				bool isEnter = (stoi(diffVec[i].first.object) % 2 == 0);
				
				if(isEnter){
					a_track++;
					if(b_track == 0){
						combined.push_back(diffVec[i].first);
					}
				} else{
					a_track--;
					if(b_track == 0){
						combined.push_back(diffVec[i].first);
					}
				}
			} else{
				bool isEnter = (stoi(diffVec[i].first.object) % 2 == 0);
				
				if(isEnter){
					b_track++;
					if(a_track == 1){
						combined.push_back(diffVec[i].first);
					}
				} else{
					b_track--;
					if(a_track == 1){
						combined.push_back(diffVec[i].first);
					}
				}
			}
		}
		while(combined.size() > 0 && combined.back().dist == DBL_MAX)
			combined.pop_back();
		for(unsigned i = 0; i < combined.size(); i++){
			combined[i].object = "A";
		}
	} else{
		cout << "error" << endl;
	}
	sort(combined);
    return combined;
}

// Returns whether a point at distance t along a ray is inside an object whose
// hit list is given in a.
// hit string object , double dist
bool Test_Inside(const std::vector<Hit>& a, double t)
{
	if(t < 0)
		return false;
	unsigned i = 0;
	while(i < a.size()){
		if(a.size() % 2 == 0){
			if(t >= a[i].dist && t <= a[i+1].dist)
				return true;
			i+=2;
		} else{ // inf. at the end
			if(i == a.size() - 1){
				return (t >= a[i].dist);
			} else{
				if(t >= a[i].dist && t <= a[i+1].dist)
					return true;
				i+=2;
			}
		}
	}
    return false;
}

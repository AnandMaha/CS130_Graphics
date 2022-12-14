#include <vector>
#include <string>
#include <iostream>
#include "vec.h"
#include <iostream>
using namespace std;
struct Triangle
{
    vec4 A,B,C;
    Triangle(vec4 A, vec4 B, vec4 C) : A(A), B(B), C(C) {}
};

vec4 calcEdgePoint(vec4 A, vec4 B, int mod, int dimension) {
	double alpha = (-1 * A[dimension] + (mod * A[3]) ) / ((B[dimension] - A[dimension]) + (mod * (A[3] - B[3])));
	return alpha * (B - A) + A;
}

vector<Triangle> Clip_recurse(const Triangle& tri, vector<Triangle> vec_tri, int side){
	// all sides checked, push the fully clipped triangle on the vector
	if(side == 6){
		vec_tri.push_back(tri);
		return vec_tri;
	}
    vec4 A = tri.A, B = tri.B, C = tri.C;
	bool insideA, insideB, insideC;
	int mod, dimension;

	// for each side
	switch(side){
		case 0: // left
			insideA = A[3] < -1 * A[0] ? false : true;
			insideB = B[3] < -1 * B[0] ? false : true;
			insideC = C[3] < -1 * C[0] ? false : true;
			mod = -1;
			dimension = 0;
			break;
		case 1: // bottom
			insideA = A[3] < -1 * A[1] ? false : true;
			insideB = B[3] < -1 * B[1] ? false : true;
			insideC = C[3] < -1 * C[1] ? false : true;
			mod = -1;
			dimension = 1;
			break;
		case 2: // near
			insideA = A[3] < -1 * A[2] ? false : true;
			insideB = B[3] < -1 * B[2] ? false : true;
			insideC = C[3] < -1 * C[2] ? false : true;
			mod = -1;
			dimension = 2;  
			break;
		case 3: // right
			insideA = A[3] < A[0] ? false : true;
			insideB = B[3] < B[0] ? false : true;
			insideC = C[3] < C[0] ? false : true;
			mod = 1;
			dimension = 0;
			break;	
		case 4: // top
			insideA = A[3] < A[1] ? false : true;
			insideB = B[3] < B[1] ? false : true;
			insideC = C[3] < C[1] ? false : true;
			mod = 1;
			dimension = 1;
			break;
		case 5: // far
			insideA = A[3] < A[2] ? false : true;
			insideB = B[3] < B[2] ? false : true;
			insideC = C[3] < C[2] ? false : true;
			mod = 1;
			dimension = 2;
			break;
		
		default: // not possible
			break;
	}
	
	// all vertices outside
	if(!insideA && !insideB && !insideC){ 
		return vec_tri;
	} 
	// 1 vertex inside
	else if(insideA && !insideB && !insideC){
		vec4 temp1 = calcEdgePoint(A, B, mod, dimension);
		vec4 temp2 = calcEdgePoint(A, C, mod, dimension);
		return Clip_recurse(Triangle(A, temp1, temp2), vec_tri, side+1);
	} else if(!insideA && insideB && !insideC){
		vec4 temp1 = calcEdgePoint(B, A, mod, dimension);
		vec4 temp2 = calcEdgePoint(B, C, mod, dimension);
		return Clip_recurse(Triangle(temp1, B, temp2), vec_tri, side+1);
	} else if(!insideA && !insideB && insideC){
		vec4 temp1 = calcEdgePoint(C, A, mod, dimension);
		vec4 temp2 = calcEdgePoint(C, B, mod, dimension);
		return Clip_recurse(Triangle(temp1, temp2, C), vec_tri, side+1);
	} 
	// 2 vertices inside
	else if(insideA && insideB && !insideC){
		vec4 temp1 = calcEdgePoint(A, C, mod, dimension);
		vec4 temp2 = calcEdgePoint(B, C, mod, dimension);
		vector<Triangle> temp_vec = Clip_recurse(Triangle(A, B, temp1),vec_tri, side+1);
		return Clip_recurse(Triangle(B, temp1, temp2), temp_vec, side+1);
	} else if(insideA && !insideB && insideC){
		vec4 temp1 = calcEdgePoint(A, B, mod, dimension);
		vec4 temp2 = calcEdgePoint(C, B, mod, dimension);
		vector<Triangle> temp_vec = Clip_recurse(Triangle(A, temp1, C),vec_tri, side+1);
		return Clip_recurse(Triangle(temp1, temp2, C), temp_vec, side+1);
	} else if(!insideA && insideB && insideC){
		vec4 temp1 = calcEdgePoint(C, A, mod, dimension);
		vec4 temp2 = calcEdgePoint(B, A, mod, dimension);
		vector<Triangle> temp_vec = Clip_recurse(Triangle(temp2, B, C),vec_tri, side+1);
		return Clip_recurse(Triangle(temp2, C, temp1), temp_vec, side+1);
	}  
	// all vertices inside -- just move to next side
	else if(insideA && insideB && insideC){
		return Clip_recurse(tri, vec_tri, side+1);
	} 
	// all 8 combinations already checked
	else{
		cout << "error" << endl;
		return {};
	}
}

std::vector<Triangle> Clip(const Triangle& tri)
{
	vector<Triangle> vec_tri;
	// triangle, vector of triangles, side number (out of 6)
	return Clip_recurse(tri, vec_tri, 0);
}



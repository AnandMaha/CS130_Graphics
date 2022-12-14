#include <vector>
#include <string>
#include <iostream>
#include <float.h>
#include "common.h"
using namespace std;

double calc_area(vec2 A, vec2 B, vec2 C){
	// cross product = AB x AC = area
	return (B[0] - A[0]) * (C[1] - A[1]) - (B[1] - A[1]) * (C[0] - A[0]);
}

void Rasterize(Pixel* pixels, int width, int height, const std::vector<Triangle>& tris)
{
    for(int i = 0; i < width; i++){
		for(int j = 0; j < height; j++){
			double close_z = DBL_MAX; // z-buffer to render closest triangle
			for(unsigned t = 0; t < tris.size(); t++){
				// check if kth triangle is within pixel (i,j)
				Triangle tTri = tris[t];
				double w_a = tTri.A[3];
				double w_b = tTri.B[3];
				double w_c = tTri.C[3];
				tTri.A = tTri.A/w_a;
				tTri.B = tTri.B/w_b;
				tTri.C = tTri.C/w_c;
				vec2 a_xy(tTri.A[0], tTri.A[1]);
				vec2 b_xy(tTri.B[0], tTri.B[1]);
				vec2 c_xy(tTri.C[0], tTri.C[1]);
				vec2 ij(-1+2*((i+0.5)/width), -1+2*((j+0.5)/height));
				double area = calc_area(a_xy, b_xy, c_xy);
				double alpha = calc_area(ij, b_xy, c_xy) / area;
				double beta = calc_area(ij, c_xy, a_xy) / area;
				double gamma = 1 - alpha - beta;
				// perspective correct
				double k = alpha/w_a + beta/w_b + gamma/w_c;
				double p_alpha = alpha/w_a/k;
				double p_beta = beta/w_b/k;
				double p_gamma = gamma/w_c/k;
				// z-buffer
				double curr_z = alpha * tTri.A[2] + beta * tTri.B[2] + gamma * tTri.C[2];
				if(curr_z <= close_z && p_alpha >= 0 && p_beta >= 0 && p_gamma >= 0){
					close_z = curr_z;
					vec3 color = p_alpha*tTri.Ca + p_beta*tTri.Cb + p_gamma*tTri.Cc;
					set_pixel(pixels, width, height, i, j, color);
				}
			}
		}
	}
}

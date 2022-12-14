#include "common.h"
#include <cmath>
// Evaluate curve c at t and return the result.  If sub_a or sub_b are not null,
// perform subdivision on curve c at t as well.  If sub_a is not null, assign it
// the first piece (so that sub_a and c agree at their endpoint A.)  If sub_b is
// not null, assign it the second piece (so that sub_b and c agree at their
// endpoint D.)
vec2 Evaluate(const Curve& c, double t, Curve* sub_a, Curve* sub_b)
{
	vec2 eval_t(pow(1-t, 3) * c.A + 3 * pow(1-t, 2) * t * c.B + 
		3 * (1-t) * pow(t, 2) * c.C + pow(t, 3) * c.D);
	
	vec2 cb_cc_div = c.B + (c.C - c.B) * t;
	if(sub_a){
		sub_a->A = c.A;
		sub_a->B = c.A + (c.B - c.A) * t;
		sub_a->C = sub_a->B + (cb_cc_div - sub_a->B) * t;
		sub_a->D = eval_t;
	} if(sub_b){
		sub_b->A = eval_t;
		sub_b->C = c.D + (c.C - c.D) * (1-t);
		sub_b->B = sub_b->C + (cb_cc_div - sub_b->C) * (1-t);
		sub_b->D = c.D;
	} 
	return eval_t;
}


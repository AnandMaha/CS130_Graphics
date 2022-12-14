#include "acceleration.h"
#include "object.h"
#include "hit.h"
#include <float.h>
#include <iostream>
#include <algorithm>
using namespace std;
extern int acceleration_grid_size;

Acceleration::Acceleration()
{
    domain.Make_Empty();
    num_cells.fill(acceleration_grid_size);
}

// Add object to the acceleration structure.  The id is the index of obj in
// the render_world.objects list.  The caller will need this to find the
// object that was intersected.  This function identifies whether the object
// has an infinite bounding box and places it in either finite_objects or
// infinite_objects.  finite_objects is a "holding area" where the objects
// are accumulated until Initialize() is called.
void Acceleration::Add_Object(const Object* obj, int id)
{
	Primitive prim;
	prim.obj = obj;
	prim.part = -1;
	prim.id = id;
    // if obj's bounding box's second param is True, add it to infinite_objects
    if(obj->Bounding_Box(-1).second == true){
		infinite_objects.push_back(prim);
	} else{ // else add it to finite_objects
		if(obj->num_parts > 1){
			for(int i = 0; i < obj->num_parts; i++){
				prim.part = i;
				finite_objects.push_back(prim);
			}
		} else{
			finite_objects.push_back(prim);
		}
	}
}

// This is called after all objects have been added with Add_Object.  This
// routine most do a few things.  (1) Allocate the cells array to the
// appropriate size. (2) Ensure that domain and dx have been computed.  (3)
// Populate the cells array with the appropriate primitives.  (4) After this
// routine has been called, finite_objects is no longer needed and can be
// cleared.
void Acceleration::Initialize()
{
	if(finite_objects.empty()){
		no_finite = true;
		return;
	}
	
    // (1)
    cells.resize(num_cells[0] * num_cells[1] * num_cells[2]);
    
    // (2)
    // domain is bounding box of finite_objects
    // cycle through finite, union every box together, that will be domain
    vector <Box> finite_boxes;
    for(unsigned i = 0; i < finite_objects.size(); i++){
		Box tBox = finite_objects[i].obj->Bounding_Box(finite_objects[i].part).first;
		finite_boxes.push_back(tBox);
		domain = domain.Union(tBox);
	}
    // dx -- size of grid cell
    // use domain hi - lo for length, divide that by num_cells
    vec3 d_num_cells(num_cells[0], num_cells[1], num_cells[2]);
    dx = (domain.hi - domain.lo) / d_num_cells;
    
    // (3)
    // For each finite_object and its bounding box, determine 
    // Cell_Index hi lo, get grid index for all in grid, then flatindex 
    // into cells, push_back the object
    for(unsigned s = 0; s < finite_boxes.size(); s++){
		Box tBox = finite_boxes[s];
		ivec3 box_lo = Cell_Index(tBox.lo);
		ivec3 box_hi = Cell_Index(tBox.hi);
		for(int i = box_lo[0]; i <= box_hi[0]; i++){
			for(int j = box_lo[1]; j <= box_hi[1]; j++){
				for(int k = box_lo[2]; k <= box_hi[2]; k++){
					vector <Primitive>& cell_vector = Cell_Data(ivec3(i, j, k));
					cell_vector.push_back(finite_objects[s]);
				}
			}
		}
	}
    // (4)
    finite_objects.clear();
}


// This routine mirrors the corresponding routine in Render_World, though
// the return value is a bit different.  The integer is the id for the
// intersected object (the id that was passed to Add_Object when the object
// was registered).  The Hit return value has the same meaning as the
// Render_World version.  When acceleration is being used, Render_World will
// call this routine to do the work.  This routine should walk the
// acceleration structure along the ray to identify intersections, returning
// the closest.  Don't forget to check the infinite_objects.
std::pair<int,Hit> Acceleration::Closest_Intersection(const Ray& ray) const
{
    pair<int, Hit> hitPair;
	hitPair.second.dist = DBL_MAX; // guarantee first object
	
	// infinite objects
	for(unsigned i = 0; i < infinite_objects.size(); i++){
		// send -1 part (meshes) to check intersection with all triangles
		Hit hitTemp = infinite_objects[i].obj->Intersection(ray, infinite_objects[i].part);
		if((hitTemp.dist > 0) && (hitTemp.dist < hitPair.second.dist)){
			hitPair.first = infinite_objects[i].id;
			hitPair.second = hitTemp;
		}
	}
	
	// if no finite objects to render, exit
	if(no_finite){
		if(hitPair.second.dist == DBL_MAX)
			hitPair.second.dist = -1;
		return hitPair;
	}
	
	// check intersect with bounding boxes, only do intersect with those
	// Check ray intersect with domain
	double t;
	if(domain.Test_Inside(ray.endpoint)){
		t = 0;
	} else{
		t = domain.Intersection(ray).second;
	}
	if(t >= 0){
		ivec3 cell_idx = Cell_Index(ray.Point(t));
		while(true) {
			// check point in cells has an object
            vector <Primitive> cell_vec = Cell_Data(cell_idx);
            for(unsigned i = 0; i < cell_vec.size(); i++){
				Hit hitTemp = cell_vec[i].obj->Intersection(ray, cell_vec[i].part);
				if((hitTemp.dist > 0) && (hitTemp.dist < hitPair.second.dist)){
					hitPair.first = cell_vec[i].id;
					hitPair.second = hitTemp;
				}
			}
			
			pair<double,ivec3> new_cell = Calc_New_Cell(t, ray, cell_idx);
			double new_t = new_cell.first;
			ivec3 new_cell_idx = new_cell.second;
			if(new_t == DBL_MAX || new_t > hitPair.second.dist)
				break;
			
			cell_idx = new_cell_idx;
			t = new_t;
		}
	}
	
	if(hitPair.second.dist == DBL_MAX)
		hitPair.second.dist = -1;
    return hitPair;
}

// Calculate next ray distance and next cell index 
pair<double,ivec3> Acceleration::Calc_New_Cell(double t, const Ray& ray, ivec3 cell_idx) const {
    vec3 curr_point = ray.Point(t);
    double close_dist = DBL_MAX; // determine closer idx change
    ivec3 change_vec(0,0,0); // to add to cell_idx for new_cell_idx
    
    vec3 ray_dir = ray.direction;
    // check each dimension for what idx to move
    for (int i = 0; i < 3; ++i){
        // x,y,z direction cannot be parallel to its grid side
        if (ray_dir[i] > 0){
			double next_point = (cell_idx[i] + 1) * dx[i] + domain.lo[i];
			double point_dist_diff = (next_point - curr_point[i]) / ray_dir[i];
			if(point_dist_diff < close_dist){
				change_vec.make_zero(); // reset change_vec
				change_vec[i] = 1;
				close_dist = point_dist_diff; // update close_dist
			} else if(point_dist_diff == close_dist){
				change_vec[i] = 1;
			}
        }
        else if (ray_dir[i] < 0){
			double next_point = cell_idx[i] * dx[i] + domain.lo[i];
			double point_dist_diff = (next_point - curr_point[i]) / ray_dir[i];
			if(point_dist_diff < close_dist){
				change_vec.make_zero(); // reset change_vec
				change_vec[i] = -1;
				close_dist = point_dist_diff; // update close_dist
			} else if(point_dist_diff == close_dist){
				change_vec[i] = -1;
			}
        }
    }
    
    ivec3 new_cell_idx = cell_idx + change_vec;
    // check out of bounds
    for(int i = 0; i < 3; i++){
        if (new_cell_idx[i] < 0 || new_cell_idx[i] >= num_cells[i]) {
            return {DBL_MAX, ivec3(-1,-1,-1)};
        }
	}
	
    return {t + close_dist, new_cell_idx};
}

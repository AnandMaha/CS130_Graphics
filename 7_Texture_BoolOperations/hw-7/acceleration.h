#ifndef __ACCELERATE_H__
#define __ACCELERATE_H__

#include "box.h"
#include "hit.h"
#include "vec.h"
#include "misc.h"
#include <iosfwd>
#include <vector>
#include <iostream>
using namespace std;

class Object;

class Acceleration
{
    // This structure stores all of the information about a primitive (or part
    // of a primitive).  You may add entries to it if you like.
    struct Primitive
    {
        const Object* obj;
        int part;
        int id;
    };

    // Holding area for objects that are finite in size, before Initialize() is
    // called.  The acceleration structures cannot be constructed until all
    // objects are known because the full bounding box (member domain below) is
    // required to initialize the acceleration structures.
    std::vector<Primitive> finite_objects;

    // This list holds infinite objects.  These are not placed in the
    // acceleration structure, since they would effectively be placed in every
    // cell, which is wasteful.  It is better to simply loop through this list
    // and detect intersections with them separately.
    std::vector<Primitive> infinite_objects;

    // This is the grid storage area.  The outer std::vector is an array of
    // cells (as a flat array, rather like how we do pixels as a flat array).
    // The Flat_Index(...) function below can convert from a cell index (ivec3)
    // to an index into this std::vector.  You will mostly access this using the
    // Cell_Data(...) routines below.  The inner std::vector is a list of
    // primitives whose bounding boxes touch the cell.  This member is allocated
    // and populated by the Initialize() routine.
    std::vector<std::vector<Primitive>> cells;

    // This is the number of cells in the grid's acceleration structure.  By
    // default, this will be (40,40,40), which indicates a 40x40x40 grid, in
    // which case cells will have 64000 entries.  This can be changed with the
    // -z commandline option through the global variable acceleration_grid_size.
    // Changing this value is very useful for testing and debugging.
    ivec3 num_cells;

    // This is the size of each grid cell.  Note that the cells might not be
    // cubes; the sides may have different lengths.
    vec3 dx;

    // This is the bounding box of all of the finite objects.
    Box domain;
    
    // Quick check that there are no finite objects to render
    bool no_finite = false;

public:
    Acceleration();

    // Add object to the acceleration structure.  The id is the index of obj in
    // the render_world.objects list.  The caller will need this to find the
    // object that was intersected.  This function identifies whether the object
    // has an infinite bounding box and places it in either finite_objects or
    // infinite_objects.  finite_objects is a "holding area" where the objects
    // are accumulated until Initialize() is called.
    void Add_Object(const Object* obj, int id);

    // This is called after all objects have been added with Add_Object.  This
    // routine will do a few things.  (1) Allocate the cells array to the
    // appropriate size. (2) Ensure that domain and dx have been computed.  (3)
    // Populate the cells array with the appropriate primitives.  (4) After this
    // routine has been called, finite_objects is no longer needed and can be
    // cleared.
    void Initialize();

    // This routine mirrors the corresponding routine in Render_World, though
    // the return value is a bit different.  The integer is the id for the
    // intersected object (the id that was passed to Add_Object when the object
    // was registered).  The Hit return value has the same meaning as the
    // Render_World version.  When acceleration is being used, Render_World will
    // call this routine to do the work.  This routine should walk the
    // acceleration structure along the ray to identify intersections, returning
    // the closest.  Don't forget to check the infinite_objects.
    std::pair<int,Hit> Closest_Intersection(const Ray& ray) const;
private:
	// Calculate next ray distance and next cell index 
	pair<double,ivec3> Calc_New_Cell(double t, const Ray& ray, ivec3 cell_idx) const;
	
    // Given a location in space, identify the grid cell that contains it.
	ivec3 Cell_Index(const vec3& pt) const
	{
		// grid dimensions are (40 x 40 x 40)
		// determine loc based on pt within domain
		vec3 d_grid_index = abs((pt - domain.lo) / dx);
		ivec3 grid_index((int)d_grid_index[0],(int)d_grid_index[1],(int)d_grid_index[2]);
		if(grid_index[0] >= num_cells[0])
			grid_index[0] = num_cells[0] - 1;
		if(grid_index[1] >= num_cells[1])
			grid_index[1] = num_cells[1] - 1;
		if(grid_index[2] >= num_cells[2])
			grid_index[2] = num_cells[2] - 1;
		
		return grid_index;
	}

    // Given a grid index, compute the flattened index.
    int Flat_Index(const ivec3& i) const
    {
        return (i[2]*num_cells[1]+i[1])*num_cells[0]+i[0];
    }

    // Given a grid index, return the primitive list for the cell.
    std::vector<Primitive>& Cell_Data(const ivec3& i)
    {
        return cells[Flat_Index(i)];
    }
    const std::vector<Primitive>& Cell_Data(const ivec3& i) const
    {
        return cells[Flat_Index(i)];
    }
};

#endif

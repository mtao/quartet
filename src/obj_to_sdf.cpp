#include "make_signed_distance.h"
#include "read_obj.h"
#include "read_sdf.h"
#include "trimesh.h"
#include <sstream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <ctime>

int
main(int argc,
     char **argv)
{
    //
    // Parse input arguments
    //

    char* surfaceFile = NULL;
    bool failure = false, usage=false;

    if (argc < 2 || argc > 4) {
        usage = true;
        failure = true;
    }



    if (usage) {
        std::printf("usage: obj2sdf <input.obj> <dx> <output.sdf> "
                );
    }
    if (failure) {
        return 1;
    }


    //
    // Run quartet mesh generation
    //

    // Get a surface mesh
    std::vector<Vec3i> surf_tri;
    std::vector<Vec3f> surf_x;
    if (!read_objfile(surf_x, surf_tri, "%s", argv[1])) {
        std::printf("error reading [%s] as a .obj mesh\n", argv[1]);
        return 2;
    }
    if (surf_tri.size() < 4 || surf_x.size() < 4) {
        std::printf("Surface mesh is too small to have an interior.\n");
        return 3;
    }

    // Get the grid spacing
    float dx=1;
    if (sscanf(argv[2], "%f", &dx) != 1) {
        std::printf("error interpreting [%s] as a float grid spacing\n",
                    argv[2]);
        return 4;
    }

    // Find bounding box
    Vec3f xmin=surf_x[0], xmax=surf_x[0];
    for (size_t i=1; i<surf_x.size(); ++i)
        update_minmax(surf_x[i], xmin, xmax);
    
    // Build triangle mesh data structure
    TriMesh trimesh(surf_x, surf_tri);


    // Make the level set
	
	// Determining dimensions of voxel grid.
	// Round up to ensure voxel grid completely contains bounding box.
	// Also add padding of 2 grid points around the bounding box.
	// NOTE: We add 5 here so as to add 4 grid points of padding, as well as
	// 1 grid point at the maximal boundary of the bounding box 
	// ie: (xmax-xmin)/dx + 1 grid points to cover one axis of the bounding box
	Vec3f origin=xmin-Vec3f(2*dx);
	int ni = (int)std::ceil((xmax[0]-xmin[0])/dx)+5,
		nj = (int)std::ceil((xmax[1]-xmin[1])/dx)+5,
		nk = (int)std::ceil((xmax[2]-xmin[2])/dx)+5;
	
    SDF sdf(origin, dx, ni, nj, nk); // Initialize signed distance field.
    std::printf("making %dx%dx%d level set\n", ni, nj, nk);
    make_signed_distance(surf_tri, surf_x, sdf);
	
    write_sdffile(sdf,"%s",argv[3]);
	
    return 0;
}


#include "read_obj.h"
#include "make_tet_mesh.h"
#include "sdf.h"
#include "feature.h"
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

    SDF sdf(Vec3f(-1,-1,-1),1/25.0,50,50,50);
	
    for(int i = 0; i < sdf.phi.ni; ++i) {
        for(int j = 0; j < sdf.phi.nj; ++j) {
            for(int k = 0; k < sdf.phi.nk; ++k) {
                Vec3f p = sdf.dx * Vec3f(i,j,k) + sdf.origin;
                sdf.phi(i,j,k) = mag(p) - .3;
            }
        }
    }
	
    // Then the tet mesh
    TetMesh mesh;

    make_tet_mesh(mesh, sdf);

    // Write it out
    std::printf("Writing mesh to file: %s\n", "sphere.tet");
    bool result = mesh.writeToFile("sphere.tet");

    return result;
}


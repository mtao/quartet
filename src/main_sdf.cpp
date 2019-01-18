#include "make_tet_mesh.h"
#include "make_signed_distance.h"
#include "read_obj.h"
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

    bool usage = false, failure = false;
    bool feature = false, automatic = false, surface = false,
         intermediate = false, optimize = false, unsafe = false;
    char* featureFile = NULL, *surfaceFile = NULL;
    float angleThreshold;

    if (argc < 4 || argc > 12) {
        usage = true;
        failure = true;
    }

    for (int arg = 4; arg < argc && !failure; ++arg) {
        if (strcmp(argv[arg], "-f") == 0) {
            if (feature) {
                std::printf("Error: already specified feature file\n");
                usage = true;
                failure = true;
                break;
            }
            ++arg;
            if (arg >= argc || argv[arg][0] == '-') {
                std::printf("Error: Expected feature file after -f\n");
                usage = true;
                --arg;
            } else {
                feature = true;
                featureFile = argv[arg];
            }


        } else if (strcmp(argv[arg], "-s") == 0) {
            if (surface) {
                std::printf("Error: already specified surface output\n");
                usage = true;
                failure = true;
                break;
            }
            ++arg;
            if (arg >= argc || argv[arg][0] == '-') {
                std::printf("Error: Expected OBJ filename after -s\n");
                usage = true;
                --arg;
            } else {
                surface = true;
                surfaceFile = argv[arg];
            }

        } else if (strcmp(argv[arg], "-i") == 0) {
            if (intermediate) {
                std::printf("Warning: Already specified intermediate "
                            "outputs\n");
                usage = true;
            }
            intermediate = true;

        } else if (strcmp(argv[arg], "-o") == 0) {
            if (optimize) {
                std::printf("Warning: Already specified optimization\n");
                usage = true;
            }
            optimize = true;

        } else if (strcmp(argv[arg], "-u") == 0) {
            if (unsafe) {
                std::printf("Warning: Already specified unsafe option\n");
                usage = true;
            }
            unsafe = true;

        } else {
            std::printf("Error: Invalid input argument: %s\n", argv[arg]);
            usage = true;
            failure = true;
        }
    }

    if (feature && automatic) {
        std::printf("Error: Feature file and automatic feature generation "
                    "selected.  Disabling automatic feature generation "
                    "and using the given feature file.\n");
        automatic = false;
    }

    if (usage) {
        std::printf("usage: quartet_sdf <input.sdf> <dx> <output.tet> "
                    "[-f <feature.feat>] "
                    "[-s <surface.obj>] [-i] [-o] \n"
                    "  -f -- Match the features in the given "
                    "<feature.feat> file\n"
                    "  -s -- Output the surface of the tet mesh as an "
                    "OBJ file\n"
                    "  -i -- Output intermediate tet meshes from each "
                    "stage of the mesh generation algorithm.\n"
                    "  -o -- Enable mesh optimization.\n"
                    "  -u -- Enable unsafe feature-matching operations.\n");
    }
    if (failure) {
        return 1;
    }


    //
    // Run quartet mesh generation
    //

    std::unique_ptr<SDF> sdfptr;
    if (!read_sdffile(sdfptr, "%s", argv[1])) {
        std::printf("error reading [%s] as a .sdf implicit surface\n", argv[1]);
        return 2;
    }
    SDF& sdf = *sdfptr;
	
	
    // Then the tet mesh
    TetMesh mesh;

    // Read in feature file, if given
    if (feature || automatic) {
        
        FeatureSet featureSet;

        if (feature) {
            featureSet.readFromFile(featureFile);
        } else { // automatic
            std::printf("automatically generating features\n");
            featureSet.writeToFile("auto.feat");
            std::printf("automatically generated features written to "
                        "auto.feat\n");
        }

        clock_t time = clock();

        // Make tet mesh with features
        std::printf("making tet mesh\n");
        make_tet_mesh(mesh, sdf, featureSet, optimize, intermediate, unsafe);
        std::printf("done tet mesh\n");
        
        time = clock() - time;
        std::printf("mesh generation took %f seconds.\n", 
                    ((float)time)/CLOCKS_PER_SEC);
    } else {
        clock_t time = clock();

        // Make tet mesh without features
        std::printf("making tet mesh\n");
		make_tet_mesh(mesh, sdf, optimize, intermediate, unsafe);
		std::printf("done tet mesh\n");
        
        time = clock() - time;
        std::printf("mesh generation took %f seconds.\n", 
                    ((float)time)/CLOCKS_PER_SEC);
    }

    // Write it out
    std::printf("Writing mesh to file: %s\n", argv[3]);
    bool result = mesh.writeToFile(argv[3]);
    if (!result)
    {
        std::printf("ERROR: Failed to write mesh to file.\n");
    }

    // Strip off file extension for creating other files.
    std::string filenameStr(argv[3]);
    if (filenameStr.find_last_of('.') != std::string::npos)
    {
        filenameStr = filenameStr.substr(0, filenameStr.find_last_of('.'));
    }

    // Write out an "info" file for the mesh as well.
    std::stringstream ss;
    ss << filenameStr << ".info";
    std::printf("Writing mesh info to file: %s\n", ss.str().c_str());
    result = mesh.writeInfoToFile(ss.str().c_str());
    if (!result)
    {
        std::printf("ERROR: Failed to write mesh info to file.\n");
    }
    
    // Write out a Matlab file containing the dihedral angles of the mesh.
    std::vector<float> dihedralAngles;
    dihedralAngles.reserve(mesh.tSize()*6);
    for (size_t tIdx = 0; tIdx < mesh.tSize(); ++tIdx)
    {
        Tet currTet = mesh.getTet(tIdx);
        std::vector<float> currAngles;
        currTet.dihedralAngles(currAngles);
        for (size_t i = 0; i < currAngles.size(); ++i)
        {
            dihedralAngles.push_back(currAngles[i] * 180.0 / M_PI);
        }
    }
    std::stringstream ss2;
    ss2 << filenameStr << "_hist.m";
    std::ofstream out(ss2.str().c_str());
    if (out.good())
    {   
        std::printf("Writing histogram to file: %s\n", ss2.str().c_str());
        out << "histValues = [ ";
        for (size_t i = 0; i < dihedralAngles.size(); ++i)
        {
            out << dihedralAngles[i] << " ";
        }
        out << "];" << std::endl;
    }
    else
    {
        std::printf("Failed to write histogram file: %s\n", ss2.str().c_str());
    }

    // Write out the exterior as a trimesh, if desired.
    if (surface)
    {
        std::vector<Vec3f> surf_verts;
        std::vector<Vec3i> surf_tris;
        mesh.getBoundary(surf_verts, surf_tris);
        std::printf("Writing surface mesh to file: %s\n", surfaceFile);
        result = write_objfile(surf_verts, surf_tris, surfaceFile);
        if (!result)
        {
            std::printf("ERROR: Failed to write surface mesh file.\n");
        }
    }

    return 0;
}


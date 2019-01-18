# This file is shared for all assignments. It is meant to be included rather
# than executed directly.
CMAKE_MINIMUM_REQUIRED(VERSION 3.8.1)
PROJECT(Quartet)
set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_STANDARD_REQUIRED YES)


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )


SET(SRC 
    src/predicates.cpp           
    src/geometry_queries.cpp     
    src/sdf.cpp                  
    src/trimesh.cpp              
    src/tet_mesh.cpp             
    src/feature.cpp              
    src/read_obj.cpp             
    src/read_sdf.cpp             
    src/tet_quality.cpp          
    src/match_features.cpp       
    src/optimize_tet_mesh.cpp    
    src/make_signed_distance.cpp 
    src/make_tet_mesh.cpp        
    )

SET(CMDLINE_SRC 
    src/main.cpp
    )

SET(VIEWER_SRC 
    src/gluvi.cpp             
    src/view_tet.cpp          
    src/feature.cpp           
    src/read_obj.cpp          
    src/predicates.cpp        
    src/geometry_queries.cpp  
    src/tet_mesh.cpp          
    src/tet_quality.cpp
    )

#add_executable(main_2d_nogui ${MAIN2D_NOGUI_SRCS}  ${LIBIGL_EXTRA_SOURCES})
#set_property(TARGET main_2d_nogui PROPERTY CXX_STANDARD 17)
#target_link_libraries(main_2d_nogui ${LIBIGL_LIBRARIES} ${LIBIGL_EXTRA_LIBRARIES})
#
#add_executable(main_2d ${MAIN2D_SRCS} ${LIBIGL_EXTRA_SOURCES})
#set_property(TARGET main_2d PROPERTY CXX_STANDARD 17)
#target_link_libraries(main_2d ${LIBIGL_LIBRARIES} ${LIBIGL_EXTRA_LIBRARIES} ${OPENGL_gl_LIBRARY} glfw glad)




ADD_LIBRARY(quartet ${SRC})

ADD_EXECUTABLE(quartet_cmdline ${CMDLINE_SRC})
TARGET_LINK_LIBRARIES(quartet_cmdline quartet)

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(GLUT REQUIRED)
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
LINK_DIRECTORIES(quartet_viewer ${OPENGL_LIBRARY_DIRS})

ADD_EXECUTABLE(quartet_viewer ${VIEWER_SRC})
TARGET_LINK_LIBRARIES(quartet_viewer quartet ${OPENGL_LIBRARY} ${GLUT_LIBRARY})

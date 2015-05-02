
# CMakeLists.txt, generated gencmake.pl, on 2015/04/04 15:32:48
cmake_minimum_required( VERSION 2.8.8 )

# CMakeScripts or use the ones that come by default with CMake.
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

project( terrain )

# The version number.
set( terrain_MAJOR 3 )
set( terrain_MINOR 0 )
set( terrain_POINT 0 )

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(IS_64BIT 1)
endif ()

# Allow developer to select is Dynamic or static library built
set( LIB_TYPE STATIC )  # set default static
option( BUILD_SHARED_LIB "Build Shared Library" OFF )

if(CMAKE_COMPILER_IS_GNUCXX)
    set( WARNING_FLAGS -Wall )
endif(CMAKE_COMPILER_IS_GNUCXX)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") 
   set( WARNING_FLAGS "-Wall -Wno-overloaded-virtual" )
endif() 

if(WIN32 AND MSVC)
    # turn off various warnings - none needed in this compile
    set(WARNING_FLAGS "${WARNING_FLAGS} /wd4996")
    # C4244: 'argument' : conversion from 'int' to 'GLfloat'
    # foreach(warning 4244 4251 4267 4275 4290 4786 4305)
    # C4305: 'argument' : truncation from 'double' to 'GLfloat'
    foreach(warning 4244 4305)
        set(WARNING_FLAGS "${WARNING_FLAGS} /wd${warning}")
    endforeach()
    # C4267: '=' : conversion from 'size_t' to 'int', possible loss of data
    if (IS_64BIT)
        set(WARNING_FLAGS "${WARNING_FLAGS} /wd4267")
        add_definitions( -DWIN64 )  # add user define. compiler defines _WIN64
    endif ()
    set( MSVC_FLAGS "-DNOMINMAX -D_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -D__CRT_NONSTDC_NO_WARNINGS" )
    # if (${MSVC_VERSION} EQUAL 1600)
    #    set( MSVC_LD_FLAGS "/FORCE:MULTIPLE" )
    # endif (${MSVC_VERSION} EQUAL 1600)
    #set( NOMINMAX 1 )
    # to distinguish between debug and release libs
    set( CMAKE_DEBUG_POSTFIX "d" )
else()
    # items for unix
endif()

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_FLAGS} ${MSVC_FLAGS} -D_REENTRANT" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} ${MSVC_FLAGS} -D_REENTRANT" )
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MSVC_LD_FLAGS}" )

# OPENGL_FOUND True, if the system has OpenGL.
# OPENGL_XMESA_FOUND True, if the system has XMESA.
# OPENGL_GLU_FOUND  True, if the system has GLU.
# OPENGL_INCLUDE_DIR Path to the OpenGL include directory.
# OPENGL_LIBRARIES Paths to the OpenGL and GLU libraries.
find_package(OpenGL)
if (OPENGL_FOUND)
    message(STATUS "*** OpenGL found inc ${OPENGL_INCLUDE_DIR} lib ${OPENGL_LIBRARIES}")
    include_directories( ${OPENGL_INCLUDE_DIR} )
    list(APPEND add_LIBS ${OPENGL_LIBRARIES})
else ()
    message(FATAL_ERROR "*** OpenGL NOT FOUND!")
endif ()

#   GLUT_INCLUDE_DIR, where to find GL/glut.h, etc.
#   GLUT_LIBRARIES, the libraries to link against
#   GLUT_FOUND, If false, do not try to use GLUT.
find_package(GLUT)
if (GLUT_FOUND)
    message(STATUS "*** GLUT/OpenGLUT found inc ${GLUT_INCLUDE_DIR} lib ${GLUT_LIBRARIES}")
    include_directories( ${GLUT_INCLUDE_DIR} )
    list(APPEND add_LIBS ${GLUT_LIBRARIES})
    string(FIND "${GLUT_LIBRARIES}" "static" FREEGLUT_STATIC)
    if (FREEGLUT_STATIC)
        message(STATUS "*** add_definitions( -DFREEGLUT_STATIC )")
        add_definitions( -DFREEGLUT_STATIC )
    endif ()
    add_definitions( -DFREEGLUT_LIB_PRAGMAS=0 )
else ()
    message(FATAL_ERROR "*** GLUT, OpenGLUT NOT FOUND!")
endif ()

# GLEW_FOUND
# GLEW_INCLUDE_PATH
# GLEW_LIBRARY
# GLEW_LIBRARY_PATH     - path to GLEW library
# GLEW_DEFINITIONS - Any compiler definitions to add
find_package(GLEW)
if (GLEW_FOUND)
    message(STATUS "*** GLEW found inc ${GLEW_INCLUDE_PATH} lib ${GLEW_LIBRARY}")
    include_directories( ${GLEW_INCLUDE_PATH} )
    list(APPEND add_LIBS ${GLEW_LIBRARY})
    if (GLEW_DEFINITIONS)
        add_definitions( ${GLEW_DEFINITIONS} )
    endif ()
else ()
    message(STATUS "*** GLEW NOT FOUND!")
endif ()

#   GLM_INCLUDE_DIRS  - where to find glm/glm.hpp
#   GLM_FOUND         - if the library was successfully located
find_package(GLM)
if (GLM_FOUND)
    message(STATUS "*** GLM found inc ${GLM_INCLUDE_DIRS}")
    include_directories( ${GLM_INCLUDE_DIRS} )
else ()
    message(STATUS "*** GLM NOT FOUND!")
endif ()

#   FREETYPE_LIBRARIES, the library to link against
#   FREETYPE_FOUND, if false, do not try to link to FREETYPE
#   FREETYPE_INCLUDE_DIRS, where to find headers.
#   FREETYPE_VERSION_STRING, the version of freetype found (since CMake 2.8.8)
find_package(Freetype)
if (FREETYPE_FOUND)
    message(STATUS "*** Freetype ${FREETYPE_VERSION_STRING} found inc ${FREETYPE_INCLUDE_DIRS} lib ${FREETYPE_LIBRARIES}")
    include_directories( ${FREETYPE_INCLUDE_DIRS} )
    list(APPEND add_LIBS ${FREETYPE_LIBRARIES})
else ()
    message(STATUS "*** Freetype NOT FOUND!")
endif ()

#  FREEIMAGE_INCLUDE_DIR, where to find tiff.h, etc.
#  FREEIMAGE_LIBRARIES, libraries to link against to use TIFF.
#  FREEIMAGE_FOUND, If false, do not try to use TIFF.
find_package(FREEIMAGE)
if (FREEIMAGE_FOUND)
    message(STATUS "*** FreeImage found inc ${FREEIMAGE_INCLUDE_DIR} lib ${FREEIMAGE_LIBRARIES}")
    include_directories( ${FREEIMAGE_INCLUDE_DIR} )
    list(APPEND add_LIBS ${FREEIMAGE_LIBRARIES})
    # if using the static library
    add_definitions( -DFREEIMAGE_LIB )
else ()
    message(STATUS "*** FreeImage NOT FOUND!")
endif ()

#  ASSIMP_FOUND, If false, do not try to use Assimp.
#  ASSIMP_INCLUDE_DIR, where to find assimp/scene.h
#  ASSIMP_LIBRARIES, libraries to link against to use Assimp.
find_package(ASSIMP)
if (ASSIMP_FOUND)
    message(STATUS "*** Assimp found inc ${ASSIMP_INCLUDE_DIR} lib ${ASSIMP_LIBRARIES}")
    include_directories( ${ASSIMP_INCLUDE_DIR} )
    list(APPEND add_LIBS ${ASSIMP_LIBRARIES})
else ()
    message(STATUS "*** Assimp NOT FOUND!")
endif ()

# configuration file, if needed
# configure_file( ${CMAKE_SOURCE_DIR}/config.h.cmake ${CMAKE_BINARY_DIR}/config.h )
# add_definitions( -DHAVE_CONFIG_H )
# include_directories( ${CMAKE_BINARY_DIR} )

if(BUILD_SHARED_LIB)
   set(LIB_TYPE SHARED)
   message(STATUS "*** Building DLL library ${LIB_TYPE}")
else()
   message(STATUS "*** Option BUILD_SHARED_LIB is OFF ${LIB_TYPE}")
endif()
#################################################
##### LIBRARIES #####
#################################################

set(name terrutils)
set(dir src)
set(${name}_SRCS
    ${dir}/imageloader.cpp
    ${dir}/vec3f.cpp
    ${dir}/lodepng/lodepng.cpp
    ${dir}/Terrain.cpp
    ${dir}/texLoad.c
    )
set(${name}_HDRS
    ${dir}/imageloader.h
    ${dir}/vec3f.h
    ${dir}/lodepng/lodepng.h
    ${dir}/Terrain.h
    ${dir}/texLoad.h
    )
add_library( ${name} ${LIB_TYPE} ${${name}_SRCS} ${${name}_HDRS} )
list(APPEND add_LIBS ${name})

# general itility library
set(name genutils)
set(dir src/utils)
set(${name}_SRCS
    ${dir}/sprtf.cxx
    ${dir}/utils.cxx
    ${dir}/slippystuff.cxx
    ${dir}/dir_utils.cxx
    )
set(${name}_HDRS
    ${dir}/sprtf.hxx
    ${dir}/utils.hxx
    ${dir}/slippystuff.hxx
    ${dir}/dir_utils.hxx
    )
add_library( ${name} ${LIB_TYPE} ${${name}_SRCS} ${${name}_HDRS} )
list(APPEND add_LIBS ${name})

set(name gltzpr)
set(dir src/gltzpr)
set(${name}_SRCS
    ${dir}/zpr.c
    )
set(${name}_HDRS
    ${dir}/zpr.h
    )
add_library( ${name} ${LIB_TYPE} ${${name}_SRCS} ${${name}_HDRS} )
list(APPEND add_LIBS ${name})

#################################################
##### EXECUTABLE #####
#################################################
# gltZpr-0.4 EXECUTABLE from [F:\Projects\gltZpr-0.4],
# have 2 C/C++ sources, 1 rc, 2 headers
set(name zprdemo)
set(dir src/gltzpr)
set(${name}_SRCS
    ${dir}/zprdemo.c
    )
set(${name}_HDRS
    ${dir}/resource.h
    )
if (WIN32)
    list(APPEND ${name}_SRCS ${dir}/zpr.rc)
endif ()
add_executable( ${name} ${${name}_SRCS} ${${name}_HDRS} )
if (add_LIBS)
    target_link_libraries( ${name} ${add_LIBS} )
endif ()
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()
# deal with install, if any...
#install( TARGETS ${name} DESTINATION bin )

# src EXECUTABLE from [F:\Projects\terrain\src],
# have 3 C/C++ sources, 2 headers
set(DEF_BMP "${CMAKE_SOURCE_DIR}/data/heightmap.png")
# or set(DEF_BMP "${CMAKE_SOURCE_DIR}/data/heightmap.bmp")
if(EXISTS ${DEF_BMP})
    add_definitions( -DDEF_HEIGHTMAP="${DEF_BMP}" )
    message(STATUS "*** add_definitions( -DDEF_HEIGHTMAP=\"${DEF_BMP}\"" )
endif ()
set(name terrain)
set(dir src)
set(${name}_SRCS
    ${dir}/main.cpp
    )
set(${name}_HDRS
    ${dir}/resource.h
    )
if (WIN32)
    list(APPEND ${name}_SRCS ${dir}/${name}.rc)
endif ()
add_executable( ${name} ${${name}_SRCS} ${${name}_HDRS})
if (add_LIBS)
    target_link_libraries( ${name} ${add_LIBS} )
endif ()
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()
# deal with install, if any...
#install( TARGETS ${name} DESTINATION bin )

#########################################
##### This is a GOOD 'flat' version #####
#########################################
set(name terrain01)
set(dir src)
set(${name}_SRCS
    ${dir}/main01.cpp
    )
add_executable( ${name} ${${name}_SRCS} )
if (add_LIBS)
    target_link_libraries( ${name} ${add_LIBS} )
endif ()
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()
# deal with install, if any...
#install( TARGETS ${name} DESTINATION bin )

###############################################################
# Other examples 

#####################################################################################
### ML_Terrain - this is a WHOLE SCENE, but has lost of dependencies
if (FREEIMAGE_FOUND AND ASSIMP_FOUND AND FREETYPE_FOUND AND GLM_FOUND AND GLEW_FOUND AND WIN32)
    set(name ML_Terrain)
    set(dir src/ML_Terrain)
    # Project [Multilayered_Terrain], type [Application], 12 C/C++, 12 Hdrs, 0 Other, 0 INCS, 5 DEFS
    # File dir [C:\Users\user\Downloads\temp\temp\Multilayered_Terrain\Multilayered_Terrain\], out [C:\Users\user\Downloads\temp\temp\Multilayered_Terrain], rel = [Multilayered_Terrain\]
    set( ${name}_SRCS
        ${dir}/assimp_model.cpp
        ${dir}/dirLight.cpp
        ${dir}/flyingCamera.cpp
        ${dir}/freeTypeFont.cpp
        ${dir}/heightmap.cpp
        ${dir}/openGLControl.cpp
        ${dir}/renderScene.cpp
        ${dir}/shaders.cpp
        ${dir}/skybox.cpp
        ${dir}/texture.cpp
        ${dir}/vertexBufferObject.cpp
        ${dir}/win_OpenGLApp.cpp 
        ${dir}/utils.cpp
        )
    set( ${name}_HDRS
        ${dir}/assimp_model.h
        ${dir}/common_header.h
        ${dir}/dirLight.h
        ${dir}/flyingCamera.h
        ${dir}/freeTypeFont.h
        ${dir}/heightmap.h
        ${dir}/openGLControl.h
        ${dir}/shaders.h
        ${dir}/skybox.h
        ${dir}/texture.h
        ${dir}/vertexBufferObject.h
        ${dir}/win_OpenGLApp.h 
        ${dir}/utils.h
        )
    add_executable( ${name} WIN32 ${${name}_SRCS} ${${name}_HDRS} )
    target_Link_libraries(${name} ${add_LIBS} )
    if (MSVC)
        set_target_properties(${name} PROPERTIES DEBUG_POSTFIX d )
    endif ()
endif()

if (WIN32)
    include_directories( include )  # for glaux.h
    set(name Lesson3)
    set(dir src)
    set(${name}_SRCS
        ${dir}/${name}.cpp
        )
    add_executable( ${name} WIN32 ${${name}_SRCS} )
    if (add_LIBS)
        target_link_libraries( ${name} ${add_LIBS} )
    endif ()
    if (MSVC)
        set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
    endif ()
    # deal with install, if any...
    #install( TARGETS ${name} DESTINATION bin )
    
    set(name projections)
    set(dir src)
    set(${name}_SRCS
        ${dir}/${name}.cpp
        )
    add_executable( ${name} WIN32 ${${name}_SRCS} )
    if (add_LIBS)
        target_link_libraries( ${name} ${add_LIBS} )
    endif ()
    if (MSVC)
        set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
    endif ()
    # deal with install, if any...
    #install( TARGETS ${name} DESTINATION bin )
    
    set(DEF_BMP "${CMAKE_SOURCE_DIR}/data/Terrain2.bmp")
    if(EXISTS ${DEF_BMP})
        add_definitions( -DDEF_TERR2_BMP="${DEF_BMP}" )
        message(STATUS "*** add_definitions( -DDEF_TERR2_BMP=\"${DEF_BMP}\"" )
    endif ()
    set(DEF_BMP "${CMAKE_SOURCE_DIR}/data/green.bmp")
    if(EXISTS ${DEF_BMP})
        add_definitions( -DDEF_GREEN_TEX="${DEF_BMP}" )
        message(STATUS "*** add_definitions( -DDEF_GREEN_TEX=\"${DEF_BMP}\"" )
    endif ()
    set(name OpenGLWindow)
    set(dir src/${name})
    set(${name}_SRCS
        ${dir}/${name}.cpp
        )
    set(${name}_HDRS
        ${dir}/vkgllib.h
        )
    add_executable( ${name} WIN32 ${${name}_SRCS} ${${name}_HDRS})
    if (add_LIBS)
        target_link_libraries( ${name} ${add_LIBS} )
    endif ()
    if (MSVC)
        set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
    endif ()
    # deal with install, if any...
    #install( TARGETS ${name} DESTINATION bin )
    
endif ()

# This is a rather SIMPLE (flat) multiple view scene
set(name Lighthouse3D)
set(dir src/Lighthouse3D)
set(${name}_SRCS
    ${dir}/${name}.cpp
    )
add_executable( ${name} ${${name}_SRCS} )
if (add_LIBS)
    target_link_libraries( ${name} ${add_LIBS} )
endif ()
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()
# deal with install, if any...
#install( TARGETS ${name} DESTINATION bin )

set(name cube)
set(dir src)
set(${name}_SRCS
    ${dir}/${name}.c
    )
add_executable( ${name} ${${name}_SRCS} )
if (add_LIBS)
    target_link_libraries( ${name} ${add_LIBS} )
endif ()
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()
# deal with install, if any...
#install( TARGETS ${name} DESTINATION bin )

set(DEF_BMP "${CMAKE_SOURCE_DIR}/data/vtr.bmp")
if(EXISTS ${DEF_BMP})
    add_definitions( -DDEF_CUBE_TEX="${DEF_BMP}" )
    message(STATUS "*** add_definitions( -DDEF_CUBE_TEX=\"${DEF_BMP}\"" )
endif ()
set(name cube2)
set(dir src)
set(${name}_SRCS
    ${dir}/${name}.cpp
    )
add_executable( ${name} ${${name}_SRCS} )
if (add_LIBS)
    target_link_libraries( ${name} ${add_LIBS} )
endif ()
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()
# deal with install, if any...
#install( TARGETS ${name} DESTINATION bin )

set(name sphere1)
set(dir src)
set(${name}_SRCS
    ${dir}/${name}.cxx
    )
add_executable( ${name} ${${name}_SRCS} )
if (add_LIBS)
    target_link_libraries( ${name} ${add_LIBS} )
endif ()
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()
# deal with install, if any...
#install( TARGETS ${name} DESTINATION bin )

set(name sphere2)
set(dir src)
set(${name}_SRCS
    ${dir}/${name}.cxx
    )
add_executable( ${name} ${${name}_SRCS} )
if (add_LIBS)
    target_link_libraries( ${name} ${add_LIBS} )
endif ()
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()
# deal with install, if any...
#install( TARGETS ${name} DESTINATION bin )

set(name sphere3)
set(dir src)
set(${name}_SRCS
    ${dir}/${name}.cxx
    )
set(${name}_HDRS
    ${dir}/${name}.hxx
    )
add_executable( ${name} ${${name}_SRCS} ${${name}_HDRS} )
if (add_LIBS)
    target_link_libraries( ${name} ${add_LIBS} )
endif ()
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()
# deal with install, if any...
#install( TARGETS ${name} DESTINATION bin )

set(DEF_BMP "${CMAKE_SOURCE_DIR}/data/earth.raw")
if(EXISTS ${DEF_BMP})
    add_definitions( -DDEF_EARTH_RAW="${DEF_BMP}" )
    message(STATUS "*** add_definitions( -DDEF_EARTH_RAW=\"${DEF_BMP}\"" )
endif ()
set(name sphere4)
set(dir src)
set(${name}_SRCS
    ${dir}/${name}.cxx
    )
add_executable( ${name} ${${name}_SRCS} )
if (add_LIBS)
    target_link_libraries( ${name} ${add_LIBS} )
endif ()
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()
# deal with install, if any...
#install( TARGETS ${name} DESTINATION bin )

set(name sphere5)
set(dir src)
set(${name}_SRCS
    ${dir}/${name}.c
    )
add_executable( ${name} ${${name}_SRCS} )
if (add_LIBS)
    target_link_libraries( ${name} ${add_LIBS} )
endif ()
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()
# deal with install, if any...
#install( TARGETS ${name} DESTINATION bin )

set(DEF_BMP "${CMAKE_SOURCE_DIR}/data/earth.bmp")
if(EXISTS ${DEF_BMP})
    add_definitions( -DDEF_EARTH_BMP="${DEF_BMP}" )
    message(STATUS "*** add_definitions( -DDEF_EARTH_BMP=\"${DEF_BMP}\"" )
endif ()
set(name sphere6)
set(dir src)
set(${name}_SRCS
    ${dir}/${name}.cxx
    )
add_executable( ${name} ${${name}_SRCS} )
if (add_LIBS)
    target_link_libraries( ${name} ${add_LIBS} )
endif ()
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()
# deal with install, if any...
#install( TARGETS ${name} DESTINATION bin )

# eof

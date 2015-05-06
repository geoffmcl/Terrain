# FindASSIMP.cmake - 20150405
# Find Assimp library
#  ASSIMP_FOUND, If false, do not try to use Assimp.
#  ASSIMP_INCLUDE_DIR, where to find assimp/scene.h
#  ASSIMP_LIBRARIES, libraries to link against to use Assimp.
find_path(ASSIMP_INCLUDE_DIR assimp/scene.h)

set(ASSIMP_NAMES ${ASSIMP_NAMES} Assimp)
if (MSVC)
    if (BUILD_SHARED_LIB)
        if( CMAKE_CL_64 )
            set(LIB_NAME Assimp64)
        else ()
            set(LIB_NAME Assimp32)
        endif ()
    else ()
        if( CMAKE_CL_64 )
            set(LIB_NAME Assimp_static64)
        else ()
            set(LIB_NAME Assimp_static32)
        endif ()
    endif ()
    message(STATUS "*** Finding assimp libraries ${LIB_NAME} and ${LIB_NAME}d...") 
    find_library(ASSIMP_LIB_DBG NAMES ${LIB_NAME}d )
    find_library(ASSIMP_LIB_REL NAMES ${LIB_NAME} )
    if (ASSIMP_LIB_DBG AND ASSIMP_LIB_REL)
        set(ASSIMP_LIBRARY
            debug ${ASSIMP_LIB_DBG}
            optimized ${ASSIMP_LIB_REL}
            )
    elseif (ASSIMP_LIB_REL)
        set(ASSIMP_LIBRARY ${ASSIMP_LIB_REL})
    else ()
        foreach(LIB_NAME ${ASSIMP_NAMES})
            message(STATUS "*** Finding assimp libraries ${LIB_NAME} and ${LIB_NAME}d...") 
            find_library(ASSIMP_LIB_DBG NAMES ${LIB_NAME}d )
            find_library(ASSIMP_LIB_REL NAMES ${LIB_NAME} )
            if (ASSIMP_LIB_DBG AND ASSIMP_LIB_REL)
            set(ASSIMP_LIBRARY
                debug ${ASSIMP_LIB_DBG}
                optimized ${ASSIMP_LIB_REL}
                )
            elseif (ASSIMP_LIB_REL)
                set(ASSIMP_LIBRARY ${ASSIMP_LIB_REL})
            endif()
        endforeach ()
    endif ()
else ()
    find_library(ASSIMP_LIBRARY NAMES ${ASSIMP_NAMES} )
endif ()

# handle the QUIETLY and REQUIRED arguments and set ASSIMP_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ASSIMP
                                  REQUIRED_VARS ASSIMP_LIBRARY ASSIMP_INCLUDE_DIR
                                  )

if(ASSIMP_FOUND)
  set( ASSIMP_LIBRARIES ${ASSIMP_LIBRARY} )
endif()

mark_as_advanced(ASSIMP_INCLUDE_DIR ASSIMP_LIBRARIES)

# eof

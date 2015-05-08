# FindEasyBMP.cmake - 20150508
# Find EasyBMP library
#
#  EASYBMP_FOUND, If false, do not try to use EasyBMP.
#  EASYBMP_INCLUDE_DIR, where to find EasyBMP.h, etc.
#  EASYBMP_LIBRARIES, libraries to link against to use EasyBMP.
#
#  EASYBMP_DEBUG - To ADD DEBUG output
set(EASYBMP_DEBUG 1)
find_path(EASYBMP_INCLUDE_DIR EasyBMP.h)
if (EASYBMP_DEBUG)
    if (EASYBMP_INCLUDE_DIR)
        message(STATUS "=== Found EASYBMP_INCLUDE_DIR=${EASYBMP_INCLUDE_DIR}" )
    else ()
        message(STATUS "=== NOT found EASYBMP_INCLUDE_DIR" )
    endif ()
endif ()
set(EASYBMP_NAMES ${EASYBMP_NAMES} EasyBMP libEasyBMP)
if (MSVC)
    if (EASYBMP_DEBUG)
        message(STATUS "=== Finding EasyBMP MSVC libraries..." )
    endif ()
    foreach (e_lib ${EASYBMP_NAMES})
        if (EASYBMP_DEBUG)
            message(STATUS "=== Searching for ${e_lib}..." )
        endif ()
        find_library(EASYBMP_LIB_DBG NAMES ${e_lib}d )
        find_library(EASYBMP_LIB_REL NAMES ${e_lib}  )
        if (EASYBMP_LIB_DBG AND EASYBMP_LIB_REL)
            if (EASYBMP_DEBUG)
                message(STATUS "=== Found BOTH DBG=${EASYBMP_LIB_DBG} REL=${EASYBMP_LIB_REL}")
            endif ()
            set(EASYBMP_LIBRARY
                debug ${EASYBMP_LIB_DBG}
                optimized ${EASYBMP_LIB_REL}
                )
            break()    # is this reuired/desired???
        elseif (EASYBMP_LIB_REL)
            if (EASYBMP_DEBUG)
                message(STATUS "=== Found ONLY REL=${EASYBMP_LIB_REL}")
            endif ()    
            set(EASYBMP_LIBRARY ${EASYBMP_LIB_REL})
        else ()
            if (EASYBMP_DEBUG)
                message(STATUS "=== Found NEITHER DBG=${e_lib}d, nor REL=${e_lib}")
            endif ()    
        endif ()
    endforeach ()
else ()
    find_library(EASYBMP_LIBRARY NAMES ${EASYBMP_NAMES} )
endif ()

# handle the QUIETLY and REQUIRED arguments and set EASYBMP_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(EASYBMP
                                  REQUIRED_VARS EASYBMP_LIBRARY EASYBMP_INCLUDE_DIR
                                  )

if(EASYBMP_FOUND)
  set( EASYBMP_LIBRARIES ${EASYBMP_LIBRARY} )
endif()

mark_as_advanced(EASYBMP_INCLUDE_DIR EASYBMP_LIBRARIES)

# eof

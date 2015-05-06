# GLEW Library - 20150406
# Try to find GLEW library and include path.
# Once done this will define
#
# GLEW_FOUND
# GLEW_INCLUDE_PATH
# GLEW_LIBRARY
# GLEW_LIBRARY_PATH     - path to GLEW library
# GLEW_DEFINITIONS - Any compiler definitions to add

IF (WIN32)
 FIND_PATH( GLEW_INCLUDE_PATH GL/glew.h
       $ENV{PROGRAMFILES}/GLEW/include
       ${GLEW_ROOT_DIR}/include
     DOC "The directory where GL/glew.h resides")
  if (GLEW_INCLUDE_PATH)
    message(STATUS "== WIN - Found GL/glew.h in ${GLEW_INCLUDE_PATH}")
  else ()
    message(STATUS "== WIN - GL/glew.h NOT FOUND!")
  endif ()
  set(FIND_GLEW64)
  if ((CMAKE_SIZEOF_VOID_P EQUAL 8) OR CMAKE_CL_64)
    set(FIND_GLEW64 1)
    message(STATUS "== CMAKE_SIZEOF_VOID_P=${CMAKE_SIZEOF_VOID_P} - 64-bits")
  else ()    
    message(STATUS "== CMAKE_SIZEOF_VOID_P=${CMAKE_SIZEOF_VOID_P} - 32-bits")
  endif ()
  if ((NV_SYSTEM_PROCESSOR STREQUAL "AMD64") OR (NV_SYSTEM_PROCESSOR STREQUAL "x86_64"))
    set(FIND_GLEW64 1)
  endif ()
  
 if (FIND_GLEW64)
    if (MSVC)
        message(STATUS "== Finding 64-bit MSVC WIN static libraries")
        find_library( GLEW_LIBRARY_DBG
            NAMES glew64sd
            PATHS $ENV{PROGRAMFILES}/GLEW/lib
                ${PROJECT_SOURCE_DIR}/src/nvgl/glew/bin
                ${PROJECT_SOURCE_DIR}/src/nvgl/glew/lib
            DOC "The GLEW DBG library (64-bit)")
       find_library( GLEW_LIBRARY_REL
            NAMES glew64s
            PATHS $ENV{PROGRAMFILES}/GLEW/lib
                ${PROJECT_SOURCE_DIR}/src/nvgl/glew/bin
                ${PROJECT_SOURCE_DIR}/src/nvgl/glew/lib
            DOC "The GLEW REL library (64-bit)")
        if (GLEW_LIBRARY_DBG AND GLEW_LIBRARY_REL)
            set(GLEW_LIBRARY
                optimized ${GLEW_LIBRARY_REL}
                debug ${GLEW_LIBRARY_DBG} )
            set(GLEW_DEFINITIONS -DGLEW_STATIC)
            message(STATUS "*** Found glew static. Add GLEW_DEFINITIONS=${GLEW_DEFINITIONS} to compile")
        elseif (GLEW_LIBRARY_REL)
            set(GLEW_LIBRARY ${GLEW_LIBRARY_REL})
            set(GLEW_DEFINITIONS -DGLEW_STATIC)
            message(STATUS "*** Found glew static. Add GLEW_DEFINITIONS=${GLEW_DEFINITIONS} to compile")
        else ()
            message(STATUS "== Finding 64-bit MSVC WIN DLL libraries")
            find_library( GLEW_LIBRARY_DBG
                NAMES glew64d
                PATHS $ENV{PROGRAMFILES}/GLEW/lib
                    ${PROJECT_SOURCE_DIR}/src/nvgl/glew/bin
                    ${PROJECT_SOURCE_DIR}/src/nvgl/glew/lib
                DOC "The GLEW DBG library (64-bit)")
           find_library( GLEW_LIBRARY_REL
                NAMES glew64
                PATHS $ENV{PROGRAMFILES}/GLEW/lib
                    ${PROJECT_SOURCE_DIR}/src/nvgl/glew/bin
                    ${PROJECT_SOURCE_DIR}/src/nvgl/glew/lib
                DOC "The GLEW REL library (64-bit)")
            if (GLEW_LIBRARY_DBG AND GLEW_LIBRARY_REL)
                set(GLEW_LIBRARY
                    optimized ${GLEW_LIBRARY_REL}
                    debug ${GLEW_LIBRARY_DBG} )
            elseif (GLEW_LIBRARY_REL)
                set(GLEW_LIBRARY ${GLEW_LIBRARY_REL})
            endif ()
        endif ()
    else ()
        message(STATUS "== Finding 64-bit WIN libraries")
        FIND_LIBRARY( GLEW_LIBRARY
            NAMES glew64s glew64
            PATHS $ENV{PROGRAMFILES}/GLEW/lib
                ${PROJECT_SOURCE_DIR}/src/nvgl/glew/bin
                ${PROJECT_SOURCE_DIR}/src/nvgl/glew/lib
                DOC "The GLEW library (64-bit)")
    endif ()     
 else ()
    if (MSVC)
        message(STATUS "== Finding 32-bit MSVC libraries")
        find_library( GLEW_LIBRARY_DBG
            NAMES glew32sd glew323d
            PATHS $ENV{PROGRAMFILES}/GLEW/lib
                ${PROJECT_SOURCE_DIR}/src/nvgl/glew/bin
                ${PROJECT_SOURCE_DIR}/src/nvgl/glew/lib
            DOC "The GLEW DBG library (32-bit)")
       find_library( GLEW_LIBRARY_REL
            NAMES glew32s glew32
            PATHS $ENV{PROGRAMFILES}/GLEW/lib
                ${PROJECT_SOURCE_DIR}/src/nvgl/glew/bin
                ${PROJECT_SOURCE_DIR}/src/nvgl/glew/lib
            DOC "The GLEW REL library (32-bit)")
        if (GLEW_LIBRARY_DBG AND GLEW_LIBRARY_REL)
            set(GLEW_LIBRARY
                optimized ${GLEW_LIBRARY_REL}
                debug ${GLEW_LIBRARY_DBG} )
        else ()
            if (GLEW_LIBRARY_REL)
                set(GLEW_LIBRARY ${GLEW_LIBRARY_REL})
            endif ()
        endif ()
        if (GLEW_LIBRARY)
            set(GLEW_DEFINITIONS -DGLEW_STATIC)
            message(STATUS "*** Found glew static. Add GLEW_DEFINITIONS=${GLEW_DEFINITIONS} to compile")
        endif ()
    else ()
        message(STATUS "== Finding 32-bit WIN libraries")
   FIND_LIBRARY( GLEW_LIBRARY
     NAMES glew GLEW glew32 glew32s
     PATHS $ENV{PROGRAMFILES}/GLEW/lib
       ${PROJECT_SOURCE_DIR}/src/nvgl/glew/bin
       ${PROJECT_SOURCE_DIR}/src/nvgl/glew/lib
     DOC "The GLEW library")
    endif ()
 endif ()
ELSE ()
 FIND_PATH( GLEW_INCLUDE_PATH GL/glew.h
   /usr/include
   /usr/local/include
   /sw/include
   /opt/local/include
   ${GLEW_ROOT_DIR}/include
   DOC "The directory where GL/glew.h resides")
 FIND_LIBRARY( GLEW_LIBRARY
   NAMES GLEW glew
   PATHS
    /usr/lib64
    /usr/lib
    /usr/local/lib64
    /usr/local/lib
    /sw/lib
    /opt/local/lib
    ${GLEW_ROOT_DIR}/lib
   DOC "The GLEW library")
ENDIF ()

SET(GLEW_FOUND "NO")
IF (GLEW_INCLUDE_PATH AND GLEW_LIBRARY)
 SET(GLEW_LIBRARIES ${GLEW_LIBRARY})
 GET_FILENAME_COMPONENT(GLEW_LIBRARY_PATH "${GLEW_LIBRARY}" PATH)
 SET(GLEW_FOUND "YES")
ENDIF ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLEW DEFAULT_MSG GLEW_LIBRARY GLEW_INCLUDE_PATH)

# eof

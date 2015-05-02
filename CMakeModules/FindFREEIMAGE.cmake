# FindFREEIMAGE.cmake - 20150405
# Find FreeImage library
#  FREEIMAGE_INCLUDE_DIR, where to find tiff.h, etc.
#  FREEIMAGE_LIBRARIES, libraries to link against to use TIFF.
#  FREEIMAGE_FOUND, If false, do not try to use TIFF.
find_path(FREEIMAGE_INCLUDE_DIR FreeImage.h)

set(FREEIMAGE_NAMES ${FREEIMAGE_NAMES_NAMES} FreeImage libFeeImage)
if (MSVC)
    find_library(FREEIMAGE_LIB_DBG NAMES FreeImaged )
    find_library(FREEIMAGE_LIB_REL NAMES FreeImage )
    if (FREEIMAGE_LIB_DBG AND FREEIMAGE_LIB_REL)
        set(FREEIMAGE_LIBRARY
            debug ${FREEIMAGE_LIB_DBG}
            optimized ${FREEIMAGE_LIB_REL}
            )
    elseif (FREEIMAGE_LIB_REL)
        set(FREEIMAGE_LIBRARY ${FREEIMAGE_LIB_REL})
    endif ()
else ()
    find_library(FREEIMAGE_LIBRARY NAMES ${FREEIMAGE_NAMES} )
endif ()

# handle the QUIETLY and REQUIRED arguments and set FREEIMAGE_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FREEIMAGE
                                  REQUIRED_VARS FREEIMAGE_LIBRARY FREEIMAGE_INCLUDE_DIR
                                  )

if(FREEIMAGE_FOUND)
  set( FREEIMAGE_LIBRARIES ${FREEIMAGE_LIBRARY} )
endif()

mark_as_advanced(FREEIMAGE_INCLUDE_DIR FREEIMAGE_LIBRARIES)

# eof

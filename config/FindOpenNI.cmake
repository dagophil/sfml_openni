#
# $Id$
#
# (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# sets the following variables:
# - OPENNI_FOUND
# - OPENNI_INCLUDE_DIRS
# - OPENNI_LIBRARIES


if (WIN32)

  # Find the file XnOpenNI.h
  # For this to work properly you must have the environment variable OPEN_NI_INCLUDE set
  # to the OpenNI include directory (set by OpenNI self-extraction)
  find_path (OPENNI_INCLUDE_DIR XnOpenNI.h
             PATHS ENV OPEN_NI_INCLUDE PATH PATH_SUFFIXES Include)

  # Find the base openNI library
  find_library (OPENNI_LIBRARY openNI PATHS ENV OPEN_NI_LIB)

else (WIN32)

  if (APPLE)
    find_path (OPENNI_INCLUDE_DIR XnOpenNI.h PATH_SUFFIXES ni)
    find_library (OPENNI_LIBRARY OpenNI)

  else (APPLE)

    # Find the file XnOpenNI.h
    # This searches in the standard include paths and check for "ni" directory
    # this works for Fedora 14 package
    # this works for the Ubuntu Lucid package
    find_path (OPENNI_INCLUDE_DIR XnOpenNI.h PATH_SUFFIXES ni openni)
    find_library (OPENNI_LIBRARY           OpenNI)
    find_library (XNCORE_LIBRARY           XnCore)
    find_library (XNFORMATS_LIBRARY        XnFormats)
    find_library (XNDEVICESENSORV2_LIBRARY XnDeviceSensorV2KM)
    #find_library (XNDEVICESENSORV2_LIBRARY XnDeviceSensorV2)
    find_library (XNDDK_LIBRARY            XnDDK)


  endif (APPLE)

endif (WIN32)

# Set found value
set (OPENNI_FOUND FALSE)
if (OPENNI_INCLUDE_DIR)
  if (OPENNI_LIBRARY)
    set (OPENNI_FOUND TRUE)
  endif (OPENNI_LIBRARY)
endif (OPENNI_INCLUDE_DIR)

# Set include dirs and libraries
set (OPENNI_INCLUDE_DIRS ${OPENNI_INCLUDE_DIR})

set (OPENNI_LIBRARIES
     ${OPENNI_LIBRARY}
     ${XNCORE_LIBRARY}
     ${XNFORMATS_LIBRARY}
     ${XNDEVICESENSORV2_LIBRARY}
     ${XNDDK_LIBRARY}
     )

# Hide temporary variables
mark_as_advanced (
  OPENNI_INCLUDE_DIRS
  OPENNI_INCLUDE_DIR
  OPENNI_LIBRARIES
  OPENNI_LIBRARY
  XNCORE_LIBRARY
  XNFORMATS_LIBRARY
  XNDEVICESENSORV2_LIBRARY
  XNDDK_LIBRARY
  )

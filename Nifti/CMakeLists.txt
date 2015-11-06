
project (Nifti)

SET(HEADERS
NiftiHeader.h
NiftiIO.h
)

SET(SOURCES
NiftiHeader.cxx
NiftiIO.cxx
)
#
# Create the NIFTI library
#
ADD_LIBRARY(Nifti
${HEADERS}
${SOURCES}
)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Common
)

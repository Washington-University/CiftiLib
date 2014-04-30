#
# The NIFTI Project
#
project (Nifti)

#
# Need XML from Qt
#
SET(QT_DONT_USE_QTGUI)

#
# Add QT for includes
#
INCLUDE (${QT_USE_FILE})


#
# Create the NIFTI library
#
ADD_LIBRARY(Nifti
NiftiHeader.h
NiftiIO.h

NiftiHeader.cxx
NiftiIO.cxx
)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Common
)

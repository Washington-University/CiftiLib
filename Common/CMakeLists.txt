#
# Name of Project
#
PROJECT(Common)

#
# Need XML from Qt
#
SET(QT_DONT_USE_QTGUI)

#
# Add QT for includes
#
INCLUDE(${QT_USE_FILE})

#
# Create Common Library
#
ADD_LIBRARY(Common
ByteSwapping.h
CaretAssert.h
BinaryFile.h
BinaryFile.cxx
CiftiException.h
CiftiException.cxx
Compact3DLookup.h
CompactLookup.h
FloatMatrix.h
FloatMatrix.cxx
MatrixFunctions.h
MathFunctions.h
MathFunctions.cxx
MultiDimArray.h
MultiDimIterator.h
Vector3D.h
Vector3D.cxx
VoxelIJK.h)

INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
)

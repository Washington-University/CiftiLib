
PROJECT(Common)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

#
# Create Common Library
#
ADD_LIBRARY(Common
AString.h
AString.cxx
ByteSwapping.h
CiftiAssert.h
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
VoxelIJK.h
XmlAdapter.h
XmlAdapter.cxx)

INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
)

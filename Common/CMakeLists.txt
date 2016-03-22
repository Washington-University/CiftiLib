
PROJECT(Common)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

SET(HEADERS
AString.h
ByteSwapping.h
CiftiAssert.h
BinaryFile.h
CiftiException.h
Compact3DLookup.h
CompactLookup.h
FloatMatrix.h
MatrixFunctions.h
MathFunctions.h
MultiDimArray.h
MultiDimIterator.h
Vector3D.h
VoxelIJK.h
XmlAdapter.h
)

SET(PUBLIC_HEADERS
AString.h #API
ByteSwapping.h #NiftiIO.h
CiftiAssert.h #NiftiIO.h
BinaryFile.h  #NiftiIO.h
CiftiException.h #API
Compact3DLookup.h #CiftiBrainModelsMap.h
CompactLookup.h #Compact3DLookup.h
#FloatMatrix.h
#MatrixFunctions.h
#MathFunctions.h
#MultiDimArray.h
MultiDimIterator.h #API
Vector3D.h #API
VoxelIJK.h #API
XmlAdapter.h #could forward declare everywhere
)

SET(SOURCES
AString.cxx
BinaryFile.cxx
CiftiException.cxx
FloatMatrix.cxx
MathFunctions.cxx
Vector3D.cxx
XmlAdapter.cxx
)

#
# Create Common Library
#
ADD_LIBRARY(Common
${HEADERS}
${SOURCES}
)

INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
)

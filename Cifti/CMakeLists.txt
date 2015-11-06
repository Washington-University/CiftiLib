
PROJECT(Cifti)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

SET(HEADERS
CiftiFile.h
CiftiXML.h
CiftiVersion.h

CiftiMappingType.h
CiftiBrainModelsMap.h
CiftiLabelsMap.h
CiftiParcelsMap.h
CiftiScalarsMap.h
CiftiSeriesMap.h

Label.h
LabelTable.h
MetaData.h

StructureEnum.h
VolumeSpace.h
)

SET(SOURCES
CiftiFile.cxx
CiftiXML.cxx
CiftiVersion.cxx

CiftiMappingType.cxx
CiftiBrainModelsMap.cxx
CiftiLabelsMap.cxx
CiftiParcelsMap.cxx
CiftiScalarsMap.cxx
CiftiSeriesMap.cxx

Label.cxx
LabelTable.cxx
MetaData.cxx

StructureEnum.cxx
VolumeSpace.cxx
)

#
# Create CIFTI Library
#
ADD_LIBRARY(Cifti
${HEADERS}
${SOURCES}
)

INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Common
)

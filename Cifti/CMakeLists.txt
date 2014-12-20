
PROJECT(Cifti)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

#
# Create CIFTI Library
#
ADD_LIBRARY(Cifti
CiftiFile.h
CiftiFile.cxx
CiftiXML.h
CiftiXML.cxx
CiftiVersion.h
CiftiVersion.cxx

CiftiMappingType.h
CiftiMappingType.cxx
CiftiBrainModelsMap.h
CiftiBrainModelsMap.cxx
CiftiLabelsMap.h
CiftiLabelsMap.cxx
CiftiParcelsMap.h
CiftiParcelsMap.cxx
CiftiScalarsMap.h
CiftiScalarsMap.cxx
CiftiSeriesMap.h
CiftiSeriesMap.cxx

Label.h
Label.cxx
LabelTable.h
LabelTable.cxx
MetaData.h
MetaData.cxx

StructureEnum.h
StructureEnum.cxx
VolumeSpace.h
VolumeSpace.cxx
)

INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Common
)

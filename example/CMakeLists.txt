#
# Name of Project
#
PROJECT(example)

#
# Need XML from Qt
#
SET(QT_DONT_USE_QTGUI)

#
# Add QT for includes
#
INCLUDE(${QT_USE_FILE})

ADD_EXECUTABLE(rewrite
rewrite.cxx)

TARGET_LINK_LIBRARIES(rewrite
Cifti
Nifti
Common
${QT_LIBRARIES}
${LIBS})

INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/example
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Cifti
)

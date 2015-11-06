CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(CiftiLib_and_example)

SET(CMAKE_CXX_FLAGS "-W -Wall")

SET(IGNORE_QT FALSE CACHE BOOL "don't try to use QT")

#
# Use QT for xml/string
#
IF (NOT IGNORE_QT)
    FIND_PACKAGE(Qt4 4.8)
    IF (QT_FOUND)
        SET(QT_DONT_USE_QTGUI TRUE)
        SET(CIFTILIB_USE_QT TRUE)
        ADD_DEFINITIONS(-DCIFTILIB_USE_QT)
        INCLUDE(${QT_USE_FILE})
        SET(LIBS ${LIBS} ${QT_LIBRARIES})
    ENDIF (QT_FOUND)
ENDIF (NOT IGNORE_QT)

IF (NOT QT_FOUND)
    SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
    FIND_PACKAGE(libxml++ REQUIRED)
    INCLUDE_DIRECTORIES(${libxml++_INCLUDE_DIRS})
    SET(LIBS ${LIBS} ${libxml++_LIBRARIES})
    ADD_DEFINITIONS(-DCIFTILIB_USE_XMLPP)
ENDIF (NOT QT_FOUND)

IF (NOT QT_FOUND)
    FIND_PACKAGE(Boost REQUIRED COMPONENTS filesystem system)
ELSE (NOT QT_FOUND)
    FIND_PACKAGE(Boost REQUIRED)
ENDIF (NOT QT_FOUND)
SET(LIBS ${LIBS} ${Boost_LIBRARIES})
IF (Boost_VERSION LESS 104400)
    #absolute() was added in 1.44.0, with filesystem v3
    ADD_DEFINITIONS(-DCIFTILIB_BOOST_NO_FSV3)
ENDIF (Boost_VERSION LESS 104400)
IF (Boost_VERSION LESS 104800)
    #canonical() was added in 1.48.0
    ADD_DEFINITIONS(-DCIFTILIB_BOOST_NO_CANONICAL)
ENDIF (Boost_VERSION LESS 104800)
IF (Boost_VERSION LESS 105600)
    #try_lexical_cast was added in 1.56.0
    ADD_DEFINITIONS(-DCIFTILIB_BOOST_NO_TRY_LEXICAL)
ENDIF (Boost_VERSION LESS 105600)

FIND_PACKAGE(ZLIB)
IF (ZLIB_FOUND)
    ADD_DEFINITIONS("-DCARET_HAVE_ZLIB")
ENDIF (ZLIB_FOUND)

#OS X has some weirdness in its zlib, so let the preprocessor know
IF (APPLE)
    ADD_DEFINITIONS(-DCARET_OS_MACOSX)
ENDIF (APPLE)

ADD_SUBDIRECTORY(Common)
ADD_SUBDIRECTORY(Nifti)
ADD_SUBDIRECTORY(Cifti)
ADD_SUBDIRECTORY(example)

#shared library, as a single file
#helper macro to deal with adding subdirectories to each file
macro(append_subdir_files variable dirname)
get_directory_property(holder DIRECTORY ${dirname} DEFINITION ${variable})
foreach(depfile ${holder})
  list(APPEND ${variable} "${dirname}/${depfile}")
endforeach()
endmacro()

append_subdir_files(SOURCES Common)
append_subdir_files(HEADERS Common)
append_subdir_files(SOURCES Nifti)
append_subdir_files(HEADERS Nifti)
append_subdir_files(SOURCES Cifti)
append_subdir_files(HEADERS Cifti)

ADD_LIBRARY(Cifti-shared SHARED
${SOURCES}
${HEADERS}
)

#NOTE: soversion/version may need to change major version even when library version doesn't, as ABI stability was not designed for
#soversion defines what symlinks are created, version defines what to put on the end of the actual library file
SET_TARGET_PROPERTIES(Cifti-shared
PROPERTIES
OUTPUT_NAME Cifti
SOVERSION 1
VERSION 1.3
)

INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Common
)
#TODO: lib64 handling?
INSTALL(TARGETS Cifti-shared DESTINATION lib)
INSTALL(FILES ${HEADERS} DESTINATION include/Cifti)

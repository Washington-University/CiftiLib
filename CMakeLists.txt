CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(CiftiLib)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall")

SET(IGNORE_QT FALSE CACHE BOOL "don't try to use QT")

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/" "${CMAKE_SOURCE_DIR}/cmake/Modules/UseDoxygen/")

include(UseDoxygen)

#QT
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

#alternative to QT xml, string
IF (NOT QT_FOUND)
    FIND_PACKAGE(libxml++ REQUIRED)
    INCLUDE_DIRECTORIES(${libxml++_INCLUDE_DIRS})
    SET(LIBS ${LIBS} ${libxml++_LIBRARIES})
    ADD_DEFINITIONS(-DCIFTILIB_USE_XMLPP)
ENDIF (NOT QT_FOUND)

#boost, including filesystem if not using QT
IF (NOT QT_FOUND)
    FIND_PACKAGE(Boost REQUIRED COMPONENTS filesystem system)
ELSE (NOT QT_FOUND)
    FIND_PACKAGE(Boost REQUIRED)
ENDIF (NOT QT_FOUND)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
SET(LIBS ${LIBS} ${Boost_LIBRARIES})
#boost quirks
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

#zlib
FIND_PACKAGE(ZLIB)
IF (ZLIB_FOUND)
    INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIRS})
    SET(LIBS ${LIBS} ${ZLIB_LIBRARIES})
    ADD_DEFINITIONS("-DCIFTILIB_HAVE_ZLIB")
ENDIF (ZLIB_FOUND)
#OS X has some weirdness in its zlib, so let the preprocessor know
IF (APPLE)
    ADD_DEFINITIONS(-DCIFTILIB_OS_MACOSX)
ENDIF (APPLE)

#these subdirectories only define the sources contained in them, don't define any build targets
#but we must do this before trying to access variables from these directories
ADD_SUBDIRECTORY(Common)
ADD_SUBDIRECTORY(Nifti)
ADD_SUBDIRECTORY(Cifti)

#example directory does define build targets
ADD_SUBDIRECTORY(example)

macro(append_subdir_files variable dirname)
    get_directory_property(holder DIRECTORY ${dirname} DEFINITION ${variable})
    foreach(depfile ${holder})
        list(APPEND ${variable} "${dirname}/${depfile}")
    endforeach()
endmacro()

#all headers of these are public
append_subdir_files(SOURCES Nifti)
append_subdir_files(HEADERS Nifti)
append_subdir_files(SOURCES Cifti)
append_subdir_files(HEADERS Cifti)
SET(PUBLIC_HEADERS ${HEADERS})

#some headers of this are private
append_subdir_files(SOURCES Common)
append_subdir_files(HEADERS Common)
append_subdir_files(PUBLIC_HEADERS Common)

ADD_LIBRARY(Cifti
${SOURCES}
${HEADERS}
)

#NOTE: soversion set to 0 because ABI compatibility was not designed into the interface
#soversion defines what symlinks are created, version defines what to put on the end of the actual library file
SET_TARGET_PROPERTIES(Cifti
PROPERTIES
OUTPUT_NAME Cifti
SOVERSION 0
VERSION 1.3
)

INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Common
)

INSTALL(TARGETS Cifti DESTINATION lib${LIB_SUFFIX})
INSTALL(FILES ${PUBLIC_HEADERS} DESTINATION include/Cifti)

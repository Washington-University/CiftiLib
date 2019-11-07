CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

#UseDoxygen trips over a policy change in cmake 3, suppress the warning
CMAKE_POLICY(VERSION 2.8.7)
#the suggested version-type policy command doesn't shut this warning up, so set it manually
IF (POLICY CMP0045)
    CMAKE_POLICY(SET CMP0045 OLD)
ENDIF (POLICY CMP0045)

PROJECT(CiftiLib)

SET(CIFTILIB_VERSION 1.5)

#MSVC seems like the only compiler that chokes on -W -Wall
IF (NOT MSVC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall")
ENDIF (NOT MSVC)

SET(IGNORE_QT FALSE CACHE BOOL "don't try to use QT")

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/" "${CMAKE_SOURCE_DIR}/cmake/Modules/UseDoxygen/")

INCLUDE(UseDoxygen)

#TSC: glibmm requires c++11, so it needs to be in the compile flags - in older cmake, this needs to be done manually
IF (${CMAKE_VERSION} VERSION_LESS "3.1")
    IF (CMAKE_COMPILER_IS_GNUCC)
        include(CheckCXXCompilerFlag)
        CHECK_CXX_COMPILER_FLAG("-std=c++11" GCC_STD11)
        IF (${GCC_STD11})
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        ELSE (${GCC_STD11})
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
        ENDIF (${GCC_STD11})
    ENDIF (CMAKE_COMPILER_IS_GNUCC)
ELSE (${CMAKE_VERSION} VERSION_LESS "3.1")
    SET(CMAKE_CXX_STANDARD 11)
    SET(CMAKE_CXX_STANDARD_REQUIRED TRUE)
ENDIF (${CMAKE_VERSION} VERSION_LESS "3.1")

#QT
IF (NOT IGNORE_QT)
    FIND_PACKAGE(Qt4 4.8.0 QUIET)
    IF (QT_FOUND)
        SET(QT_DONT_USE_QTGUI TRUE)
        ADD_DEFINITIONS(-DCIFTILIB_USE_QT)
        INCLUDE(${QT_USE_FILE})
        SET(LIBS ${LIBS} ${QT_LIBRARIES})
        #for pkg-config file
        SET(CIFTILIB_PKGCONFIG_REQUIRES_LINE "Requires: QtCore >= 4.8.0 QtXml")
        SET(CIFTILIB_PKGCONFIG_DEFINE "-DCIFTILIB_USE_QT")
    ELSE (QT_FOUND)
        FIND_PACKAGE(Qt5Core)
        IF (Qt5Core_FOUND)
            SET(QT_FOUND TRUE)
            INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS})
            SET(LIBS ${LIBS} Qt5::Core)
            #whatever that means
            ADD_DEFINITIONS(-DCIFTILIB_USE_QT)
            SET(CIFTILIB_PKGCONFIG_REQUIRES_LINE "Requires: Qt5Core")
            SET(CIFTILIB_PKGCONFIG_DEFINE "-DCIFTILIB_USE_QT")
        ENDIF (Qt5Core_FOUND)
    ENDIF (QT_FOUND)
ENDIF (NOT IGNORE_QT)

#alternative to QT xml, string
IF (NOT QT_FOUND)
    FIND_PACKAGE(libxml++ 2.17.0 REQUIRED)
    INCLUDE_DIRECTORIES(${libxml++_INCLUDE_DIRS})
    SET(LIBS ${LIBS} ${libxml++_LIBRARIES})
    ADD_DEFINITIONS(-DCIFTILIB_USE_XMLPP)
    #for pkg-config file
    SET(CIFTILIB_PKGCONFIG_REQUIRES_LINE "Requires: libxml++-2.6 >= 2.17.0")
    SET(CIFTILIB_PKGCONFIG_DEFINE "-DCIFTILIB_USE_XMLPP")
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

#zlib, useful for volume reading
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

#openmp provides a fast mutex implementation, faster than QT (and probably faster than glibmm)
FIND_PACKAGE(OpenMP)
IF (OPENMP_FOUND)
    SET(CMAKE_CXX_FLAGS "${OpenMP_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
ENDIF (OPENMP_FOUND)

ENABLE_TESTING()

#the library source, doesn't contain build targets
ADD_SUBDIRECTORY(src)

#example directory, has build targets and tests
ADD_SUBDIRECTORY(example)

macro(append_subdir_files variable dirname)
    get_directory_property(holder DIRECTORY ${dirname} DEFINITION ${variable})
    foreach(depfile ${holder})
        list(APPEND ${variable} "${dirname}/${depfile}")
    endforeach()
endmacro()

#get the sources and install info
append_subdir_files(SOURCES src)
append_subdir_files(HEADERS src)
append_subdir_files(PUBLIC_HEADERS src)
append_subdir_files(PRIVATE_DIRS src)

ADD_LIBRARY(Cifti
${SOURCES}
${HEADERS}
)

#one way to get qt5's new compiler flag restrictions into the build - does it have other consequences?
TARGET_LINK_LIBRARIES(Cifti ${LIBS})

#NOTE: soversion set to 0 because ABI compatibility was not designed into the interface
#soversion defines what symlinks are created, version defines what to put on the end of the actual library file
SET_TARGET_PROPERTIES(Cifti
PROPERTIES
OUTPUT_NAME Cifti
SOVERSION 0
VERSION ${CIFTILIB_VERSION}
)

INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/src
)

#install dirs
INCLUDE(GNUInstallDirs)

#pkg-config
CONFIGURE_FILE(CiftiLib.pc.in CiftiLib.pc @ONLY)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/CiftiLib.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

#install
INSTALL(TARGETS Cifti DESTINATION ${CMAKE_INSTALL_LIBDIR})
INSTALL(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CiftiLib)
INSTALL(DIRECTORY ${PRIVATE_DIRS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CiftiLib FILES_MATCHING PATTERN "*.h")

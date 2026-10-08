# CiftiLib

## Dependencies

The CiftiLib library requires boost headers and either QT (4.8.x or 5), or libxml++ 2.17.x or newer (and its dependencies: libxml2, glib, sigc++, gtkmm and glibmm) and the boost filesystem library to compile, and optionally uses zlib if you want to use its NIfTI reading capabilities for other purposes.

## Building

To build it, and example executables, in the recommended "out-of-source" method using cmake:

- Start one level up from the source tree, make the 'build' directory, then enter it:
```
mkdir build; cd build
```

- Generate makefiles with cmake (NOTE: you may want to set the cmake variable CMAKE_BUILD_TYPE to Release or Debug before building
the default build behavior may be non-optimized and without debug symbols.)
```
#run cmake to generate makefiles
cmake -D CMAKE_BUILD_TYPE=Release ../CiftiLib
#OR
cmake-gui ../CiftiLib
```

- Build:
```
make
```

- (OPTIONAL) Run tests:
```
make test
```

- Install library to location specified by cmake variable CMAKE_INSTALL_PREFIX
```
make install
```

- (OPTIONAL) Generate documentation (needs doxygen installed and on PATH)
```
make doc
```

The build results will be in subdirectories of the build directory, not in the source directory.

To use the installed library, use pkg-config to get the needed directories and libraries.  For instance, in cmake:

```
find_package(PkgConfig)
pkg_check_modules(PC_CIFTI REQUIRED CiftiLib)
```

You can then use cmake variables PC_CIFTI_LIBRARIES, PC_CIFTI_INCLUDE_DIRS, and similar, see here for descriptions:

https://cmake.org/cmake/help/v3.0/module/FindPkgConfig.html

## Troubleshooting

If you are using manually-installed libraries rather than distribution-packaged libraries, you may need to use the cmake variables CMAKE_LIBRARY_PATH and CMAKE_INCLUDE_PATH.

If you are getting link errors related to boost, try setting a cmake variable Boost_NO_BOOST_CMAKE to true.

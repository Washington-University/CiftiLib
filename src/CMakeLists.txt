PROJECT(src)

SET(HEADERS
CiftiFile.h
NiftiIO.h
CiftiMultiDimIterator.h
)

SET(SOURCES
CiftiFile.cxx
NiftiIO.cxx
)

#only the headers that should end up in the top level of CiftiLib when installed
#do not move this below the append_subdir_files calls
SET(PUBLIC_HEADERS
${HEADERS}
)

#list the subdirectories here, so each build system file only references one additional level
SET(PRIVATE_DIRS
Cifti
Common
Nifti
)

ADD_SUBDIRECTORY(Cifti)
ADD_SUBDIRECTORY(Common)
ADD_SUBDIRECTORY(Nifti)

macro(append_subdir_files variable dirname)
    get_directory_property(holder DIRECTORY ${dirname} DEFINITION ${variable})
    foreach(depfile ${holder})
        list(APPEND ${variable} "${dirname}/${depfile}")
    endforeach()
endmacro()

append_subdir_files(SOURCES Nifti)
append_subdir_files(HEADERS Nifti)
append_subdir_files(SOURCES Cifti)
append_subdir_files(HEADERS Cifti)
append_subdir_files(SOURCES Common)
append_subdir_files(HEADERS Common)


PROJECT(example)

ADD_EXECUTABLE(rewrite
rewrite.cxx)

TARGET_LINK_LIBRARIES(rewrite
Cifti
${LIBS})

ADD_EXECUTABLE(xmlinfo
xmlinfo.cxx)

TARGET_LINK_LIBRARIES(xmlinfo
Cifti
${LIBS})

INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/example
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Cifti
)

SET(cifti_files
Conte69.MyelinAndCorrThickness.32k_fs_LR.dscalar.nii
Conte69.MyelinAndCorrThickness.32k_fs_LR.dtseries.nii
Conte69.MyelinAndCorrThickness.32k_fs_LR.ptseries.nii
Conte69.parcellations_VGD11b.32k_fs_LR.dlabel.nii
ones.dscalar.nii
)

IF(QT_FOUND)
    #QT4
    SET(cifti_le_md5s
    b7cfedcd5f81425ecc6c0ad4541eae31
    c28eb8188e3657081f155203445767d3
    9bc749538f059cc75f4ec58b9895ac34
    d304e04bdee80a7235ac17a0b45b3c0a
    761bc5dfbbb1778f6af3db0b28b022c1
    )
    SET(cifti_be_md5s
    97f76338c99d391c36f689d2f2e1ab82
    04fd9356b71b361360b30d633635df1c
    d8918e890c14effb69cfcee782320bdb
    fae94e6d79f063ae64e862b6c73293ea
    36efab624af137a0d5466f4259628c27
    )
ELSE(QT_FOUND)
    #xml++
    SET(cifti_le_md5s
    d26151f9223b480b314e1b42fda814a4
    d977a70df95b62b55e90ecea84a8e391
    2708ea22c31c3cbf17c288d58e4695ac
    a144a1039e64ddc3b2fb602d8a921962
    fc86edf460b4641d0bc252ee820ce1b5
    )
    SET(cifti_be_md5s
    2cee01f97ae61b7c9875a24c0e09dbf5
    b7b06fb2a977691a3177dbf3cd1b1f6d
    bf34d6aca436d8ce09315a0b4c40a7b8
    d5dbf733f81214ef818576dc9940d3d5
    9266f7a4968400d2f90b2f7f944a771d
    )
ENDIF(QT_FOUND)

#ADD_TEST(timer ${CMAKE_CURRENT_BINARY_DIR}/Tests/test_driver timer)

LIST(LENGTH cifti_files num_cifti_files)
#FOREACH(... RANGE x) is stupid, it does all of [0, x] inclusive, totaling x + 1 iterations
MATH(EXPR loop_end "${num_cifti_files} - 1")

FOREACH(index RANGE ${loop_end})
    LIST(GET cifti_files ${index} testfile)
    #ADD_TEST doesn't seem to have a way to capture stdout, so checking the output for consistency seems to be a problem
    ADD_TEST(info-${testfile} xmlinfo ${CMAKE_SOURCE_DIR}/example/data/${testfile})

    ADD_TEST(rewrite-little-${testfile} rewrite ${CMAKE_SOURCE_DIR}/example/data/${testfile} little-${testfile} LITTLE)
    LIST(GET cifti_le_md5s ${index} goodsum)
    ADD_TEST(rewrite-little-md5-${testfile} ${CMAKE_COMMAND} -Dgood_sum=${goodsum} -Dcheck_file=little-${testfile} -P ${CMAKE_SOURCE_DIR}/cmake/scripts/testmd5.cmake)

    ADD_TEST(rewrite-big-${testfile} rewrite ${CMAKE_SOURCE_DIR}/example/data/${testfile} big-${testfile} BIG)
    LIST(GET cifti_be_md5s ${index} goodsum)
    ADD_TEST(rewrite-big-md5-${testfile} ${CMAKE_COMMAND} -Dgood_sum=${goodsum} -Dcheck_file=big-${testfile} -P ${CMAKE_SOURCE_DIR}/cmake/scripts/testmd5.cmake)
ENDFOREACH(index RANGE ${loop_end})

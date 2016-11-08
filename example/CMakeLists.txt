
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
${CMAKE_SOURCE_DIR}/src
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
    SET(cifti_be_md5s
        3ba20f6ef590735c1211991f8e0144e6
        e3a1639ef4b354752b0abb0613eedb73
        9fd1b6fb7b938155226ca53fdaff5a12
        48903d80589d294b5421cad9a7ba9264
        93bb7f76c8d273817601ea5be236f83d
    )
    SET(cifti_le_md5s
        3e24e7908122905c2a2d357347ecee7c
        4ee31da414dcd26fe37855601ae3e9e2
        e096a1cf0713f7a36590445e9f6564b1
        32345267599b07083092b7dedfd8796c
        512e0359c64d69dde93d605f8797f3a2
    )
ELSE(QT_FOUND)
    #xml++
    SET(cifti_be_md5s
        a2581f76a94b7c4371d6970f1262e1bc
        ca4cfb02fb7f0d8f167f546aab85e91d
        892c26e691fe62c4edf358f7f91942a3
        e761d405c945e724b37f96e356dc19ad
        2e4efa4e36cdb13f514c265e6c9c8c4b
    )
    SET(cifti_le_md5s
        39ecfba4c1dade1a2ce3e95b325bd8b1
        921a42e4181052ef4f211d389a2e9328
        764c816004ebe23d188d98f9f29c7337
        6fabac021e377efd35dede7198feefd4
        fe0cbb768e26aa12a0e03990f4f50a30
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
    SET_TESTS_PROPERTIES(rewrite-little-md5-${testfile} PROPERTIES DEPENDS rewrite-little-${testfile})

    ADD_TEST(rewrite-big-${testfile} rewrite ${CMAKE_SOURCE_DIR}/example/data/${testfile} big-${testfile} BIG)
    LIST(GET cifti_be_md5s ${index} goodsum)
    ADD_TEST(rewrite-big-md5-${testfile} ${CMAKE_COMMAND} -Dgood_sum=${goodsum} -Dcheck_file=big-${testfile} -P ${CMAKE_SOURCE_DIR}/cmake/scripts/testmd5.cmake)
    SET_TESTS_PROPERTIES(rewrite-big-md5-${testfile} PROPERTIES DEPENDS rewrite-big-${testfile})
ENDFOREACH(index RANGE ${loop_end})

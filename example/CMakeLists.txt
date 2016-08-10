
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
        ab40340d86eb5ba8fad31ad3445d944e
        ae51bb3fb97a0cb666399989cd613164
        95918d752f86f27c2b585232082937ac
        d7026fe27d47336629beda31a932cb32
        517001a95841a8cf7ed4cc6b714bdf39
    )
    SET(cifti_le_md5s
        e359977a01c81d35fdf2742d161d1865
        8fa2e4efec70e1cdecf2ecad3097c292
        8d5529875f8ad811416f05d2670dc17b
        4ade8f44e0da86c1de404ea30de2029d
        efa021c4049f4ae2d32264857d4b3864
    )
ELSE(QT_FOUND)
    #xml++
    SET(cifti_be_md5s
        ecc2f8d9bcc7cb9c8d7a4efcf3c1a8d6
        92dd447e15b39521c00439ddb76a4fdb
        abaee50490045fe4205077354be114ca
        b7e23fd47880ec59b9b3431a4ad496d1
        900ed1c7a1c22d45853d99ab52ed5130
    )
    SET(cifti_le_md5s
        81e248f13f4787143e0a2be63739eda4
        25e83e94d6a2bd7622e4ca9c409470a5
        6e5f9d5db255227a18a23c3c5c35e458
        0e06db5f7e7b04ec47493bc0e6eba3fa
        952e56391c9fef2fce5a39cbae10e5af
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

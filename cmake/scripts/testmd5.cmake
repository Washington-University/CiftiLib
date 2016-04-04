FILE(MD5 ${check_file} real_sum)

IF(NOT (${real_sum} STREQUAL ${good_sum}))
    MESSAGE(FATAL_ERROR "expected ${good_sum}, got ${real_sum}")
ENDIF(NOT (${real_sum} STREQUAL ${good_sum}))

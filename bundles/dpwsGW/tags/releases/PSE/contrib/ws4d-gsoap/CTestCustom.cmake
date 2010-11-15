set(CTEST_CUSTOM_MAXIMUM_NUMBER_OF_ERRORS 1000)
set(CTEST_CUSTOM_MAXIMUM_NUMBER_OF_WARNINGS 1000)

SET(CTEST_CUSTOM_WARNING_EXCEPTION
    ${CTEST_CUSTOM_WARNING_EXCEPTION}
    "gen/lcmC.c"
    "stdsoap2.c"
    "gen/evsC.c"
    "gen/atsC.c"
    "gen/sisC.c"
    "gen/dpwsC.c"
    "gen/acs1Server.c"
)

SET(CTEST_CUSTOM_ERROR_EXCEPTION
    ${CTEST_CUSTOM_ERROR_EXCEPTION}
    "gen/lcmC.c"
    "stdsoap2.c"
    "gen/evsC.c"
    "gen/atsC.c"
    "gen/sisC.c"
    "gen/dpwsC.c"
    "gen/acs1Server.c"
)

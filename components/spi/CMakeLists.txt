
idf_component_register(SRCS "src/spi.c" "src/ads1x9x.c"
                       INCLUDE_DIRS "include" "."
                       REQUIRES "${public_requires}" "driver")

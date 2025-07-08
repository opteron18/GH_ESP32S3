
idf_component_register(SRCS "src/spi.c" "src/ads1x9x.c" "src/circular_buffer.c" "src/ads1292r.c"
                       INCLUDE_DIRS "include" "."
                       REQUIRES "${public_requires}" "driver")

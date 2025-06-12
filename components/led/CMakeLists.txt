
idf_component_register(SRCS "src/led_blink.c"
                       INCLUDE_DIRS "include" "."
                       REQUIRES "${public_requires}" "driver" "led_strip")

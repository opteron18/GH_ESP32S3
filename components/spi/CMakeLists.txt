
idf_component_register(SRCS 
                            "src/spi.c" 
                            "src/ads1x9x.c" 
                            "src/circular_buffer.c" 
                            "src/ads1292r.c" 
                            "src/spi_bitbang.c" 
                            "src/lcd_init.c"
                       INCLUDE_DIRS "include" "." "../../main"
                       REQUIRES "${public_requires}" "driver" "main" "esp_timer" "uart") 

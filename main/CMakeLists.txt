idf_component_register(
    SRCS   
        "blink_example_main.c" 
        "gh3300_task.c" 
        "led_task.c" 
        "uart_task.c" 
        "adc_buttom.c" 
        "EMG_task.c" 
        "global_config.c" 
        "UI_task.c"
        "sdio_sdcard.c"
    INCLUDE_DIRS "." "${CMAKE_SOURCE_DIR}/components/gh_algo/include_2" "${CMAKE_SOURCE_DIR}/components/gh_algo/include" "${CMAKE_SOURCE_DIR}/components/led/include"
    REQUIRES
        esp_wifi 
        esp_event 
        esp_http_server 
        nvs_flash 
        json
        esp_netif
        esp_adc 
        spi
        spiffs
        # led
        # gh_algo
        fatfs
        sd_card
        uart
        pwm_pump
        esp_timer
    WHOLE_ARCHIVE

)

set(GH_ALGO_LIBS
    ${CMAKE_CURRENT_SOURCE_DIR}/../components/gh_algo/lib/esp32_xtensa-esp32-gcc-14.2.0_os_common_dl.a
    ${CMAKE_CURRENT_SOURCE_DIR}/../components/gh_algo/lib/esp32_xtensa-esp32-gcc-14.2.0_os_common_dsp.a
    ${CMAKE_CURRENT_SOURCE_DIR}/../components/gh_algo/lib/esp32_xtensa-esp32-gcc-14.2.0_os_hr.a
    ${CMAKE_CURRENT_SOURCE_DIR}/../components/gh_algo/lib/esp32_xtensa-esp32-gcc-14.2.0_os_hrv.a
    ${CMAKE_CURRENT_SOURCE_DIR}/../components/gh_algo/lib/esp32_xtensa-esp32-gcc-14.2.0_os_ecg.a
    ${CMAKE_CURRENT_SOURCE_DIR}/../components/gh_algo/lib/esp32_xtensa-esp32-gcc-14.2.0_os_nadt.a
    ${CMAKE_CURRENT_SOURCE_DIR}/../components/gh_algo/lib/esp32_xtensa-esp32-gcc-14.2.0_os_spo2.a
)
set_target_properties(${COMPONENT_LIB} PROPERTIES
    INTERFACE_LINK_LIBRARIES "${GH_ALGO_LIBS}"
)
target_link_libraries(${COMPONENT_LIB}
    INTERFACE
    "-Wl,--start-group"
    ${GH_ALGO_LIBS}
    "-Wl,--end-group"
)
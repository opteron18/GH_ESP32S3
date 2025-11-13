idf_component_register(
    # SRC_DIRS
    #     "./"
    #     "./lv_port"
    #     "./BLE_SPP_SERVER"
    #     "./blufi"
    #     "../components/lvgl/demos/music/"
    #     "../components/lvgl/demos/music/assets"
    SRCS   
        "main.c"
        "gh3300_task.c" 
        "led_task.c" 
        "uart_task.c" 
        "adc_buttom.c" 
        "EMG_task.c" 
        "global_config.c" 
        "UI_task.c"
        "sdio_sdcard.c"
        "https_ota.c"
        "BLE_SPP_SERVER/ble_data_sender.c"
        "BLE_SPP_SERVER/ble_protocol_integration.c"
        "BLE_SPP_SERVER/ble_protocol_test.c"
        "BLE_SPP_SERVER/protocol_demo.c"
        "BLE_SPP_SERVER/protocol_handler.c"
        "BLE_SPP_SERVER/protocol_test.c"
        "BLE_SPP_SERVER/rf_tesing_configuration_cmd.c"
        "BLE_SPP_SERVER/ble_spp_server_demo.c"
        "blufi/blufi_example_main.c"
        "blufi/blufi_init.c"
        "blufi/blufi_security.c"
        # "lv_port/lv_port_disp.c"

    INCLUDE_DIRS 
        "." 
        "${CMAKE_SOURCE_DIR}/components/gh_algo/include_2" 
        "${CMAKE_SOURCE_DIR}/components/gh_algo/include" 
        # "${CMAKE_SOURCE_DIR}/main/gh_algo/include_2" 
        # "${CMAKE_SOURCE_DIR}/main/gh_algo/include" 
        "${CMAKE_SOURCE_DIR}/components/led/include"
        "${CMAKE_SOURCE_DIR}/main/BLE_SPP_SERVER"
        "${CMAKE_SOURCE_DIR}/main/blufi"
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
        esp_https_ota
        app_update
        bt
        console
        mbedtls
        esp_wifi
        # BSP
        # lvgl
    WHOLE_ARCHIVE
    EMBED_TXTFILES ${project_dir}/server_certs/ca_cert.pem
)

# add_prebuilt_library(prebuilt 
#                             "./gh_algo/lib/esp32_xtensa-esp32-gcc-14.2.0_os_common_dl.a"
#                             "./gh_algo/lib/esp32_xtensa-esp32-gcc-14.2.0_os_common_dsp.a"
#                             "./gh_algo/lib/esp32_xtensa-esp32-gcc-14.2.0_os_hr.a"
#                             "./gh_algo/lib/esp32_xtensa-esp32-gcc-14.2.0_os_hrv.a"
#                             "./gh_algo/lib/esp32_xtensa-esp32-gcc-14.2.0_os_ecg.a"
#                             "./gh_algo/lib/esp32_xtensa-esp32-gcc-14.2.0_os_nadt.a"
#                             "./gh_algo/lib/esp32_xtensa-esp32-gcc-14.2.0_os_spo2.a"
# )

# target_link_libraries(${COMPONENT_LIB} PRIVATE prebuilt)

# set(GH_ALGO_LIBS
#     ${CMAKE_CURRENT_SOURCE_DIR}/../components/gh_algo/lib/esp32_xtensa-esp32-gcc-14.2.0_os_common_dl.a
#     ${CMAKE_CURRENT_SOURCE_DIR}/../components/gh_algo/lib/esp32_xtensa-esp32-gcc-14.2.0_os_common_dsp.a
#     ${CMAKE_CURRENT_SOURCE_DIR}/../components/gh_algo/lib/esp32_xtensa-esp32-gcc-14.2.0_os_hr.a
#     ${CMAKE_CURRENT_SOURCE_DIR}/../components/gh_algo/lib/esp32_xtensa-esp32-gcc-14.2.0_os_hrv.a
#     ${CMAKE_CURRENT_SOURCE_DIR}/../components/gh_algo/lib/esp32_xtensa-esp32-gcc-14.2.0_os_ecg.a
#     ${CMAKE_CURRENT_SOURCE_DIR}/../components/gh_algo/lib/esp32_xtensa-esp32-gcc-14.2.0_os_nadt.a
#     ${CMAKE_CURRENT_SOURCE_DIR}/../components/gh_algo/lib/esp32_xtensa-esp32-gcc-14.2.0_os_spo2.a
# )
# add_prebuilt_library(gh_algo_lib GH_ALGO_LIBS) 

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
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../components/gh_algo/lib/"
)
target_link_libraries(${COMPONENT_LIB}
    INTERFACE
    "-Wl,--start-group"
    ${GH_ALGO_LIBS}
    "-Wl,--end-group"
)

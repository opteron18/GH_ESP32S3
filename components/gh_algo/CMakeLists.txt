idf_component_register(
    SRCS 
        "src/gh3x_demo_algo_call_adt.c" 
        "src/gh3x_demo_algo_call_bia.c" 
        "src/gh3x_demo_algo_call_ecg.c" 
        "src/gh3x_demo_algo_call_gsr.c" 
        "src/gh3x_demo_algo_call_hr.c" 
        "src/gh3x_demo_algo_call_hrv.c" 
        "src/gh3x_demo_algo_call_nadt.c" 
        "src/gh3x_demo_algo_call_spo2.c" 
        "src/gh3x_demo_algo_call.c" 
        "src/gh3x_demo_algo_config.c" 
        "src/gh3x_demo_algo_hook.c" 
        "src/gh3x_demo_algo_memory.c" 
        "src/gh3x_demo_strategy.c" 
        "src/goodix_ecg_config.c" 
        "src/goodix_hba_config.c" 
        "src/goodix_hrv_config.c" 
        "src/goodix_nadt_config.c" 
        "src/goodix_spo2_config_for_GH3300_c3450c.c" 
        "src/goodix_spo2_net_for_GH3300_c3450c.c" 
        "src/goodix_sys_bia.c" 
        "src/heart_net_org_dll.c" 
        "src/HRNet_knBasicWeights.c" 
        "src/HRNet_knConfNetWeights.c" 
        "src/HRNet_knMultiWeights.c" 
        "src/HRNet_knSceneNetWeights.c" 
        "src/HRNet_knSceneSwitchWeights.c" 
        "src/HRNet_knSmallWeights.c" 
        "src/HRNet_knTdfusionWeights.c" 
        "src/HRNet_knWeights.c" 
        "src/iot_sys_common_lib.c"
    # INCLUDE_DIRS "include" "." "../gh_drv/include" "lib"
    INCLUDE_DIRS  "."  "lib" "include"
    REQUIRES "${public_requires}" "freertos"  gh_drv gh_algo 
)

# add_prebuilt_library(prebuilt 
#     "lib/esp32_xtensa-esp32-gcc-14.2.0_os_common_dl.a"
#     "lib/esp32_xtensa-esp32-gcc-14.2.0_os_common_dsp.a"
#     "lib/esp32_xtensa-esp32-gcc-14.2.0_os_ecg.a"
#     "lib/esp32_xtensa-esp32-gcc-14.2.0_os_hr.a"
#     "lib/esp32_xtensa-esp32-gcc-14.2.0_os_hrv.a"
#     "lib/esp32_xtensa-esp32-gcc-14.2.0_os_nadt.a"
#     "lib/esp32_xtensa-esp32-gcc-14.2.0_os_spo2.a"
#     PRIV_REQUIRES gh_algo)
set(GH_ALGO_LIBS
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32_xtensa-esp32-gcc-14.2.0_os_common_dl.a
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32_xtensa-esp32-gcc-14.2.0_os_common_dsp.a
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32_xtensa-esp32-gcc-14.2.0_os_ecg.a
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32_xtensa-esp32-gcc-14.2.0_os_hr.a
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32_xtensa-esp32-gcc-14.2.0_os_hrv.a
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32_xtensa-esp32-gcc-14.2.0_os_nadt.a
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32_xtensa-esp32-gcc-14.2.0_os_spo2.a
)
target_link_libraries(${COMPONENT_LIB}
    "-Wl,--start-group"
    ${GH_ALGO_LIBS}
    "-Wl,--end-group"
)
# target_link_libraries(${COMPONENT_LIB} 
#     -Wl,--start-group
#     "lib/esp32_xtensa-esp32-gcc-14.2.0_os_common_dl.a"
#     "lib/esp32_xtensa-esp32-gcc-14.2.0_os_common_dsp.a"
#     "lib/esp32_xtensa-esp32-gcc-14.2.0_os_ecg.a"
#     "lib/esp32_xtensa-esp32-gcc-14.2.0_os_hr.a"
#     "lib/esp32_xtensa-esp32-gcc-14.2.0_os_hrv.a"
#     "lib/esp32_xtensa-esp32-gcc-14.2.0_os_nadt.a"
#     "lib/esp32_xtensa-esp32-gcc-14.2.0_os_spo2.a"
#     -Wl,--end-group
# )


# 声明静态库
# add_library(mylib STATIC IMPORTED)
# set_target_properties(mylib PROPERTIES IMPORTED_LOCATION 
#     "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32_xtensa-esp32-gcc-14.2.0_os_common_dl.a"
#     "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32_xtensa-esp32-gcc-14.2.0_os_common_dsp.a"
#     "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32_xtensa-esp32-gcc-14.2.0_os_ecg.a"
#     "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32_xtensa-esp32-gcc-14.2.0_os_hr.a"
#     "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32_xtensa-esp32-gcc-14.2.0_os_hrv.a"
#     "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32_xtensa-esp32-gcc-14.2.0_os_nadt.a"
#     "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32_xtensa-esp32-gcc-14.2.0_os_spo2.a"

# )

# 指定头文件路径
# target_include_directories(mylib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# 将库注册为组件依赖
# register_component(mylib)

# add_prebuilt_library(PREBUILT_LIB_FILES
#     "lib/esp32_xtensa-esp32-gcc-14.2.0_os_common_dl.a"
#     "lib/esp32_xtensa-esp32-gcc-14.2.0_os_common_dsp.a"
#     "lib/esp32_xtensa-esp32-gcc-14.2.0_os_ecg.a"
#     "lib/esp32_xtensa-esp32-gcc-14.2.0_os_hr.a"
#     "lib/esp32_xtensa-esp32-gcc-14.2.0_os_hrv.a"
#     "lib/esp32_xtensa-esp32-gcc-14.2.0_os_nadt.a"
#     "lib/esp32_xtensa-esp32-gcc-14.2.0_os_spo2.a"
#     PRIV_REQUIRES gh_algo gh_drv
# )

# target_link_libraries(${COMPONENT_LIB} PRIVATE PREBUILT_LIB_FILES )


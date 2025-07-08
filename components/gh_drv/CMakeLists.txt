
idf_component_register(
    SRCS 
        "src/BiaSampleManager.c" 
        "src/gh_gsr.c" 
        "src/gh_multi_sen_pro.c" 
        "src/gh_package.c" 
        "src/gh_rpccore.c" 
        "src/gh3x_bia_cali_processor.c" 
        "src/gh3x_bia_impedance_fixer.c" 
        "src/gh3x_bia_invalid_data_filter.c" 
        "src/gh3x_bia_offset_processor.c" 
        "src/gh3x_demo_hook.c" 
        "src/gh3x_demo_protocol.c" 
        "src/gh3x_demo_reg_array.c" 
        "src/gh3x_demo_struct.c" 
        "src/gh3x_demo_user.c" 
        "src/gh3x_demo.c" 
        "src/gh3x_drv_config.c" 
        "src/gh3x_drv_control.c" 
        "src/gh3x_drv_interface.c" 
        "src/gh3x_gsensor_control.c" 
        "src/gh3x_protocol_cmd.c" 
        "src/gh3x_soft_adt.c" 
        "src/gh3x_soft_led_agc.c" 
        "src/impedance_algo.c" 
        "src/slabmemory.c" 
        "src/staticmapimp.c" 
        "src/gh3x_drvlib.c" 
        "src/gh3x_fifo_analyze.c"
    
    INCLUDE_DIRS "include" "." "../gh_algo/include"
    REQUIRES "${public_requires}" "freertos" "driver" gh_algo )


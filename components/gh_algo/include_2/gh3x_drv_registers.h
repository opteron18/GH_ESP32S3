/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x_drv_registers.h
 *
 * @brief   gh3x driver registers define
 *
 * @version ref gh3x_drv_version.h
 *
 */

#ifndef _GH3X_DRV_REGISTERS_H_
#define _GH3X_DRV_REGISTERS_H_

/* register mapping definition */

#define GH3X_REG_INT_EVENT1_EN_ADDR                       (0x0602)
#define GH3X_REG_INT_EVENT2_EN_ADDR                       (0x0604)
#define GH3X_REG_INT_EVENT1_ADDR                          (0x060A)
#define GH3X_REG_INT_EVENT2_ADDR                          (0x060C)
#define GH3X_REG_FIFO_USE_ADDR                            (0x060E)
#define GH3X_REG_CHIP_READY_CODE_REG_ADDR                 (0x0086)

#define GH3X_REG_SLOT_SAMPLE_START_ADDR                   (0x0008)
#define GH3X_REG_SLOT_SAMPLE_START_MSB                    (0)
#define GH3X_REG_SLOT_SAMPLE_START_LSB                    (0)


#define GH3X_REG_ADT_LEADON_CR_ADDR                       (0x0500)
#define GH3X_REG_LD_GSR_DET_EN_BIT                        (0)
#define GH3X_REG_LD_ECG0_DET_EN_BIT                       (1)
#define GH3X_REG_LD_ECG1_DET_EN_BIT                       (2)

#define GH3X_REG_WEARON_CR_ADDR                           (0x0518)
#define GH3X_REG_WEAR_STATUS_BIT                          (4)

#define GH3X_REG_PMU_REG0_ADDR                            (0x0780)
#define GH3X_REG_IA_DC_EN_BIT                             (8)
#define GH3X_REG_IA_AC_EN_BIT                             (9)

#define GH3X_REG_ECG_CTRL0_ADDR                           (0x0252)
#define GH3X_REG_ECG0_EN_BIT                              (0)
#define GH3X_REG_ECG1_EN_BIT                              (1)
#define GH3X_REG_ECGDC_EN_BIT                             (2)

#define GH3X_REG_GSR_CTRL1_ADDR                           (0x0204)
#define GH3X_REG_GSR_EN_BIT                               (0)
#define GH3X_REG_GSR_MODE_BIT                             (1)
  
#define GH3X_REG_BIA_CTRL0_ADDR                           (0x0208)
#define GH3X_REG_BIA_EN_BIT                               (0)
	
#define GH3X_REG_SLOT_INDEX_CTRL0_ADDR                    (0x0C00)
#define GH3X_REG_SLOT_EN0_ADDR                            (0x0C20)
	 
	
#define GH3X_REG_TIMER_CTRL_ADDR                           (0x0FC0)

#define	GH3X_INSTRUCTIONS_CHIP_INIED_REG_ADDR              (0x0094)
#define GH3X_INSTRUCTIONS_CHIP_INIED_REG_VAL               (0x0011)    /**< reg rg_chip_sw_backup bit4&bit0 set 1(0x0011) */

// top ctrl block, start addr: 0x0000
 
  

// special reg addr
#define   GH3X_FIFO_REG_ADDR                      (0xAAAA)    /**< fifo reg addr */
#define   GH3X_I2C_CMD_ADDR                       (0xDDDD)    /**< i2c cmd reg addr */


#define   GH3X_REG_ADDR_SIZE                      (0x0002)    /**< reg addr size */
#define   GH3X_REG_ADDR_EVEN_FIXED                (0xFFFE)    /**< reg addr even fixed */
#define   GH3X_REG_ADDR_MAX                       (0xFFFF)    /**< reg addr max */

/* Register bit definition */ 
#define   GH3X_CHIP_READY_CODE_REG_ADDR             (0x0086) 
#define   GH3X_CHIP_READY_REG_VAL                 (0xAA55)    /**< chip ready value */

 
  
#define   GH3X_SLOT_CTRL_OFFSET                   (0x001C)    /**< slot ctrl reg offset */

 
#define   GH3X_SKIN_COLOR_STATUS_MASK_BIT         (0x0003u)   /**< skin color status bit mask */

#define   GH3X_SLOT_LED_CURRENT_CLEAR_MASK        (0x00FFu)   /**< slot led current clear mask */

/* utils reg macro */

#define   GH3X_SLOT_NUM_MAX                       (32)         /**< slot num max */

#define   GH3X_SLOT_LED_DRV_NUM_MAX               (4)         /**< slot led driver max */

#define   GH3X_SLOT_TIA_GAIN_NUM_MAX              (4)         /**< slot tia gain num max */

#define   GH3X_SLOT_TIA_GAIN_VAL_MAX              (13)        /**< slot tia gain val max */

#define   GH3X_SLOT_TIA_GAIN_BITS_SIZE            (4)         /**< slot tia gain val bits size */

#define   GH3X_SLOT_TIA_GAIN_BITS_MARK            (0x000F)    /**< slot tia gain val bits mark */
 
/* reg bit field index */
 






#endif /* _GH3X_DRV_REGISTERS_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/

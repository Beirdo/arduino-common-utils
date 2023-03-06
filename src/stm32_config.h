#ifndef __stm32_config_h_
#define __stm32_config_h_

#ifdef STM32F0xx

#include "hal_conf_extra.h"

#ifdef HAL_I2C_MODULE_DISABLED
#undef USE_I2C
#else
#define USE_I2C
#endif

#ifdef HAL_SPI_MODULE_DISABLED
#undef USE_SPI
#else
#define USE_SPI
#endif

#endif

#endif

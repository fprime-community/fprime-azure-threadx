/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/

#ifndef FX_STM32_SRAM_DRIVER_H
#define FX_STM32_SRAM_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "fx_api.h"

/* include the stm32h7xx.h to be able to access the memory region defines */
#include "stm32h7xx.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
#define FX_SRAM_DISK_BASE_ADDRESS         fxSRAMDisk
#define FX_SRAM_DISK_SIZE                 1024 * 32

/* USER CODE BEGIN EC */
/* USER CODE BEGIN SRAMDiskSection */
#if defined ( __ICCARM__ ) /* IAR Compiler */
#pragma location = ".SRAMDiskSection"
#elif defined ( __CC_ARM ) /* MDK ARM Compiler */
__attribute__((section(".SRAMDiskSection")))
#elif defined ( __GNUC__ ) /* GNU Compiler */
__attribute__((section(".SRAMDiskSection")))
#endif
/* USER CODE END SRAMDiskSection */

ALIGN_32BYTES(static UCHAR fxSRAMDisk[FX_SRAM_DISK_SIZE]);
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
VOID fx_stm32_sram_driver(FX_MEDIA *media_ptr);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

#ifdef __cplusplus
}
#endif
#endif /* FX_STM32_SRAM_DRIVER_H */


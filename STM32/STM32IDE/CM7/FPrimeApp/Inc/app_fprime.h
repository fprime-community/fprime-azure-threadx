/*
 * fprime_app.hpp
 *
 *  Created on: 29 Oct 2021
 *      Author: cmjl
 */

#ifndef __APP_FPRIME_H_
#define __APP_FPRIME_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "tx_api.h"
#include "../../Core/Inc/stm32ide_main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN EFP */

UINT App_FPrime_Init(VOID *memory_ptr);

void run_test(int test_num);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define     APP_FPRIME_STACK_SIZE                         1024*32

#define     UT_THREAD_PRIO                                8
/* USER CODE END PD */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

#ifdef __cplusplus
}
#endif

#endif /* __APP_FPRIME_H_ */

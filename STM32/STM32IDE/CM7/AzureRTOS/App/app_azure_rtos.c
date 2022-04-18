/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_azure_rtos.c
  * @author  MCD Application Team
  * @brief   azure_rtos application implementation file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_azure_rtos.h"
#include "tx_byte_pool.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */
#if (USE_MEMORY_POOL_ALLOCATION == 1)
/* USER CODE BEGIN TX_Pool_Buffer */
/* USER CODE END TX_Pool_Buffer
 * NOTE: ThreadX tx_byte_pool_create requires that the starting address must be
 * aligned to the ULONG data type */
static UCHAR tx_byte_pool_buffer[TX_APP_MEM_POOL_SIZE] __attribute__ ((aligned (sizeof(ULONG))));
static TX_BYTE_POOL tx_app_byte_pool;

static UCHAR  fx_byte_pool_buffer[FX_APP_MEM_POOL_SIZE]  __attribute__ ((aligned (sizeof(ULONG))));
static TX_BYTE_POOL fx_app_byte_pool;
#endif

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
  * @brief  Define the initial system.
  * @param  first_unused_memory : Pointer to the first unused memory
  * @retval None
  */
VOID tx_application_define(VOID *first_unused_memory)
{
    /* USER CODE BEGIN  tx_application_define_1*/

    /* USER CODE END  tx_application_define_1 */
#if (USE_MEMORY_POOL_ALLOCATION == 1)
  VOID *memory_ptr;

  if (tx_byte_pool_create(&tx_app_byte_pool, "Tx App memory pool", tx_byte_pool_buffer, TX_APP_MEM_POOL_SIZE) != TX_SUCCESS)
  {
    /* USER CODE BEGIN TX_Byte_Pool_Error */

    /* USER CODE END TX_Byte_Pool_Error */
  }
  else
  {
    /* USER CODE BEGIN TX_Byte_Pool_Success */

    /* USER CODE END TX_Byte_Pool_Success */
    memory_ptr = (VOID *)&tx_app_byte_pool;
      /* USER CODE BEGIN  App_ThreadX_Init_Success */

      /* USER CODE END  App_ThreadX_Init_Success */

    // FileX initialisation
    fx_system_initialize();

#ifdef UT_FPRIME
    if (App_FPrime_Init(memory_ptr) != TX_SUCCESS)
    {
          /* USER CODE BEGIN  App_FPrime_Init_Error */

          /* USER CODE END  App_FPrime_Init_Error */
    }
#endif
      /* USER CODE BEGIN  App_FPrime_Init_Success */
      /* USER CODE END  App_FPrime_Init_Success */

  }

#ifdef FX_TEST_CODE
  if (tx_byte_pool_create(&fx_app_byte_pool, "Fx App memory pool", fx_byte_pool_buffer, FX_APP_MEM_POOL_SIZE) != TX_SUCCESS)
  {
    /* USER CODE BEGIN FX_Byte_Pool_Error */

    /* USER CODE END FX_Byte_Pool_Error */
  }
  else
  {
    /* USER CODE BEGIN FX_Byte_Pool_Success */

    /* USER CODE END FX_Byte_Pool_Success */

    // FileX initialisation
    fx_system_initialize();

    memory_ptr = (VOID *)&fx_app_byte_pool;

    if (MX_FileX_Init(memory_ptr) != FX_SUCCESS)
    {
      /* USER CODE BEGIN MX_FileX_Init_Error */

      /* USER CODE END MX_FileX_Init_Error */
    }

    /* USER CODE BEGIN MX_FileX_Init_Success */

    /* USER CODE END MX_FileX_Init_Success */
  }
#endif // FX_TEST_CODE

#else
/*
 * Using dynamic memory allocation requires to apply some changes to the linker file.
 * ThreadX needs to pass a pointer to the first free memory location in RAM to the tx_application_define() function,
 * using the "first_unused_memory" argument.
 * This require changes in the linker files to expose this memory location.
 * For EWARM add the following section into the .icf file:
     place in RAM_region    { last section FREE_MEM };
 * For MDK-ARM
     - either define the RW_IRAM1 region in the ".sct" file
     - or modify the line below in "tx_low_level_initilize.s to match the memory region being used
        LDR r1, =|Image$$RW_IRAM1$$ZI$$Limit|

 * For STM32CubeIDE add the following section into the .ld file:
     ._threadx_heap :
       {
          . = ALIGN(8);
          __RAM_segment_used_end__ = .;
          . = . + 64K;
          . = ALIGN(8);
        } >RAM_D1 AT> RAM_D1
    * The simplest way to provide memory for ThreadX is to define a new section, see ._threadx_heap above.
    * In the example above the ThreadX heap size is set to 64KBytes.
    * The ._threadx_heap must be located between the .bss and the ._user_heap_stack sections in the linker script.
    * Caution: Make sure that ThreadX does not need more than the provided heap memory (64KBytes in this example).
    * Read more in STM32CubeIDE User Guide, chapter: "Linker script".

 * The "tx_initialize_low_level.s" should be also modified to enable the "USE_DYNAMIC_MEMORY_ALLOCATION" flag.
 */

  /* USER CODE BEGIN DYNAMIC_MEM_ALLOC */
  (void)first_unused_memory;
  /* USER CODE END DYNAMIC_MEM_ALLOC */
#endif

}

/**
  * @brief  MX_AZURE_RTOS_Init
  * @param  None
  * @retval None
  */
void MX_AZURE_RTOS_Init(void)
{
  /* USER CODE BEGIN  Before_Kernel_Start */

  /* USER CODE END  Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN  Kernel_Start_Error */

  /* USER CODE END  Kernel_Start_Error */
}

/* USER CODE BEGIN  2 */

/**
  * @brief  function returning a pointer to the ThreadX memory byte pool
  * @param  Pointer to store ThreadX memory byte pool
  * @retval Operation get status
  *
  * @note   The pool must have been created before calling this function
  */
UINT app_tx_get_byte_pool(TX_BYTE_POOL** pool_ptr)
{
    /* Default status to success.  */
    UINT status =  TX_SUCCESS;

    /* Check for an invalid byte pool pointer.  */
    if (pool_ptr == TX_NULL)
    {
        /* Null destination pointer, return appropriate error.  */
        status =  TX_PTR_ERROR;
    }
    /* Now check for invalid pool ID.  */
    else if (tx_app_byte_pool.tx_byte_pool_id != TX_BYTE_POOL_ID)
    {
        /* Byte pool pointer is invalid, return appropriate error code.  */
        status =  TX_POOL_ERROR;
    }
    else
    {
        *pool_ptr = &tx_app_byte_pool;
    }

    return status;
}

/* USER CODE END  2 */

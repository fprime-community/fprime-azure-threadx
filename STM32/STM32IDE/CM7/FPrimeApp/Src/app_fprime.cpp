/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  Spin.Works
  * @brief   ThreadX applicative file
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
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "app_fprime.h"
#include "app_filex.h"

#include "Assert.hpp"
#include "BasicTypes.hpp"
#include "WatchdogTimer.hpp"
#include "EightyCharString.hpp"
#include <Task.hpp>
#include <Os/Queue.hpp>
#include <File.hpp>
#include <FileSystem.hpp>
#include <Fw/Types/EightyCharString.hpp>
#include <Fw/Types/Assert.hpp>

extern "C" {
  void startTestTask(int iters);
  void qtest_block_receive();
  void qtest_nonblock_receive();
  void qtest_nonblock_send();
  void qtest_block_send();
  void qtest_performance();
  void qtest_concurrent();
  void intervalTimerTest();
  void watchdogTest();
  void fileSystemTest();
  void validateFileTest(const char* filename);
}
const char* filename = "some_test_file.txt";

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
static TX_THREAD UTThread;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void UT_Entry(ULONG thread_input);

//void fx_ls(void);
/* USER CODE END PFP */



void run_test(int test_num)
{
    switch(test_num) {
        case 0:
            startTestTask(10);
            break;
        case 1:
            qtest_block_receive();
            break;
        case 2:
            qtest_nonblock_receive();
            break;
        case 3:
            qtest_nonblock_send();
            break;
        case 4:
            qtest_block_send();
            break;
        case 5:
            qtest_performance();
            break;
        case 6:
            qtest_concurrent();
            break;
        case 7:
            intervalTimerTest();
            break;
        case 8:
            fileSystemTest();
            break;
        case 9:
            validateFileTest(filename);
            break;
        case 10:
           watchdogTest();
           break;
        default:
            fprintf(stderr, "Invalid test number: %d\n", test_num);
                break;
    }

}

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_FPrime_Init(VOID *memory_ptr)
{
    UINT ret = TX_SUCCESS;
    TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

    /* USER CODE BEGIN App_FPrime_Init */
    ULONG *pointer;

    /* Allocate the stack for MsgSenderThreadOne.  */
    if (tx_byte_allocate(byte_pool, (VOID **) &pointer, APP_FPRIME_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
    {
      ret = TX_POOL_ERROR;
    }

    /* Create MsgSenderThreadOne.  */
    if (tx_thread_create(&UTThread, "UT_Tests_Thread",
                         UT_Entry, 0, pointer, APP_FPRIME_STACK_SIZE,
                         UT_THREAD_PRIO, UT_THREAD_PRIO,
                         TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
    {
      ret = TX_THREAD_ERROR;
    }

    return ret;
}


/**
  * @brief  This service checks the specified media for basic structural errors,
  *         including file/directory cross-linking, invalid FAT chains, and lost clusters.
  *         This service also provides the capability to correct detected errors.
  *
  *         If status is FX_SUCCESS and detected_errors is 0, the media was successfully
  *         checked and found to be error free.
  *
  * @param  NONE
  * @retval media check status
  */
UINT media_check(void)
{
    UINT status;
    FX_MEDIA* mediaPtr;
    ULONG detected_errors;
    UCHAR sratch_memory[4096];

    file_sys_media_get(&mediaPtr);

    /* Check the media and correct all errors. */
    status = fx_media_check(mediaPtr,
                            sratch_memory,
                            4096,
                            FX_FAT_CHAIN_ERROR | FX_DIRECTORY_ERROR | FX_LOST_CLUSTER_ERROR | FX_FILE_SIZE_ERROR,
                            &detected_errors);

    return status;
}

/* USER CODE BEGIN 1 */
/**
  * @brief  Function implementing the UT thread function from where all
  *         UT runvoid UT_Entry(ULONG thread_input)
  * @param  thread_input: Not used
  * @retval None
  */
void UT_Entry(ULONG thread_input)
{
    (void)thread_input;

    // FileX initialisation
    if (filex_initialise() != FX_SUCCESS)
    {
      Error_Handler();
    }

    for( U32 ii = 0; ii <11; ii++ )
    {
    	printf("-- Test section[%d] started ----\n", ii);
        run_test(ii);
    }

    printf("-- All tests terminated ----\n");

    if(FX_SUCCESS != media_check())
    {
        FW_ASSERT(0);
    }

    /* Close the media.  */
    FX_MEDIA* mediaPtr;
    file_sys_media_get(&mediaPtr);
    if (fx_media_close(mediaPtr) != FX_SUCCESS)
    {
      Error_Handler();
    }

    /* this thread is not needed any more, we relinquish it */
    tx_thread_relinquish();
}
/* USER CODE END 1 */

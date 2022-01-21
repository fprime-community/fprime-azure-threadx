/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
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
#include "tim.h"
#include "tx_api.h"
#include "nx_api.h"
#include "app_threadx.h"
#include "Assert.hpp"
#include "BasicTypes.hpp"
#include "EightyCharString.hpp"
#include <Task.hpp>
#include <Os/Queue.hpp>
#include <Fw/Types/EightyCharString.hpp>
#include <Fw/Types/Assert.hpp>


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
//TX_THREAD              MsgSenderThreadOne;
//TX_THREAD              MsgReceiverThread;
//TX_THREAD              MsgSenderThreadTwo;
TX_QUEUE               MsgQueueOne;
TX_QUEUE               MsgQueueTwo;
//*F Prime --------------------------------
Os::Task MsgSenderThreadOne;
Os::Task MsgReceiverThread;
Os::Task MsgSenderThreadTwo;

Os::Queue* pMsgQueueOne = new Os::Queue();
Os::Queue* pMsgQueueTwo = new Os::Queue();
//*/
//*F Prime --------------------------------
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
//void    MsgSenderThreadOne_Entry(ULONG thread_input);
//void    MsgSenderThreadTwo_Entry(ULONG thread_input);
//void    MsgReceiverThread_Entry(ULONG thread_input);

//*F Prime --------------------------------
void    MsgSenderThreadOne_Entry(void* thread_input);
void    MsgSenderThreadTwo_Entry(void* thread_input);
void    MsgReceiverThread_Entry(void* thread_input);
//*/
//*F Prime --------------------------------
/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
      UINT ret = TX_SUCCESS;
      TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

      /* USER CODE BEGIN App_ThreadX_MEM_POOL */
      /* USER CODE END App_ThreadX_MEM_POOL */

      /* USER CODE BEGIN App_ThreadX_Init */
      ULONG *pointer;

//      /* Allocate the stack for MsgSenderThreadOne.  */
//      if (tx_byte_allocate(byte_pool, (VOID **) &pointer,
//                           APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
//      {
//        ret = TX_POOL_ERROR;
//      }
//
//      /* Create MsgSenderThreadOne.  */
//      if (tx_thread_create(&MsgSenderThreadOne, "Message Queue Sender Thread One",
//                           MsgSenderThreadOne_Entry, 0, pointer, APP_STACK_SIZE,
//                           SENDER_THREAD_PRIO, SENDER_THREAD_PREEMPTION_THRESHOLD,
//                           TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
//      {
//        ret = TX_THREAD_ERROR;
//      }
      //* MsgSenderThreadOne_Entry  -----------------

      Fw::EightyCharString mqsend1Name("Message Queue Sender Thread One");

      Os::Task::TaskStatus mqsend1Status = MsgSenderThreadOne.start(mqsend1Name,
                                                                    0,
                                                                    SENDER_THREAD_PRIO,
                                                                    APP_STACK_SIZE,
                                                                    MsgSenderThreadOne_Entry,
                                                                    0,
                                                                    0);

      if (mqsend1Status != Os::Task::TaskStatus::TASK_OK)
      {
        return NX_NOT_ENABLED;
      }
      //*/

//      /* Allocate the stack for MsgSenderThreadTwo.  */
//      if (tx_byte_allocate(byte_pool, (VOID **) &pointer,
//                           APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
//      {
//        ret = TX_POOL_ERROR;
//      }
//
//      /* Create MsgSenderThreadTwo.  */
//      if (tx_thread_create(&MsgSenderThreadTwo, "Message Queue Sender Thread Two",
//                           MsgSenderThreadTwo_Entry, 0, pointer, APP_STACK_SIZE,
//                           SENDER_THREAD_PRIO, SENDER_THREAD_PREEMPTION_THRESHOLD,
//                           TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
//      {
//        ret = TX_THREAD_ERROR;
//      }
      //* MsgSenderThreadTwo_Entry  -----------------

      Fw::EightyCharString mqsend2Name("Message Queue Sender Thread Two");

      Os::Task::TaskStatus mqsend2Status = MsgSenderThreadTwo.start(mqsend2Name,
                                                                    0,
                                                                    SENDER_THREAD_PRIO,
                                                                    APP_STACK_SIZE,
                                                                    MsgSenderThreadTwo_Entry,
                                                                    0,
                                                                    0);

      if (mqsend2Status != Os::Task::TaskStatus::TASK_OK)
      {
        return NX_NOT_ENABLED;
      }
      //*/

//      /* Allocate the stack for MsgReceiverThread.  */
//      if (tx_byte_allocate(byte_pool, (VOID **) &pointer,
//                           APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
//      {
//        ret = TX_POOL_ERROR;
//      }
//
//      /* Create MsgReceiverThread.  */
//      if (tx_thread_create(&MsgReceiverThread, "Message Queue Receiver Thread",
//                           MsgReceiverThread_Entry, 0, pointer, APP_STACK_SIZE,
//                           RECEIVER_THREAD_PRIO, RECEIVER_THREAD_PREEMPTION_THRESHOLD,
//                           TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
//      {
//        ret = TX_THREAD_ERROR;
//      }
      //* MsgReceiverThread_Entry  -----------------

      Fw::EightyCharString mqrecvName("Message Queue Receiver Thread");

      Os::Task::TaskStatus mqrecvStatus = MsgReceiverThread.start(mqrecvName,
                                                                  0,
                                                                  RECEIVER_THREAD_PRIO,
                                                                  APP_STACK_SIZE,
                                                                  MsgReceiverThread_Entry,
                                                                  0,
                                                                  0);

      if (mqrecvStatus != Os::Task::TaskStatus::TASK_OK)
      {
        return NX_NOT_ENABLED;
      }
      //*/

//      /* Allocate the MsgQueueOne.  */
//      if (tx_byte_allocate(byte_pool, (VOID **) &pointer,
//                           APP_QUEUE_SIZE*sizeof(ULONG), TX_NO_WAIT) != TX_SUCCESS)
//      {
//        ret = TX_POOL_ERROR;
//      }
//
//      /* Create the MsgQueueOne shared by MsgSenderThreadOne and MsgReceiverThread */
//      if (tx_queue_create(&MsgQueueOne, "Message Queue One",TX_1_ULONG,
//                          pointer, APP_QUEUE_SIZE*sizeof(ULONG)) != TX_SUCCESS)
//      {
//        ret = TX_QUEUE_ERROR;
//      }

      //* MsgQueueOne  -----------------
      Fw::EightyCharString queueRecvOneName("Message Queue One");
      Os::Queue::QueueStatus recvStatQ1 = pMsgQueueOne->create(queueRecvOneName, APP_QUEUE_SIZE, TX_1_ULONG*sizeof(ULONG));
      FW_ASSERT(recvStatQ1 == Os::Queue::QUEUE_OK, recvStatQ1);
      //* MsgQueueOne  -----------------

//      /* Allocate the MsgQueueTwo.  */
//      if (tx_byte_allocate(byte_pool, (VOID **) &pointer,
//                           APP_QUEUE_SIZE*sizeof(ULONG), TX_NO_WAIT) != TX_SUCCESS)
//      {
//        ret = TX_POOL_ERROR;
//      }
//
//      /* Create the MsgQueueTwo shared by MsgSenderThreadTwo and MsgReceiverThread.  */
//      if (tx_queue_create(&MsgQueueTwo, "Message Queue Two", TX_1_ULONG,
//                          pointer, APP_QUEUE_SIZE*sizeof(ULONG)) != TX_SUCCESS)
//      {
//        ret = TX_QUEUE_ERROR;
//      }
      //* MsgQueueOne  -----------------
      Fw::EightyCharString queueRecvTwoName("Message Queue two");
      Os::Queue::QueueStatus recvStatQ2 = pMsgQueueTwo->create(queueRecvTwoName, APP_QUEUE_SIZE, TX_1_ULONG*sizeof(ULONG));
      FW_ASSERT(recvStatQ2 == Os::Queue::QUEUE_OK, recvStatQ2);
      //* MsgQueueOne  -----------------

      /* USER CODE END App_ThreadX_Init */

      return ret;
}

/* USER CODE BEGIN 1 */
/**
  * @brief  Function implementing the MsgSenderThreadOne thread.
  * @param  thread_input: Not used
  * @retval None
  */
//void MsgSenderThreadOne_Entry(ULONG thread_input)

//*F Prime --------------------------------
void MsgSenderThreadOne_Entry(void* thread_input)
//*/
//*F Prime --------------------------------
{
//  U8 Msg[6] __attribute__ ((aligned (sizeof(ULONG))));
//  Msg[0]= 0xDE;
//  Msg[1]= 0xAD;
//  Msg[2]= 0xBE;
//  Msg[3]= 0xAF;
//  Msg[4]= 0xA0;
//  Msg[5]= 0xA1;
    ULONG Msg = 0xDEADBEEF;

  (void) thread_input;
  /* Infinite loop */
  printf("Nx_SNTP_Client::MsgSenderThreadOne_Entry\n");
  while(1)
  {
    /* Send message to MsgQueueOne.  */
//    if (tx_queue_send(&MsgQueueOne, &Msg, TX_WAIT_FOREVER) != TX_SUCCESS)
//    {
//      Error_Handler();
//    }
    //*F Prime --------------------------------
    // Raw buffer Queue::send
    Os::Queue::QueueStatus stat = pMsgQueueOne->send((const U8*)&Msg, sizeof(ULONG), 0, Os::Queue::QUEUE_NONBLOCKING);
    switch(stat)
    {
        case Os::Queue::QUEUE_OK:
            printf("TX Q1, available slots[%d]\n", pMsgQueueOne->getMaxMsgs());
            break;
        case Os::Queue::QUEUE_FULL:
            printf("Q1 is FULL\n");
//            MsgSenderThreadOne.suspend(0);
//            printf("MsgSenderThreadOne_Entry suspended[%d]\n", MsgSenderThreadOne.isSuspended());
            break;
        default:
            // stat == Os::Queue::QUEUE_UNKNOWN_ERROR
            Error_Handler();
            break;
    }
    //*/
    //*F Prime --------------------------------

    /* Sleep for 200ms */
    tx_thread_sleep(200);
  }
}

/**
  * @brief  Function implementing the MsgSenderThreadTwo thread.
  * @param  thread_input: Not used
  * @retval None
  */
//void MsgSenderThreadTwo_Entry(ULONG thread_input)
//*F Prime --------------------------------
void MsgSenderThreadTwo_Entry(void* thread_input)
//*/
//*F Prime --------------------------------
{
//  U8 Msg[6] __attribute__ ((aligned (sizeof(ULONG))));
//  Msg[0]= 0x60;
//  Msg[1]= 0x0D;
//  Msg[2]= 0xBE;
//  Msg[3]= 0xAF;
//  Msg[4]= 0xB0;
//  Msg[5]= 0xB1;
    ULONG Msg = 0x600DBEEF;

  (void) thread_input;
  /* Infinite loop */
  printf("Nx_SNTP_Client::MsgSenderThreadTwo_Entry\n");
  while(1)
  {
    /* Send message to MsgQueueTwo.  */
//    if (tx_queue_send(&MsgQueueTwo, &Msg, TX_WAIT_FOREVER) != TX_SUCCESS)
//    {
//      Error_Handler();
//    }
    //*F Prime --------------------------------
    // Raw buffer Queue::send
    Os::Queue::QueueStatus stat = pMsgQueueTwo->send((const U8*)&Msg, sizeof(ULONG), 0, Os::Queue::QUEUE_NONBLOCKING);
    switch(stat)
    {
        case Os::Queue::QUEUE_OK:
            printf("Tx Q2, available slots[%d]\n", pMsgQueueTwo->getMaxMsgs());
            break;
        case Os::Queue::QUEUE_FULL:
            printf("Q2 is FULL\n");
            break;
        default:
            // stat == Os::Queue::QUEUE_UNKNOWN_ERROR
            Error_Handler();
            break;
    }
    //*/
    //*F Prime --------------------------------

    /* Sleep for 500ms */
    tx_thread_sleep(400);
  }
}

/**
  * @brief  Function implementing the MsgReceiverThread thread.
  * @param  thread_input: Not used
  * @retval None
  */
//void MsgReceiverThread_Entry(ULONG thread_input)
//*F Prime --------------------------------
void MsgReceiverThread_Entry(void* thread_input)
//*/
//*F Prime --------------------------------
{
  ULONG RQ1Msg = 0;
  ULONG RQ2Msg = 0;
  UINT status = 0 ;
  (void) thread_input;
  /* Infinite loop */
  printf("Nx_SNTP_Client::MsgReceiverThread_Entry\n");
  while (1)
  {
    /* Determine whether a message MsgQueueOne or MsgQueueTwo is available */
//    status = tx_queue_receive(&MsgQueueOne, &RMsg, TX_NO_WAIT);
//    if (status == TX_SUCCESS)
//    {
//      /* Check Message value */
//      if (RMsg != TOGGLE_LED)
//      {
//        Error_Handler();
//      }
//      else
//      {
//        printf("MsgReceiverThread_Entry::Msg#1\n");
////        BSP_LED_Toggle(LED_GREEN);
//      }
//    }
//    else
//    {
//      status = tx_queue_receive(&MsgQueueTwo, &RMsg, TX_NO_WAIT);
//      if ( status == TX_SUCCESS)
//      {
//        /* Check Message value */
//        if (RMsg != TOGGLE_LED)
//        {
//          Error_Handler();
//        }
//        else
//        {
//          printf("MsgReceiverThread_Entry::Msg#2\n");
////          BSP_LED_Toggle(LED_RED);
//        }
//      }
//    }
    //*F Prime --------------------------------
    NATIVE_INT_TYPE actualSize;
    NATIVE_INT_TYPE priority = 0;
    ULONG refMsg1 = 0xDEADBEEF;
    ULONG refMsg2 = 0x600DBEEF;

    Os::Queue::QueueStatus statQ1 = pMsgQueueOne->receive((U8*)&RQ1Msg, 0, actualSize, priority, Os::Queue::QUEUE_NONBLOCKING);
    switch(statQ1)
    {
        case Os::Queue::QUEUE_OK:
            printf("RX Q1 recv[%d] data[0x%.8x] To read[%d]\n",actualSize, RQ1Msg, pMsgQueueOne->getNumMsgs());
            if(memcmp(&refMsg1,&RQ1Msg,sizeof(ULONG)) != 0)
            {
                printf("RX Q1 corruption\n");
                Error_Handler();
                FW_ASSERT(0);
            }
            RQ1Msg = 0;
            break;
        case Os::Queue::QUEUE_EMPTY_BUFFER:
            break;
        default:
            // stat == Os::Queue::QUEUE_UNKNOWN_ERROR
            Error_Handler();
            break;
    }

    Os::Queue::QueueStatus statQ2 = pMsgQueueTwo->receive((U8*)&RQ2Msg, 0, actualSize, priority, Os::Queue::QUEUE_NONBLOCKING);
    switch(statQ2)
    {
        case Os::Queue::QUEUE_OK:
            printf("RX Q2 recv[%d] data[0x%.8x] To read[%d]\n",actualSize, RQ2Msg, pMsgQueueTwo->getNumMsgs());
            if(memcmp(&refMsg2,&RQ2Msg,sizeof(ULONG)) != 0)
            {
                printf("RX Q2 corruption\n");
                Error_Handler();
                FW_ASSERT(0);
            }
            RQ2Msg = 0;
            break;
        case Os::Queue::QUEUE_EMPTY_BUFFER:
            break;
        default:
            // stat == Os::Queue::QUEUE_UNKNOWN_ERROR
            Error_Handler();
            break;
    }

    /* Sleep for 500ms */
    tx_thread_sleep(800);
    //*/
    //*F Prime --------------------------------
  }
}
/* USER CODE END 1 */

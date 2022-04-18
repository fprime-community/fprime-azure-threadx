/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include "tx_api.h"

/* USER CODE BEGIN 0 */

/**
* @brief  Retargets the C library printf function to the USART.
* @param  None
* @retval None
*/
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
  usart2_send((uint8_t *)&ch,1);

  return ch;
}

/**
 * \brief           USART RX buffer for DMA to transfer every received byte RX
 * \note            Contains raw data that are about to be processed by different events
 *
 * Special use case for STM32H7 series.
 * Default memory configuration in STM32H7 may put variables to DTCM RAM,
 * part of memory that is super fast, however DMA has no access to it.
 *
 * For this specific example, all variables are by default
 * configured in D1 RAM. This is configured in linker script
 */

/* USER CODE BEGIN Private defines */
usart_dma_buffers_t usart2_DMA_buffers;
usart_dma_buffers_t usart6_DMA_buffers;

/* USER CODE END 0 */

UART_HandleTypeDef huart3;

/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOD);
  /**USART2 GPIO Configuration
  PD5   ------> USART2_TX
  PD6   ------> USART2_RX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_5|LL_GPIO_PIN_6;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* USART2 DMA Init */

  /* USART2_RX Init */
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_STREAM_2, LL_DMAMUX1_REQ_USART2_RX);
  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_2, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
  LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_2, LL_DMA_PRIORITY_LOW);
  LL_DMA_SetMode(DMA1, LL_DMA_STREAM_2, LL_DMA_MODE_CIRCULAR);
  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_2, LL_DMA_PERIPH_NOINCREMENT);
  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_2, LL_DMA_MEMORY_INCREMENT);
  LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_2, LL_DMA_PDATAALIGN_BYTE);
  LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_2, LL_DMA_MDATAALIGN_BYTE);
  LL_DMA_DisableFifoMode(DMA1, LL_DMA_STREAM_2);
  LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_2, LL_USART_DMA_GetRegAddr(USART2, LL_USART_DMA_REG_DATA_RECEIVE));
  LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_2, (uint32_t)usart2_DMA_buffers.usart_rx_dma_buffer);
  LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_2, ARRAY_LEN(usart2_DMA_buffers.usart_rx_dma_buffer));

  /* USART2_TX Init */
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_STREAM_3, LL_DMAMUX1_REQ_USART2_TX);
  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_3, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
  LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_3, LL_DMA_PRIORITY_LOW);
  LL_DMA_SetMode(DMA1, LL_DMA_STREAM_3, LL_DMA_MODE_NORMAL);
  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_3, LL_DMA_PERIPH_NOINCREMENT);
  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_3, LL_DMA_MEMORY_INCREMENT);
  LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_3, LL_DMA_PDATAALIGN_BYTE);
  LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_3, LL_DMA_MDATAALIGN_BYTE);
  LL_DMA_DisableFifoMode(DMA1, LL_DMA_STREAM_3);
  LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_3, LL_USART_DMA_GetRegAddr(USART2, LL_USART_DMA_REG_DATA_TRANSMIT));

  /* Enable DMA RX HT & TC interrupts */
  LL_DMA_EnableIT_HT(DMA1, LL_DMA_STREAM_2);
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_2);
  /* Enable DMA TX TC interrupts */
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_3);

  /* DMA1_Stream2_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Stream2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(DMA1_Stream2_IRQn);
  /* DMA1_Stream3_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Stream3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(DMA1_Stream3_IRQn);


  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
  USART_InitStruct.BaudRate = 1497600;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART2, &USART_InitStruct);
  LL_USART_SetTXFIFOThreshold(USART2, LL_USART_FIFOTHRESHOLD_7_8);
  LL_USART_SetRXFIFOThreshold(USART2, LL_USART_FIFOTHRESHOLD_7_8);
  LL_USART_EnableFIFO(USART2);
  LL_USART_ConfigAsyncMode(USART2);
  LL_USART_EnableDMAReq_RX(USART2);
  LL_USART_EnableDMAReq_TX(USART2);
  LL_USART_EnableIT_IDLE(USART2);

  /* USER CODE BEGIN WKUPType USART2 */

  NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(USART2_IRQn);

  /* USER CODE END WKUPType USART2 */

  /* Enable USART and DMA RX */
  LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_2);
  LL_USART_Enable(USART2);

  /* Polling USART2 initialisation */
  while((!(LL_USART_IsActiveFlag_TEACK(USART2))) || (!(LL_USART_IsActiveFlag_REACK(USART2))))
  {
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* Initialise USART2 ringbuff for TX & RX */
  lwrb_init(&usart2_DMA_buffers.usart_tx_rb, usart2_DMA_buffers.usart_tx_rb_data, sizeof(usart2_DMA_buffers.usart_tx_rb_data));
  lwrb_init(&usart2_DMA_buffers.usart_rx_rb, usart2_DMA_buffers.usart_rx_rb_data, sizeof(usart2_DMA_buffers.usart_rx_rb_data));

  /* USER CODE END USART2_Init 2 */

}
/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}
/* USART6 init function */

void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART6;
  PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART6);

  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOC);
  /**USART6 GPIO Configuration
  PC6   ------> USART6_TX
  PC7   ------> USART6_RX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_6|LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USART6 DMA Init */

  /* USART6_RX Init */
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_STREAM_0, LL_DMAMUX1_REQ_USART6_RX);
  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_0, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
  LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_0, LL_DMA_PRIORITY_LOW);
  LL_DMA_SetMode(DMA1, LL_DMA_STREAM_0, LL_DMA_MODE_CIRCULAR);
  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_0, LL_DMA_PERIPH_NOINCREMENT);
  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_0, LL_DMA_MEMORY_INCREMENT);
  LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_0, LL_DMA_PDATAALIGN_BYTE);
  LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_0, LL_DMA_MDATAALIGN_BYTE);
  LL_DMA_DisableFifoMode(DMA1, LL_DMA_STREAM_0);
  LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_0, LL_USART_DMA_GetRegAddr(USART6, LL_USART_DMA_REG_DATA_RECEIVE));
  LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_0, (uint32_t)usart6_DMA_buffers.usart_rx_dma_buffer);
  LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_0, ARRAY_LEN(usart6_DMA_buffers.usart_rx_dma_buffer));

  /* USART6_TX Init */
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_STREAM_1, LL_DMAMUX1_REQ_USART6_TX);
  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_1, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
  LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_1, LL_DMA_PRIORITY_LOW);
  LL_DMA_SetMode(DMA1, LL_DMA_STREAM_1, LL_DMA_MODE_NORMAL);
  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_1, LL_DMA_PERIPH_NOINCREMENT);
  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_1, LL_DMA_MEMORY_INCREMENT);
  LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_1, LL_DMA_PDATAALIGN_BYTE);
  LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_1, LL_DMA_MDATAALIGN_BYTE);
  LL_DMA_DisableFifoMode(DMA1, LL_DMA_STREAM_1);
  LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_1, LL_USART_DMA_GetRegAddr(USART6, LL_USART_DMA_REG_DATA_TRANSMIT));

  /* Enable DMA RX HT & TC interrupts */
  LL_DMA_EnableIT_HT(DMA1, LL_DMA_STREAM_0);
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_0);
  /* Enable DMA TX TC interrupts */
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_1);

  /* DMA interrupt init */
  NVIC_SetPriority(DMA1_Stream0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream1_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Stream1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(DMA1_Stream1_IRQn);

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
  USART_InitStruct.BaudRate = 115200;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART6, &USART_InitStruct);
  LL_USART_SetTXFIFOThreshold(USART6, LL_USART_FIFOTHRESHOLD_7_8);
  LL_USART_SetRXFIFOThreshold(USART6, LL_USART_FIFOTHRESHOLD_7_8);
  LL_USART_EnableFIFO(USART6);
  LL_USART_ConfigAsyncMode(USART6);
  LL_USART_EnableDMAReq_RX(USART6);
  LL_USART_EnableDMAReq_TX(USART6);
  LL_USART_EnableIT_IDLE(USART6);

  /* USER CODE BEGIN WKUPType USART6 */

  /* USART6 interrupt Init */
  NVIC_SetPriority(USART6_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(USART6_IRQn);

  /* USER CODE END WKUPType USART6 */

  /* Enable USART and DMA RX */
  LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_0);
  LL_USART_Enable(USART6);

  /* Polling USART6 initialisation */
  while((!(LL_USART_IsActiveFlag_TEACK(USART6))) || (!(LL_USART_IsActiveFlag_REACK(USART6))))
  {
  }

  /* USER CODE BEGIN USART6_Init 2 */

  /* Initialise USART6 ringbuff for TX & RX */
  lwrb_init(&usart6_DMA_buffers.usart_tx_rb, usart6_DMA_buffers.usart_tx_rb_data, sizeof(usart6_DMA_buffers.usart_tx_rb_data));
  lwrb_init(&usart6_DMA_buffers.usart_rx_rb, usart6_DMA_buffers.usart_rx_rb_data, sizeof(usart6_DMA_buffers.usart_rx_rb_data));


  /* USER CODE END USART6_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3;
    PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PD8     ------> USART3_TX
    PD9     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = STLINK_RX_Pin|STLINK_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PD8     ------> USART3_TX
    PD9     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOD, STLINK_RX_Pin|STLINK_TX_Pin);

  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/**
 * \brief           Check for new data received with DMA
 *
 * User must select context to call this function from:
 * - Only interrupts (DMA HT, DMA TC, UART IDLE) with same preemption priority level
 * - Only thread context (outside interrupts)
 *
 * If called from both context-es, exclusive access protection must be implemented
 * This mode is not advised as it usually means architecture design problems
 *
 * When IDLE interrupt is not present, application must rely only on thread context,
 * by manually calling function as quickly as possible, to make sure
 * data are read from raw buffer and processed.
 *
 * Not doing reads fast enough may cause DMA to overflow unread received bytes,
 * hence application will lost useful data.
 *
 * Solutions to this are:
 * - Improve architecture design to achieve faster reads
 * - Increase raw buffer size and allow DMA to write more data before this function is called
 *
 * \param[in]       DMAx: DMAx Instance (e.g. DMA<1..2>)
 * \param[in]       ll_dma_stream: DMA stream index (e.g. LL_DMA_STREAM_<0..7>)
 * \param[in]       usart_rx_dma_buffer: USART RX DMA buffer
 * \param[in]       buffer_len: USART RX DMA buffer array length
 * \param[in]       usart_rx_rb: Ring buffer instance for RX data
 */
static void usart_rx_check(DMA_TypeDef* DMAx,uint32_t ll_dma_stream, uint8_t* usart_rx_dma_buffer, uint32_t buffer_len, lwrb_t* usart_rx_rb)
{
    static size_t old_pos;
    size_t pos;

    /* Calculate current position in buffer and check for new data available */
    pos = buffer_len - LL_DMA_GetDataLength(DMAx, ll_dma_stream);
    if (pos != old_pos)
    {                       /* Check change in received data */
        if (pos > old_pos)
        {                    /* Current position is over previous one */
            /*
             * Processing is done in "linear" mode.
             *
             * Application processing is fast with single data block,
             * length is simply calculated by subtracting pointers
             *
             * [   0   ]
             * [   1   ] <- old_pos |------------------------------------|
             * [   2   ]            |                                    |
             * [   3   ]            | Single block (len = pos - old_pos) |
             * [   4   ]            |                                    |
             * [   5   ]            |------------------------------------|
             * [   6   ] <- pos
             * [   7   ]
             * [ N - 1 ]
             */
            SCB_CleanInvalidateDCache_by_Addr((uint32_t*)&usart_rx_dma_buffer[old_pos],pos - old_pos);
            lwrb_write(usart_rx_rb, &usart_rx_dma_buffer[old_pos], pos - old_pos);
        }
        else
        {
            /*
             * Processing is done in "overflow" mode..
             *
             * Application must process data twice,
             * since there are 2 linear memory blocks to handle
             *
             * [   0   ]            |---------------------------------|
             * [   1   ]            | Second block (len = pos)        |
             * [   2   ]            |---------------------------------|
             * [   3   ] <- pos
             * [   4   ] <- old_pos |---------------------------------|
             * [   5   ]            |                                 |
             * [   6   ]            | First block (len = N - old_pos) |
             * [   7   ]            |                                 |
             * [ N - 1 ]            |---------------------------------|
             */
            SCB_CleanInvalidateDCache_by_Addr((uint32_t*)&usart_rx_dma_buffer[old_pos],buffer_len - old_pos);
            lwrb_write(usart_rx_rb, &usart_rx_dma_buffer[old_pos], buffer_len - old_pos);
            if (pos > 0)
            {
                SCB_CleanInvalidateDCache_by_Addr((uint32_t*)&usart_rx_dma_buffer[0],pos);
                lwrb_write(usart_rx_rb, &usart_rx_dma_buffer[0], pos);
            }
        }
        old_pos = pos;                          /* Save current position as old for next transfers */
    }
}

/**
 * \brief           Check for new data received with DMA on USART2
 *                  For more detail please see usart_rx_check()
 */
void usart2_rx_check(void)
{
    usart_rx_check(DMA1, LL_DMA_STREAM_2, usart2_DMA_buffers.usart_rx_dma_buffer, RX_DMA_BUFFER_LEN, &usart2_DMA_buffers.usart_rx_rb);
}

/**
 * \brief           Check for new data received with DMA on USART6
 *                  For more detail please see usart_rx_check()
 */
void usart6_rx_check(void)
{
    usart_rx_check(DMA1, LL_DMA_STREAM_0, usart6_DMA_buffers.usart_rx_dma_buffer, RX_DMA_BUFFER_LEN, &usart6_DMA_buffers.usart_rx_rb);
}

/**
 * \brief           Clears the DMA TX stream flags for a specific USART using LL drivers
 *
 * \param[in]       DMAx: DMAx Instance (e.g. DMA<1..2>)
 * \param[in]       ll_dma_stream: DMA stream index (e.g. LL_DMA_STREAM_<0..7>)
 */
void usart_ll_dma_clear_flags(DMA_TypeDef* DMAx,uint32_t ll_dma_stream)
{
    switch(ll_dma_stream)
    {
      /* USART6 TX DMA stream */
      case LL_DMA_STREAM_1:

        LL_DMA_ClearFlag_TC1(DMAx);
        LL_DMA_ClearFlag_HT1(DMAx);
        LL_DMA_ClearFlag_TE1(DMAx);
        LL_DMA_ClearFlag_DME1(DMAx);
        LL_DMA_ClearFlag_FE1(DMAx);
        break;

      /* USART2 TX DMA stream */
      case LL_DMA_STREAM_3:

        LL_DMA_ClearFlag_TC3(DMAx);
        LL_DMA_ClearFlag_HT3(DMAx);
        LL_DMA_ClearFlag_TE3(DMAx);
        LL_DMA_ClearFlag_DME3(DMAx);
        LL_DMA_ClearFlag_FE3(DMAx);
        break;

      default:
         break;
    }
}

/**
 * \brief           Check if DMA is active and if not try to send data
 *
 * This function can be called either by application to start data transfer
 * or from DMA TX interrupt after previous transfer just finished
 *
 * \param[in]       DMAx: DMAx Instance (e.g. DMA<1..2>)
 * \param[in]       ll_dma_stream: DMA stream index (e.g. LL_DMA_STREAM_<0..7>)
 * \param[in]       usart_tx_dma_current_len: Length of currently active TX DMA transfer
 * \param[in]       usart_rx_rb: Ring buffer instance for RX data
 *
 * \return          `1` if transfer just started, `0` if on-going or no data to transmit
 */
static uint8_t usart_start_tx_dma_transfer(DMA_TypeDef* DMAx, uint32_t ll_dma_stream, volatile size_t* usart_tx_dma_current_len, lwrb_t* usart_tx_rb)
{
    uint32_t primask;
    uint8_t started = 0;

    /*
     * First check if transfer is currently in-active,
     * by examining the value of usart_tx_dma_current_len variable.
     *
     * This variable is set before DMA transfer is started and cleared in DMA TX complete interrupt.
     *
     * It is not necessary to disable the interrupts before checking the variable:
     *
     * When usart_tx_dma_current_len == 0
     *    - This function is called by either application or TX DMA interrupt
     *    - When called from interrupt, it was just reset before the call,
     *         indicating transfer just completed and ready for more
     *    - When called from an application, transfer was previously already in-active
     *         and immediate call from interrupt cannot happen at this moment
     *
     * When usart_tx_dma_current_len != 0
     *    - This function is called only by an application.
     *    - It will never be called from interrupt with usart_tx_dma_current_len != 0 condition
     *
     * Disabling interrupts before checking for next transfer is advised
     * only if multiple operating system threads can access to this function w/o
     * exclusive access protection (mutex) configured,
     * or if application calls this function from multiple interrupts.
     *
     * This example assumes worst use case scenario,
     * hence interrupts are disabled prior every check
     */
    // ORIGINAL: primask = __get_PRIMASK();
    // ORIGINAL: __disable_irq();
    primask = tx_interrupt_control(TX_INT_DISABLE);
    if (*usart_tx_dma_current_len == 0
            && (*usart_tx_dma_current_len = lwrb_get_linear_block_read_length(usart_tx_rb)) > 0)
    {
        /* Disable channel if enabled */
        LL_DMA_DisableStream(DMAx, ll_dma_stream);

        /* Clear all flags */
        usart_ll_dma_clear_flags(DMAx, ll_dma_stream);

        /* Prepare DMA data and length */
        SCB_CleanDCache_by_Addr((uint32_t*)lwrb_get_linear_block_read_address(usart_tx_rb), *usart_tx_dma_current_len);
        LL_DMA_SetDataLength(DMAx, ll_dma_stream, *usart_tx_dma_current_len);
        LL_DMA_SetMemoryAddress(DMAx, ll_dma_stream, (uint32_t)lwrb_get_linear_block_read_address(usart_tx_rb));

        /* Start transfer */
        LL_DMA_EnableStream(DMAx, ll_dma_stream);
        started = 1;
    }
    // ORIGINAL: __set_PRIMASK(primask);
    tx_interrupt_control((UINT)primask);
    return started;
}

/**
 * \brief           Check if USART2 DMA is active and if not try to send data
 *                  Please check usart_start_tx_dma_transfer() for more details
 *
 * \return          `1` if transfer just started, `0` if on-going or no data to transmit
 */
uint8_t usart2_start_tx_dma_transfer(void)
{
    return usart_start_tx_dma_transfer(DMA1, LL_DMA_STREAM_3, &usart2_DMA_buffers.usart_tx_dma_current_len, &usart2_DMA_buffers.usart_tx_rb);
}

/**
 * \brief           Check if USART6 DMA is active and if not try to send data
 *                  Please check usart_start_tx_dma_transfer() for more details
 *
 * \return          `1` if transfer just started, `0` if on-going or no data to transmit
 */
uint8_t usart6_start_tx_dma_transfer(void)
{
    return usart_start_tx_dma_transfer(DMA1, LL_DMA_STREAM_1, &usart6_DMA_buffers.usart_tx_dma_current_len, &usart6_DMA_buffers.usart_tx_rb);
}

/**
 * \brief           Process received data over USART2
 * Data are written to RX ringbuffer for application processing at latter stage
 * \param[in]       data: Data to process
 * \param[in]       len: Length in units of bytes
 */
void usart2_process_data(const void* data, size_t len)
{
    lwrb_write(&usart2_DMA_buffers.usart_rx_rb, data, len);  /* Write data to receive buffer */
}

/**
 * \brief           Process received data over USART6
 * Data are written to RX ringbuffer for application processing at latter stage
 * \param[in]       data: Data to process
 * \param[in]       len: Length in units of bytes
 */
void usart6_process_data(const void* data, size_t len)
{
    lwrb_write(&usart6_DMA_buffers.usart_rx_rb, data, len);  /* Write data to receive buffer */
}

/**
 * \brief           Send data over USART2
 * \param[in]       byte: Pointer to data to write into buffer
 * \param[in]       str: Number of bytes to write
 */
void usart2_send(const uint8_t* byte, size_t len)
{
    lwrb_write(&usart2_DMA_buffers.usart_tx_rb, byte, len);   /* Write data to transmit buffer */
    usart_start_tx_dma_transfer(DMA1, LL_DMA_STREAM_3, &usart2_DMA_buffers.usart_tx_dma_current_len, &usart2_DMA_buffers.usart_tx_rb);
}

/**
 * \brief           Read data from UART2 circular buffer.
 *                  Copies data from buffer to `data` for maximum `len` number of bytes
 *
 * \param[out]      data: Pointer to output memory to copy buffer data to
 * \param[in]       len: Number of bytes to read
 * \return          Number of bytes read and copied to data array
 */
size_t usart2_recv(void* data, size_t len)
{
    return lwrb_read(&usart2_DMA_buffers.usart_rx_rb, data, len);
}


/**
 * \brief           Send data over USART6
 * \param[in]       byte: Pointer to data to write into buffer
 * \param[in]       str: Number of bytes to write
 */
void usart6_send(const uint8_t* byte, size_t len)
{
    lwrb_write(&usart6_DMA_buffers.usart_tx_rb, byte, len);   /* Write data to transmit buffer */
    usart_start_tx_dma_transfer(DMA1, LL_DMA_STREAM_1, &usart6_DMA_buffers.usart_tx_dma_current_len, &usart6_DMA_buffers.usart_tx_rb);
}

/**
 * \brief           Read data from UART6 circular buffer.
 *                  Copies data from buffer to `data` for maximum `len` number of bytes
 *
 * \param[out]      data: Pointer to output memory to copy buffer data to
 * \param[in]       len: Number of bytes to read
 * \return          Number of bytes read and copied to data array
 */
size_t usart6_recv(void* data, size_t len)
{
    return lwrb_read(&usart6_DMA_buffers.usart_rx_rb, data, len);
}


/* USER CODE END 1 */

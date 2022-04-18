/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32ide_main.h"
#include "string.h"
#include "lwrb/lwrb.h"
/* USER CODE END Includes */

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;

/* USER CODE BEGIN Private defines */

#if defined ( __GNUC__) && !defined(__clang__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

/**
 * \brief           USART DMA buffers length setup
 */
#define RX_DMA_BUFFER_LEN        64U
#if((RX_DMA_BUFFER_LEN & 0x20) != 0)
  #error "RX_DMA_BUFFER_LEN should be a Multiple of cache line size (32 bytes)"
#endif

#define RX_RING_BUFFER_DATA_LEN  128U
#if((RX_RING_BUFFER_DATA_LEN & 0x20) != 0)
  #error "RX_RING_BUFFER_DATA_LEN should be a Multiple of cache line size (32 bytes)"
#endif

#define TX_RING_BUFFER_DATA_LEN  128U
#if((TX_RING_BUFFER_DATA_LEN & 0x20) != 0)
  #error "TX_RING_BUFFER_DATA_LEN should be a Multiple of cache line size (32 bytes)"
#endif

/**
 * \brief           Calculate length of statically allocated array
 */
#define ARRAY_LEN(x)            (sizeof(x) / sizeof((x)[0]))

/* USER CODE END Private defines */

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
typedef struct usart_dma_buffers_tag
{
    /**
     * \brief           Buffer used for RX DMA
     *                  Size should be a Multiple of cache line size (32 bytes)
     */
    ALIGN_32BYTES (uint8_t usart_rx_dma_buffer[RX_DMA_BUFFER_LEN]);

    /**
     * \brief           Ring buffer instance for RX data
     */
    lwrb_t usart_rx_rb;

    /**
     * \brief           Ring buffer data array for RX DMA
     *                  Size should be a Multiple of cache line size (32 bytes)
     */
    ALIGN_32BYTES (uint8_t usart_rx_rb_data[RX_RING_BUFFER_DATA_LEN]);

    /**
     * \brief           Ring buffer instance for TX data
     */
    lwrb_t usart_tx_rb;

    /**
     * \brief           Ring buffer data array for TX DMA
     *                  Size should be a Multiple of cache line size (32 bytes)
     */
    ALIGN_32BYTES (uint8_t usart_tx_rb_data[TX_RING_BUFFER_DATA_LEN]);

    /**
     * \brief           Length of currently active TX DMA transfer
     */
    volatile size_t usart_tx_dma_current_len;
} usart_dma_buffers_t;


extern usart_dma_buffers_t usart2_DMA_buffers;
extern usart_dma_buffers_t usart6_DMA_buffers;

/* USER CODE BEGIN Prototypes */

void usart2_rx_check();
void usart2_send(const uint8_t* byte, size_t len);
size_t usart2_recv(void* data, size_t len);
void usart2_process_data(const void* data, size_t len);
uint8_t usart2_start_tx_dma_transfer(void);

void usart6_rx_check();
void usart6_send(const uint8_t* byte, size_t len);
size_t usart6_recv(void* data, size_t len);
void usart6_process_data(const void* data, size_t len);
uint8_t usart6_start_tx_dma_transfer(void);

void MX_USART2_UART_Init(void);
void MX_USART3_UART_Init(void);
void MX_USART6_UART_Init(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_filex.c
  * @author  MCD Application Team
  * @brief   FileX applicative file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020-2021 STMicroelectronics.
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
#include "app_filex.h"
#include <stdbool.h>

#ifdef FX_ENABLE_FAULT_TOLERANT
#include "fx_fault_tolerant.h"
#endif /* FX_ENABLE_FAULT_TOLERANT */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define FILEX_DEFAULT_STACK_SIZE         (1024 * 24)
/* fx_sd_thread priority */
#define DEFAULT_THREAD_PRIO              10
/* fx_sd_thread preemption priority */
#define DEFAULT_PREEMPTION_THRESHOLD     DEFAULT_THREAD_PRIO

#define FILEX_MEDIA_MEMORY_SIZE          512

#ifdef FX_ENABLE_FAULT_TOLERANT
UCHAR         fault_tolerant_memory[FX_FAULT_TOLERANT_MAXIMUM_LOG_FILE_SIZE];
#endif /* FX_ENABLE_FAULT_TOLERANT */

// fx_ls constants
#define LINES 12
#define CHARS 128

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE BEGIN SRAMDiskSection */

/* Buffer for FileX FX_MEDIA sector cache. this should be 32-Bytes
aligned to avoid cache maintenance issues */
ALIGN_32BYTES(uint32_t media_memory[FILEX_MEDIA_MEMORY_SIZE / sizeof(uint32_t)]);

/* Define FileX global data structures.  */
FX_MEDIA        sram_disk;
FX_FILE         fx_file;

/* Define ThreadX global data structures.  */
TX_THREAD       fx_thread;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void fx_thread_entry(ULONG thread_input);
void Error_Handler(void);

/* USER CODE END PFP */

/**
  * @brief  FileX initialisation.
  * @param none
  * @retval Initialisation status
  */
UINT filex_initialise(void)
{
  UINT status;

  /* USER CODE BEGIN MX_FileX_Init */

  /* Format the SRAM2 disk memory as FAT */
  status =  fx_media_format(&sram_disk,                   // RamDisk pointer
                            fx_stm32_sram_driver,         // Driver entry
                            (VOID *)FX_NULL,              // Device info pointer
                            (UCHAR *) media_memory,       // Media buffer pointer
                            FILEX_MEDIA_MEMORY_SIZE,      // Media buffer size
                            "SRAM_DISK",                  // Volume Name
                            1,                            // Number of FATs
                            32,                           // Directory Entries
                            0,                            // Hidden sectors
                            FX_SRAM_DISK_SIZE / FILEX_MEDIA_MEMORY_SIZE,      // Total sectors
                            FILEX_MEDIA_MEMORY_SIZE,      // Sector size
                            8,                            // Sectors per cluster
                            1,                            // Heads
                            1);                           // Sectors per track

  /* Check the format status */
  if (status == FX_SUCCESS)
  {

#ifdef FX_ENABLE_FAULT_TOLERANT

    status =  fx_fault_tolerant_enable(&sram_disk, fault_tolerant_memory, sizeof(fault_tolerant_memory));

    /* Check the media open status. */
    if (status == FX_SUCCESS)
    {
      printf("Fault tolerance enabled\n");
    }
    else
    {
        printf("Fault tolerance enabling failed\n");
        assert(0);
    }
#endif

    /* Open the sram_disk driver. */
    status =  fx_media_open(&sram_disk, "SRAM_DISK", fx_stm32_sram_driver, (VOID *)FX_NULL, (VOID *) media_memory, sizeof(media_memory));

    /* Check the media open status. */
    if (status == FX_SUCCESS)
    {
      printf("SRAM Disk successfully formatted and opened.\n");
    }
    else
    {
        assert(0);
    }

    status = fx_directory_create(&sram_disk, FILEX_MEDIA_DEFAULT_DIRECTORY);

    if (FX_SUCCESS == status)
    {
        // NOTE: A default directory must be set to enable successful directory
        // operations on "." and ".."
        status = fx_directory_default_set(&sram_disk, FILEX_MEDIA_DEFAULT_DIRECTORY);

        if (FX_SUCCESS != status)
        {
            printf("Error 0x%.2x when setting the default path [%s]\n", status, FILEX_MEDIA_DEFAULT_DIRECTORY);
        }
    }
    else
    {
        printf("Error 0x%.2x when creating the default path [%s]\n", status, FILEX_MEDIA_DEFAULT_DIRECTORY);
    }
  }

  return status;
}

/**
  * @brief  Application FileX Initialisation.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT MX_FileX_Init(VOID *memory_ptr)
{
  UINT ret = FX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

  /* USER CODE BEGIN MX_FileX_MEM_POOL */
  /* USER CODE END MX_FileX_MEM_POOL */

  /* USER CODE BEGIN MX_FileX_Init */
 CHAR *pointer;

  /*Allocate memory for fx_thread_entry*/
  ret = tx_byte_allocate(byte_pool, (VOID **) &pointer, FILEX_DEFAULT_STACK_SIZE, TX_NO_WAIT);

  /* Check FILEX_DEFAULT_STACK_SIZE allocation*/
  if (ret != FX_SUCCESS)
  {
    Error_Handler();
  }

  /* Create the main thread.  */
  ret = tx_thread_create(&fx_thread, "fx_sd_thread", fx_thread_entry, 0, pointer, FILEX_DEFAULT_STACK_SIZE, DEFAULT_THREAD_PRIO,
                         DEFAULT_PREEMPTION_THRESHOLD, TX_NO_TIME_SLICE, TX_AUTO_START);

  /* Check main thread creation */
  if (ret != FX_SUCCESS)
  {
    Error_Handler();
  }

  return ret;
}

/**
  * @brief Function used to retrieve a media control block for a specific
  *        FS disk memory.
  * @param Reference to a media control block
  * @retval none
  */
void file_sys_media_get(FX_MEDIA** mediaPtr)
{
    assert(mediaPtr);
    *mediaPtr = &sram_disk;
}

/* USER CODE BEGIN 1 */
void fx_thread_entry(ULONG thread_input)
{
  UINT status;
  ULONG bytes_read;
  CHAR read_buffer[32];
  CHAR data[] = "This is FileX working on STM32";

  /* Start application */
   printf("FileX SRAM Application Start.\n");

  // FileX initialisation
  if (filex_initialise() != FX_SUCCESS)
  {
    Error_Handler();
  }

  /* Create a file called STM32.TXT in the root directory.  */
  status =  fx_file_create(&sram_disk, "STM32.TXT");

  /* Check the create status.  */
  if (status != FX_SUCCESS)
  {
    /* Check for an already created status. This is expected on the
    second pass of this loop!  */
    if (status != FX_ALREADY_CREATED)
    {
      /* Create error, call error handler.  */
      Error_Handler();
    }
  }

  /* Open the test file.  */
  status =  fx_file_open(&sram_disk, &fx_file, "STM32.TXT", FX_OPEN_FOR_WRITE);

  /* Check the file open status.  */
  if (status != FX_SUCCESS)
  {
    /* Error opening file, call error handler.  */
    Error_Handler();
  }

  /* Seek to the beginning of the test file.  */
  status =  fx_file_seek(&fx_file, 0);

  /* Check the file seek status.  */
  if (status != FX_SUCCESS)
  {
    /* Error performing file seek, call error handler.  */
    Error_Handler();
  }

  printf("Writing data into the file. \n");

  /* Write a string to the test file.  */
  status =  fx_file_write(&fx_file, data, sizeof(data));

  /* Check the file write status.  */
  if (status != FX_SUCCESS)
  {
    /* Error writing to a file, call error handler.  */
    Error_Handler();
  }

  /* Close the test file.  */
  status =  fx_file_close(&fx_file);

  /* Check the file close status.  */
  if (status != FX_SUCCESS)
  {
    /* Error closing the file, call error handler.  */
    Error_Handler();
  }

  status = fx_media_flush(&sram_disk);

  /* Check the media flush  status.  */
  if (status != FX_SUCCESS)
  {
    /* Error closing the file, call error handler.  */
    Error_Handler();
  }

  /* Open the test file.  */
  status =  fx_file_open(&sram_disk, &fx_file, "STM32.TXT", FX_OPEN_FOR_READ);

  /* Check the file open status.  */
  if (status != FX_SUCCESS)
  {
    /* Error opening file, call error handler.  */
    Error_Handler();
  }

  /* Seek to the beginning of the test file.  */
  status =  fx_file_seek(&fx_file, 0);

  /* Check the file seek status.  */
  if (status != FX_SUCCESS)
  {
    /* Error performing file seek, call error handler.  */
    Error_Handler();
  }

  /* Read the first 28 bytes of the test file.  */
  status =  fx_file_read(&fx_file, read_buffer, sizeof(data), &bytes_read);

  /* Check the file read status.  */
  if ((status != FX_SUCCESS) || (bytes_read != sizeof(data)))
  {
    /* Error reading file, call error handler.  */
    Error_Handler();
  }

  /* Close the test file.  */
  status =  fx_file_close(&fx_file);

  /* Check the file close status. */
  if (status != FX_SUCCESS)
  {
    /* Error closing the file, call error handler. */
    Error_Handler();
  }

  /* Close the media.  */
  status =  fx_media_close(&sram_disk);

  /* Check the media close status.  */
  if (status != FX_SUCCESS)
  {
    /* Error closing the media, call error handler.  */
    Error_Handler();
  }

  printf("Data successfully written.\n");

  /* Infinite loop */
  while (1)
  {
    tx_thread_relinquish();
  }
}

/**
  * @brief  Prints the file/dir list in the storage media
  * @param  None
  * @retval None
  */
void fx_ls(void)
{
    UINT status;
    CHAR file_name[LINES][CHARS];

    // Get default directory
    CHAR* defaultDir;
    if (FX_SUCCESS != fx_directory_default_get(&sram_disk, &defaultDir))
    {
        assert(0);
    }
    printf("Default directory: [%s]\n", defaultDir);

    // Get local directory
    CHAR* localDir;
    if (FX_SUCCESS != fx_directory_local_path_get(&sram_disk, &localDir))
    {
        assert(0);
    }

    if (localDir != NULL)
    {
        printf("fprime: ls %s\n",localDir);
    }
    else
    {
        printf("fprime: [Local directory not set]\n");
    }

    int ii = 0;
    status =  fx_directory_first_entry_find(&sram_disk, file_name[ii]);
    while (status == FX_SUCCESS)
    {
        // Print an entry and tehn find next one
        printf("%s\n", file_name[ii++]);
        if (ii >= LINES) break;
        status =  fx_directory_next_entry_find(&sram_disk, file_name[ii]);

    }
}
//»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
/* USER CODE END 1 */

#include <Os/Queue.hpp>
#include <Fw/Types/Assert.hpp>

#include <stdio.h>
#include <string.h>
//#include <logLib.h>

#include "app_azure_rtos.h"

// Type used to piggyback on class member "m_handle" all required data to
// operate a Queue object without the need to add extra members in Queue.hpp
typedef struct queueHandleDataTag
{
    TX_QUEUE handle;           // handle for implementation specific queue on Azure ThreadX
    ULONG* queueStart;         // pointer to the queue storage area
    NATIVE_INT_TYPE msgSize;   // message size (maximum message size queue can hold)
    NATIVE_INT_TYPE depth;     // queue depth (maximum number of messages queue can hold)
    NATIVE_UINT_TYPE count;    // Current number of messages on the queue
    NATIVE_UINT_TYPE maxCount; // Maximum number of messages ever seen on the queue
} queueHandleData_t;

//NOTE: ThreadX queue message size ranges from 1*4 bytes to 16*4 bytes (4 bytes
//      to 64 byte maximum length)
//
// ThreadX user manual for tx_queue_create(.., message_size, .. ) states:
//      message_size Specifies the size of each message in the queue. Message
//      sizes range from 1 32-bit word to 16 32-bit words. Valid message size
//      options are numerical values from 1 through 16,inclusive.
#define THREADX_Q_WORD_MAX_RANGE                  16
#define THREADX_Q_WORD_BYTE_NUM                   4

#define THREADX_MSG_SIZE_MAX_BYTES                THREADX_Q_WORD_BYTE_NUM * THREADX_Q_WORD_MAX_RANGE
#define THREADX_MSG_SIZE_MIN_BYTES                1

#define THREADX_MSG_SIZE_RANGE_LUT_ENTRIES        THREADX_MSG_SIZE_MAX_BYTES + 1

// Size word index in the RX and TX buffers
#define THREADX_Q_WORD_SIZE_INDEX                 0

// ThreadX queue message size range lookup table
//      input:  F' queue message size
//      output: ThreadX word message size range
static const UINT rangeLUT[THREADX_MSG_SIZE_RANGE_LUT_ENTRIES] =
                  {0,  1, 1, 1, 1,  //  1  up to  4 byte
                       2, 2, 2, 2,  //  5  up to  8 byte
                       3, 3, 3, 3,  //  9  up to 12 byte
                       4, 4, 4, 4,  // 13  up to 16 byte
                       5, 5, 5, 5,  // 17  up to 20 byte
                       6, 6, 6, 6,  // 21  up to 24 byte
                       7, 7, 7, 7,  // 25  up to 28 byte
                       8, 8, 8, 8,  // 29  up to 32 byte
                       9, 9, 9, 9,  // 33  up to 36 byte
                      10,10,10,10,  // 37  up to 40 byte
                      11,11,11,11,  // 41  up to 44 byte
                      12,12,12,12,  // 44  up to 48 byte
                      13,13,13,13,  // 49  up to 52 byte
                      14,14,14,14,  // 53  up to 56 byte
                      15,15,15,15,  // 57  up to 60 byte
                      16,16,16,16   // 61  up to 64 byte
                  };

namespace Os {

    Queue::Queue(): m_handle() {

    }

    Queue::QueueStatus Queue::createInternal(const Fw::StringBase &name, NATIVE_INT_TYPE depth, NATIVE_INT_TYPE msgSize) {

        // Assert if the message size exceeds the ThreadX supported size -THREADX_Q_WORD_BYTE_NUM
        //     NOTE: 1 word must be reserved for the message size because ThreadX
        //           does not implement a send method with request to send bytes parameter
        FW_ASSERT(msgSize <= (THREADX_MSG_SIZE_MAX_BYTES - THREADX_Q_WORD_BYTE_NUM));

        // Assert if the message size is less than 1 byte
        FW_ASSERT(msgSize >= THREADX_MSG_SIZE_MIN_BYTES);

        this->m_name = "QV_";
        this->m_name += name;

        QueueStatus queueStatus = QUEUE_OK;
        UINT ret = TX_SUCCESS;

        // ThreadX memory pool pointer
        TX_BYTE_POOL* bytePoolPtr;

        // Get ThreadX memory pool
        ret = app_tx_get_byte_pool(&bytePoolPtr);

        if(ret == TX_SUCCESS)
        {
            // Allocate the memory for auxiliary queue data handling structure
            if (tx_byte_allocate(bytePoolPtr, (VOID **)&this->m_handle, sizeof(queueHandleData_t), TX_NO_WAIT) != TX_SUCCESS)
            {
                ret = TX_POOL_ERROR;
            }
            else
            {
                // Queue storage pointer (including extra memory word for the message size)
                ULONG txQueueByteDepth = ((1 + rangeLUT[msgSize]) * THREADX_Q_WORD_BYTE_NUM) * depth;


                // Allocate the memory for the queue
                if (tx_byte_allocate(bytePoolPtr, (VOID **)&(reinterpret_cast<queueHandleData_t*>(this->m_handle)->queueStart), txQueueByteDepth, TX_NO_WAIT) != TX_SUCCESS)
                {
                    ret = TX_POOL_ERROR;
                }
                else
                {
                    // The queue message size is calculated based on the client
                    // message byte size + extra payload size word(THREADX_Q_WORD_BYTE_NUM)
                    ret = tx_queue_create(reinterpret_cast<TX_QUEUE*>(this->m_handle),
                                          (char*)this->m_name.toChar(),
                                          rangeLUT[msgSize + THREADX_Q_WORD_BYTE_NUM],
                                          reinterpret_cast<queueHandleData_t*>(this->m_handle)->queueStart,
                                          txQueueByteDepth);
                }
            }
        }

        // Check ThreadX return status
        switch (ret)
        {
            case TX_SUCCESS:
                reinterpret_cast<queueHandleData_t*>(this->m_handle)->depth   = depth;
                reinterpret_cast<queueHandleData_t*>(this->m_handle)->msgSize = msgSize;
                reinterpret_cast<queueHandleData_t*>(this->m_handle)->count    = 0;
                reinterpret_cast<queueHandleData_t*>(this->m_handle)->maxCount = 0;
                Queue::s_numQueues++;
                break;
            case TX_QUEUE_ERROR:
            case TX_PTR_ERROR:
            case TX_SIZE_ERROR:
            case TX_CALLER_ERROR:
            case TX_POOL_ERROR:
                printf("queue creation [ERROR][0x%.2x]\n", ret);
                queueStatus = QUEUE_UNINITIALIZED;
                break;
            default:
                queueStatus = QUEUE_UNKNOWN_ERROR;
                break;
        }

        return queueStatus;
    }

    Queue::~Queue() {

        (void)tx_queue_delete(reinterpret_cast<TX_QUEUE*>(this->m_handle));

        // Release queue's allocated pool memory
        UINT ret = tx_byte_release((VOID *)reinterpret_cast<queueHandleData_t*>(this->m_handle)->queueStart);
        // TODO: if ret != TX_SUCCESS log a memory leak

        // Release auxiliary handle allocated memory
        ret = tx_byte_release((VOID *)this->m_handle);
        // TODO: if ret != TX_SUCCESS log a memory leak
    }

    Queue::QueueStatus Queue::send(const U8* buffer, NATIVE_INT_TYPE size, NATIVE_INT_TYPE priority, QueueBlocking block) {

        QueueStatus queueStatus = QUEUE_OK;

        CHAR *name;
        ULONG enqueued;
        ULONG availableStorage;
        TX_THREAD *firstSuspended;
        ULONG suspendedCount;
        TX_QUEUE *nextQueue;

        if (TX_SUCCESS != tx_queue_info_get(reinterpret_cast<TX_QUEUE*>(this->m_handle), &name, &enqueued, &availableStorage, &firstSuspended, &suspendedCount,&nextQueue))
        {
            queueStatus = QUEUE_UNINITIALIZED;
        }
        else if (NULL == buffer)
        {
            queueStatus = QUEUE_EMPTY_BUFFER;
        }
        // Ensure that the application does not violate the reserved size word
        else if (size >= (THREADX_MSG_SIZE_MAX_BYTES - THREADX_Q_WORD_BYTE_NUM))
        {
            queueStatus = QUEUE_SEND_ERROR;
        }
        else
        {
            UINT ret = TX_SUCCESS;

            //pointer to the buffer used to TX the client's payload + payload size
            ULONG sendBuffer[THREADX_Q_WORD_MAX_RANGE];

            //Set the payload size in the size reserved word
            sendBuffer[THREADX_Q_WORD_SIZE_INDEX] = size;

            // Copy the payload to the next ULONG word
            memcpy(&sendBuffer[1], buffer, size);

            // Urgent priority message
            if (priority > 0)
            {
                ret = tx_queue_front_send(reinterpret_cast<TX_QUEUE*>(this->m_handle), (VOID*)sendBuffer, (QUEUE_NONBLOCKING == block)?TX_NO_WAIT:TX_WAIT_FOREVER);
            }
            // Normal priority message
            else
            {
                ret = tx_queue_send(reinterpret_cast<TX_QUEUE*>(this->m_handle), (VOID*)sendBuffer, (QUEUE_NONBLOCKING == block)?TX_NO_WAIT:TX_WAIT_FOREVER);
            }

            // Check ThreadX return status
            NATIVE_UINT_TYPE* count = &reinterpret_cast<queueHandleData_t*>(this->m_handle)->count;
            NATIVE_UINT_TYPE* maxCount = &reinterpret_cast<queueHandleData_t*>(this->m_handle)->maxCount;

            switch (ret)
            {
                case TX_SUCCESS:
                    // Increment count:
                    ++(*count);
                    if ( *count > *maxCount )
                    {
                      *maxCount = *count;
                    }
                    break;
                case TX_QUEUE_FULL:
                    queueStatus = QUEUE_FULL;
                    break;
                case TX_DELETED:
                case TX_QUEUE_ERROR:
                case TX_WAIT_ABORTED:
                case TX_PTR_ERROR:
                case TX_WAIT_ERROR:
                default:
                      // TODO: port logMsg
    //                logMsg("Queue send error %s! %d\n",(int)strerror(errno),(int)this->m_handle,0,0,0,0);
                    printf("Queue send [ERROR][0x%.2x]\n", ret);
                    queueStatus = QUEUE_UNKNOWN_ERROR;
                    break;
            }
        }
        return queueStatus;
    }

    Queue::QueueStatus Queue::receive(U8* buffer, NATIVE_INT_TYPE capacity, NATIVE_INT_TYPE &actualSize, NATIVE_INT_TYPE &priority, QueueBlocking block) {

        // ThreadX receive does not require a buffer capacity
        (void)capacity;

        // ThreadX receive does not support prioritised messages
        priority = 0;

        QueueStatus queueStatus = QUEUE_OK;

        CHAR *name;
        ULONG enqueued;
        ULONG availableStorage;
        TX_THREAD *firstSuspended;
        ULONG suspendedCount;
        TX_QUEUE *nextQueue;

        if (TX_SUCCESS != tx_queue_info_get(reinterpret_cast<TX_QUEUE*>(this->m_handle), &name, &enqueued, &availableStorage, &firstSuspended, &suspendedCount,&nextQueue))
        {
            queueStatus = QUEUE_UNINITIALIZED;
        }
        else
        {

            UINT ret = TX_SUCCESS;

            // Pointer to the buffer used to RX the client's payload + payload size
            ULONG recvBuffer[THREADX_Q_WORD_MAX_RANGE];

            ret = tx_queue_receive(reinterpret_cast<TX_QUEUE*>(this->m_handle), (VOID *)recvBuffer, (QUEUE_NONBLOCKING == block)?TX_NO_WAIT:TX_WAIT_FOREVER);

            // Check ThreadX return status
            NATIVE_UINT_TYPE* count = &reinterpret_cast<queueHandleData_t*>(this->m_handle)->count;

            switch (ret)
            {
                case TX_SUCCESS:
                    // Set the received payload size
                    actualSize = recvBuffer[THREADX_Q_WORD_SIZE_INDEX];
                    // Copy the payload from the next ULONG word
                    memcpy(buffer ,&recvBuffer[1], actualSize);

                    // Decrement count:
                    --(*count);
                    break;

                case TX_QUEUE_EMPTY:
                    queueStatus = QUEUE_NO_MORE_MSGS;
                    actualSize = 0;
                    break;
                case TX_DELETED:
                case TX_QUEUE_ERROR:
                case TX_WAIT_ABORTED:
                case TX_PTR_ERROR:
                case TX_WAIT_ERROR:
                default:
                      // TODO: port logMsg
//                    logMsg("Queue receive error %s! %d %d\n",(int)strerror(errno),(int)this->m_handle,(int)block,0,0,0);
                    printf("Queue recv [ERROR][0x%.2x]\n", ret);
                    queueStatus = QUEUE_UNKNOWN_ERROR;
                    actualSize = 0;
                    break;
            }
        }
        return queueStatus;
    }

    NATIVE_INT_TYPE Queue::getNumMsgs() const {

        CHAR *name;
        ULONG enqueued;
        ULONG availableStorage;
        TX_THREAD *firstSuspended;
        ULONG suspendedCount;
        TX_QUEUE *nextQueue;

        if (TX_SUCCESS != tx_queue_info_get(reinterpret_cast<TX_QUEUE*>(this->m_handle), &name, &enqueued, &availableStorage, &firstSuspended, &suspendedCount,&nextQueue))
        {
            enqueued = 0;
        }

        return (NATIVE_INT_TYPE)enqueued;
    }

    NATIVE_INT_TYPE Queue::getMaxMsgs() const{
        return reinterpret_cast<queueHandleData_t*>(this->m_handle)->maxCount;
    }

    NATIVE_INT_TYPE Queue::getQueueSize() const {
        return reinterpret_cast<queueHandleData_t*>(this->m_handle)->depth;
    }

    NATIVE_INT_TYPE Queue::getMsgSize() const {
        return reinterpret_cast<queueHandleData_t*>(this->m_handle)->msgSize;
    }

}


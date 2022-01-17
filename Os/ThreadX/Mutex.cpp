#include <Os/Mutex.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/TaskString.hpp>

#include "app_azure_rtos.h"

namespace Os {
    Mutex::Mutex(void) {
        TaskString name = "f_prime_mutex";

        // ThreadX memory pool pointer
        TX_BYTE_POOL* bytePoolPtr;

        // Get ThreadX memory pool
        UINT status = app_tx_get_byte_pool(&bytePoolPtr);
        FW_ASSERT(status == TX_SUCCESS);

        // Allocate the memory for the mutex control structure
        status = tx_byte_allocate(bytePoolPtr, (VOID **)&this->m_handle, sizeof(TX_MUTEX), TX_NO_WAIT);
        FW_ASSERT(status == TX_SUCCESS);

        status = tx_mutex_create(reinterpret_cast<TX_MUTEX*>(this->m_handle), (char*)name.toChar(), TX_INHERIT);
        FW_ASSERT(status == TX_SUCCESS);
    }

    Mutex::~Mutex(void) {
        UINT status = tx_mutex_delete(reinterpret_cast<TX_MUTEX*>(this->m_handle));
        // TODO: if ret != TX_SUCCESS log a memory leak

        // Release auxiliary handle allocated memory
        status = tx_byte_release((VOID *)this->m_handle);
        // TODO: if ret != TX_SUCCESS log a memory leak
    }

    void Mutex::lock(void) {
        UINT status = tx_mutex_get(reinterpret_cast<TX_MUTEX*>(this->m_handle), TX_WAIT_FOREVER);
        FW_ASSERT(status == TX_SUCCESS);
    }

    void Mutex::unLock(void) {
        UINT status = tx_mutex_put(reinterpret_cast<TX_MUTEX*>(this->m_handle));
        FW_ASSERT(status == TX_SUCCESS);
    }

}



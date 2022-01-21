#include <Os/WatchdogTimer.hpp>
#include <Fw/Types/Assert.hpp>
#include <tx_api.h>

#include "app_azure_rtos.h"

typedef struct wdCallbackWrapperTag {
    Os::WatchdogTimer::WatchdogCb callback; // routine to call on time-out
    void* parameter;                        // parameter with which to call routine
} wdCallbackWrapper_t;

// Type used to piggyback on class member "m_handle" all required data to
// operate a Watchdog object without the need to add extra members in WatchdogTimer.hpp
typedef struct wdHandleDataTag
{
    TX_TIMER m_handle;                     // handle for implementation specific watchdog
    wdCallbackWrapper_t wdCallbackWrapper; // structure holding the input Watchdog callback and input argument
} wdHandleData_t;

static void wd_callback_wrapper(ULONG parameter) {
    FW_ASSERT(parameter);
    wdCallbackWrapper_t *wd = reinterpret_cast<wdCallbackWrapper_t *>(parameter);
    FW_ASSERT(wd->callback);
    wd->callback(wd->parameter);
}

namespace Os {
    
    WatchdogTimer::WatchdogTimer()
    : m_handle(0), m_parameter(0), m_timerTicks(0), m_timerMs(0)
    {
#ifdef __SIZEOF_POINTER__
        FW_ASSERT( !(sizeof(this->m_handle) < __SIZEOF_POINTER__) );
#else
  #error Unable to verify a pointer size!
#endif

        // ThreadX memory pool pointer
        TX_BYTE_POOL* bytePoolPtr;

        if(app_tx_get_byte_pool(&bytePoolPtr) == TX_SUCCESS)
        {
            // Allocate the memory for auxiliary watchdog data handling structure
            if (tx_byte_allocate(bytePoolPtr, (VOID **)&this->m_handle, sizeof(wdHandleData_t), TX_NO_WAIT) != TX_SUCCESS)
            {
                this->m_handle = 0;

                //TODO: Log memory allocation error
            }
        }
    }

    WatchdogTimer::~WatchdogTimer() {

        CHAR *name;
        UINT active;
        ULONG remaining_ticks;
        ULONG reschedule_ticks;
        TX_TIMER *next_timer;

        // If timer exists, delete it
        if (this->m_handle)
        {
            if (TX_SUCCESS == tx_timer_info_get(reinterpret_cast<TX_TIMER*>(this->m_handle), &name, &active,&remaining_ticks, &reschedule_ticks, &next_timer))
            {
                (void)tx_timer_delete(reinterpret_cast<TX_TIMER*>(this->m_handle));
            }

            // Release auxiliary handle allocated memory
            UINT ret = tx_byte_release((VOID *)this->m_handle);
            // TODO: if ret != TX_SUCCESS log a memory leak
        }
    }

    WatchdogTimer::WatchdogStatus WatchdogTimer::startTicks( I32 delayInTicks, WatchdogCb p_callback, void* parameter) {

        FW_ASSERT(p_callback);

        CHAR *name;
        CHAR wdTimerName[] = "WatchdogTimer";
        UINT active;
        ULONG remaining_ticks;
        ULONG reschedule_ticks;
        TX_TIMER *next_timer;

        WatchdogStatus wdStatus = WATCHDOG_START_ERROR;

        if (TX_SUCCESS != tx_timer_info_get(reinterpret_cast<TX_TIMER*>(this->m_handle), &name, &active,&remaining_ticks, &reschedule_ticks, &next_timer))
        {
            // The Watchdog has not yet been created
            wdCallbackWrapper_t* pWdCallbackWrapper = &reinterpret_cast<wdHandleData_t*>(this->m_handle)->wdCallbackWrapper;
            pWdCallbackWrapper->callback  = p_callback;
            pWdCallbackWrapper->parameter = parameter;

            if (TX_SUCCESS == tx_timer_create(reinterpret_cast<TX_TIMER*>(this->m_handle),
                                       wdTimerName,
                                       wd_callback_wrapper,
                                       reinterpret_cast<ULONG>(pWdCallbackWrapper),
                                       delayInTicks,
                                       0,
                                       TX_AUTO_ACTIVATE))
            {
                this->m_timerTicks = delayInTicks;
                wdStatus = WATCHDOG_OK;
            }
        }
        // The Watchdog has been created and is no longer active
        else if(TX_FALSE == active)
        {
            // Reset the Watchdog
            if (TX_SUCCESS == tx_timer_change(reinterpret_cast<TX_TIMER*>(this->m_handle), delayInTicks, 0))
            {
                // Restart the Watchdog
                if (TX_SUCCESS == tx_timer_activate(reinterpret_cast<TX_TIMER*>(this->m_handle)))
                {
                    this->m_timerTicks = delayInTicks;
                    wdStatus = WATCHDOG_OK;
                }
            }
        }

        return wdStatus;
    }

    WatchdogTimer::WatchdogStatus WatchdogTimer::startMs( I32 delayInMs, WatchdogCb p_callback, void* parameter) {

        FW_ASSERT(p_callback);

        WatchdogStatus wdStatus = WATCHDOG_START_ERROR;

        // convert to ticks. Take the number
        // of ticks per second and multiply by the number of seconds.
        // Add 999 to force a roundup situation.
        U32 ticks = ((delayInMs * TX_TIMER_TICKS_PER_SECOND + 999) / 1000);

        wdStatus = this->startTicks(ticks, p_callback, parameter);

        if(WATCHDOG_OK == wdStatus)
        {
            this->m_timerMs = delayInMs;
            this->m_timerTicks = ticks;
        }

        return wdStatus;
    }

    WatchdogTimer::WatchdogStatus WatchdogTimer::restart(void) {

        WatchdogStatus wdStatus = WATCHDOG_START_ERROR;

        CHAR *name;
        UINT active;
        ULONG remaining_ticks;
        ULONG reschedule_ticks;
        TX_TIMER *next_timer;

        if (TX_SUCCESS == tx_timer_info_get(reinterpret_cast<TX_TIMER*>(this->m_handle), &name, &active,&remaining_ticks, &reschedule_ticks, &next_timer))
        {
            // If the Watchdog is no longer active
            if(TX_FALSE == active)
            {
                // Reset the Watchdog
                if (TX_SUCCESS == tx_timer_change(reinterpret_cast<TX_TIMER*>(this->m_handle), this->m_timerTicks, 0))
                {
                    // Restart the Watchdog
                    if (TX_SUCCESS == tx_timer_activate(reinterpret_cast<TX_TIMER*>(this->m_handle)))
                    {
                        wdStatus = WATCHDOG_OK;
                    }
                }
            }
        }

        return wdStatus;
    }

    WatchdogTimer::WatchdogStatus WatchdogTimer::cancel(void) {

        WatchdogStatus wdStatus = WATCHDOG_CANCEL_ERROR;

        CHAR *name;
        UINT active;
        ULONG remaining_ticks;
        ULONG reschedule_ticks;
        TX_TIMER *next_timer;

        if (TX_SUCCESS == tx_timer_info_get(reinterpret_cast<TX_TIMER*>(this->m_handle), &name, &active,&remaining_ticks, &reschedule_ticks, &next_timer))
        {
            // The exists so deactivate it
            if (TX_SUCCESS == tx_timer_deactivate(reinterpret_cast<TX_TIMER*>(this->m_handle))) wdStatus = WATCHDOG_OK;
        }

        return wdStatus;
    }

}



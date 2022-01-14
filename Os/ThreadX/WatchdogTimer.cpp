#include <Os/WatchdogTimer.hpp>
#include <Fw/Types/Assert.hpp>

static void wd_callback_wrapper(ULONG parameter) {
    FW_ASSERT(parameter);
    Os::WatchdogTimer::WdCallbackWrapper *wd = reinterpret_cast<Os::WatchdogTimer::WdCallbackWrapper *>(parameter);
    FW_ASSERT(wd->callback);
    wd->callback(wd->parameter);
}

namespace Os {
    
    WatchdogTimer::WatchdogTimer()
    : m_wdCallbackWrapper(),m_parameter(0),m_timerTicks(0),m_timerMs(0)
    {
    }

    WatchdogTimer::~WatchdogTimer() {

        CHAR *name;
        UINT active;
        ULONG remaining_ticks;
        ULONG reschedule_ticks;
        TX_TIMER *next_timer;

        // If timer exists, delete it
        if (TX_SUCCESS == tx_timer_info_get(&this->m_handle, &name, &active,&remaining_ticks, &reschedule_ticks, &next_timer))
        {
            tx_timer_delete(&this->m_handle);
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

        if (TX_SUCCESS != tx_timer_info_get(&this->m_handle, &name, &active,&remaining_ticks, &reschedule_ticks, &next_timer))
        {
            // The Watchdog has not yet been created
            this->m_wdCallbackWrapper.callback = p_callback;
            this->m_wdCallbackWrapper.parameter = parameter;

            if (TX_SUCCESS == tx_timer_create(&this->m_handle,
                                       wdTimerName,
                                       wd_callback_wrapper,
                                       reinterpret_cast<ULONG>(&this->m_wdCallbackWrapper),
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
            if (TX_SUCCESS == tx_timer_change(&this->m_handle, delayInTicks, 0))
            {
                // Restart the Watchdog
                if (TX_SUCCESS == tx_timer_activate(&this->m_handle))
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

        if (TX_SUCCESS == tx_timer_info_get(&this->m_handle, &name, &active,&remaining_ticks, &reschedule_ticks, &next_timer))
        {
            // If the Watchdog is no longer active
            if(TX_FALSE == active)
            {
                // Reset the Watchdog
                if (TX_SUCCESS == tx_timer_change(&this->m_handle, this->m_timerTicks, 0))
                {
                    // Restart the Watchdog
                    if (TX_SUCCESS == tx_timer_activate(&this->m_handle))
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

        if (TX_SUCCESS == tx_timer_info_get(&this->m_handle, &name, &active,&remaining_ticks, &reschedule_ticks, &next_timer))
        {
            // The exists so deactivate it
            if (TX_SUCCESS == tx_timer_deactivate(&this->m_handle)) wdStatus = WATCHDOG_OK;
        }

        return wdStatus;
    }

}



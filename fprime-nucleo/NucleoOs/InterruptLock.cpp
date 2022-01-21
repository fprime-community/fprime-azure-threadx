#include <Os/InterruptLock.hpp>

#include <tx_api.h>

//                         !!! WARNING (THREADX) !!!
// This service should not be used to enable interrupts during initialisation!
// Doing so could cause unpredictable results.

namespace Os {
    InterruptLock::InterruptLock(void): m_key(0) {}
    InterruptLock::~InterruptLock(void) {}
    
    void InterruptLock::lock(void) {
        this->m_key = (POINTER_CAST)tx_interrupt_control(TX_INT_DISABLE);
    }

    void InterruptLock::unLock(void) {
    	tx_interrupt_control((UINT)this->m_key);
    }
    
    POINTER_CAST InterruptLock::getKey(void) {
        return this->m_key;
    }
        
}



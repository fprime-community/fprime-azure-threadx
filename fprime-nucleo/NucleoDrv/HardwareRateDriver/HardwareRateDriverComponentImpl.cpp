#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include "HardwareRateDriverComponentImpl.hpp"

namespace Nucleo {

    HardwareRateDriverComponentImpl* HardwareRateDriverComponentImpl::s_driver = NULL;

    HardwareRateDriverComponentImpl::HardwareRateDriverComponentImpl(const char* compName, U32 intervalMs) :
        HardwareRateDriverComponentBase(compName),
        m_interval(intervalMs)
    {
        s_driver = this;
    }

    HardwareRateDriverComponentImpl::~HardwareRateDriverComponentImpl(void) {}

    void HardwareRateDriverComponentImpl::s_timer(void* comp) {
        Svc::TimerVal now;
        now.take();
        HardwareRateDriverComponentImpl* driver = reinterpret_cast<HardwareRateDriverComponentImpl*>(comp);
        //Check if it is time to run the group
        driver->CycleOut_out(0, now);
        driver->m_last = now;
    }
}

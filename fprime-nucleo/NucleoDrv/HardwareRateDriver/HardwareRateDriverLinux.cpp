#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/Task.hpp>
#include <Fw/Types/EightyCharString.hpp>
#include <sys/time.h>
#include "HardwareRateDriverComponentImpl.hpp"

namespace Nucleo {
Os::Task s_task;
//Temporary function used to time for linux
void linux_task_function(void* vself) {
    HardwareRateDriverComponentImpl* self = reinterpret_cast<HardwareRateDriverComponentImpl*>(vself);
    struct timeval time;
    gettimeofday(&time, NULL);
    U64 time_us = time.tv_sec * (int)1e6 + time.tv_usec;
    if ((time_us % (self->m_interval * 1000)) == 0) {
        HardwareRateDriverComponentImpl::s_timer(self);
    }
}

void HardwareRateDriverComponentImpl::start() {
    Fw::EightyCharString fake("FakeNucleo");
    s_task.setStarted(true);
    s_task.start(fake, 0xdeafbeef, 255, 0, linux_task_function, this, 0);
}
void HardwareRateDriverComponentImpl::stop() {
    s_task.setStarted(false);
}
void HardwareRateDriverComponentImpl::s_timerISR(void* param) {}
};

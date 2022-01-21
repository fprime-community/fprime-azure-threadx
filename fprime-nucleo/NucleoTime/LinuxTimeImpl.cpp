#include <fprime-nucleo/NucleoTime/NucleoTimeImpl.hpp>
#include <Fw/Time/Time.hpp>
#include <time.h>

namespace Svc {

    NucleoTimeImpl::NucleoTimeImpl(const char* name) : TimeComponentBase(name) { }

    NucleoTimeImpl::~NucleoTimeImpl() { }

    void NucleoTimeImpl::timeGetPort_handler(
            NATIVE_INT_TYPE portNum, /*!< The port number*/
            Fw::Time &time /*!< The U32 cmd argument*/
        ) {
        timespec stime;
        (void)clock_gettime(CLOCK_REALTIME,&stime);
        time.set(TB_WORKSTATION_TIME,0, stime.tv_sec, stime.tv_nsec/1000);
    }

    void NucleoTimeImpl::init(NATIVE_INT_TYPE instance) {
        TimeComponentBase::init(instance);
    }

}

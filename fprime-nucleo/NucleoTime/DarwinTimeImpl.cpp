#include <fprime-nucleo/NucleoTime/NucleoTimeImpl.hpp>
#include <Fw/Time/Time.hpp>
#include <sys/time.h>

namespace Svc {

    NucleoTimeImpl::NucleoTimeImpl(const char* name) : TimeComponentBase(name) { }

    NucleoTimeImpl::~NucleoTimeImpl() { }

    void NucleoTimeImpl::timeGetPort_handler(
            NATIVE_INT_TYPE portNum, /*!< The port number*/
            Fw::Time &time /*!< The U32 cmd argument*/
        ) {
        timeval stime;
        (void)gettimeofday(&stime,0);
        time.set(TB_WORKSTATION_TIME,0,stime.tv_sec, stime.tv_usec);
    }

    void NucleoTimeImpl::init(NATIVE_INT_TYPE instance) {
        Svc::TimeComponentBase::init(instance);
    }

}

#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include "HardwareRateDriverComponentImpl.hpp"

// TODO: For now use a software timer. If more accuracy is required then an HW timer must be used
#include <Os/WatchdogTimer.hpp>

namespace Nucleo {

  // TODO: For now use a software timer. If more accuracy is required then an HW timer must be used
  static Os::WatchdogTimer s_itimer;

  void HardwareRateDriverComponentImpl::start() {
      Os::WatchdogTimer::WatchdogStatus status = s_itimer.startMs(m_interval, HardwareRateDriverComponentImpl::s_timerISR, nullptr);
      if (status != Os::WatchdogTimer::WatchdogStatus::WATCHDOG_OK) {
    	  //TODO: Blink red LED
      }
  }

  void HardwareRateDriverComponentImpl::stop() {
	  // TODO: For now use a software timer. If more accuracy is required then an HW timer must be used
      s_itimer.cancel();
  }

  void HardwareRateDriverComponentImpl::s_timerISR(void* param) {
      s_timer(s_driver);
      s_itimer.restart();
  }

};

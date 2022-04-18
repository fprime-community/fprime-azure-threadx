// ====================================================================== 
// \title  LedBlinkerImpl.cpp
// \author lestarch
// \brief  cpp file for LedBlinker component implementation class
// ====================================================================== 


#include <LedBlinker/LedBlinkerComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace Nucleo {

  // ----------------------------------------------------------------------
  // Construction, initialisation, and destruction
  // ----------------------------------------------------------------------

  LedBlinkerComponentImpl ::
    LedBlinkerComponentImpl(
        const char *const compName
    ) :
      LedBlinkerComponentBase(compName), 
    m_led(LED_GREEN),
    m_state(false)
  {}

  LedBlinkerComponentImpl ::
    ~LedBlinkerComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void LedBlinkerComponentImpl ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
      blink();
      m_state = !m_state;
  }

} // end namespace Nucleo

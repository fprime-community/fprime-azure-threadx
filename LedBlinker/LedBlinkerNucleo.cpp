// ====================================================================== 
// \title  LedBlinkerImpl.cpp
// \author lestarch
// \brief  cpp file for LedBlinker component implementation class
// ====================================================================== 

#include "LedBlinker/LedBlinkerComponentImpl.hpp"
#include "Fw/Types/BasicTypes.hpp"

#include "stm32h7xx_nucleo.h"

namespace Nucleo {

  void LedBlinkerComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    ) 
  {
    LedBlinkerComponentBase::init(instance);
    BSP_LED_Init(LED_GREEN);
    BSP_LED_Init(LED_YELLOW);
    BSP_LED_Init(LED_RED);
  }

  void LedBlinkerComponentImpl ::
    blink()
  {
      m_state ? BSP_LED_On(m_led) : BSP_LED_Off(m_led);
  }
} // end namespace Nucleo

// ====================================================================== 
// \title  LedBlinkerImpl.cpp
// \author lestarch
// \brief  cpp file for LedBlinker component implementation class
// ====================================================================== 


#include "Fw/Types/BasicTypes.hpp"

#include <Arduino.h> // TODO: Replace by STM32 equivalent

#include "../../NucleoBlink/LedBlinker/LedBlinker.hpp"
namespace Nucleo {

  void LedBlinkerComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    ) 
  {
    LedBlinkerComponentBase::init(instance);
    pinMode(m_pin, OUTPUT);

  }

  void LedBlinkerComponentImpl ::
    blink()
  {
      digitalWrite(m_pin, m_state);
  }
} // end namespace Nucleo

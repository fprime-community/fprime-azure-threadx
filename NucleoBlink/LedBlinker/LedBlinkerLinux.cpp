// ====================================================================== 
// \title  LedBlinkerImpl.cpp
// \author lestarch
// \brief  cpp file for LedBlinker component implementation class
// ====================================================================== 


#include "Fw/Types/BasicTypes.hpp"

#include <stdio.h>
#include "../../NucleoBlink/LedBlinker/LedBlinker.hpp"

namespace Nucleo {
  void LedBlinkerComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    ) 
  {
    LedBlinkerComponentBase::init(instance);
  }

  void LedBlinkerComponentImpl ::
    blink()
  {
      static U64 s_counter = 0;
      if ((s_counter % 10) == 0) {
          printf("Setting: %lu to %d\n", m_pin, m_state);
      }
      s_counter += 1;
  }
} // end namespace Nucleo

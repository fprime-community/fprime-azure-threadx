// ======================================================================
// \title  SerialDriverImpl.cpp
// \author lestarch
// \brief  cpp file for SerialDriver component implementation class
// ======================================================================


#include "Fw/Types/BasicTypes.hpp"
#include <Os/Log.hpp>
#include <Fw/Types/Assert.hpp>
#include "SerialDriverComponentImpl.hpp"

#include "usart.h"
#include "stm32h7xx_hal.h"

namespace Nucleo {

  void SerialDriverComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance,
        const NATIVE_UINT_TYPE baud
    )
  {
	  (void)baud;
      SerialDriverComponentBase::init(instance);
      switch (m_port_number) {
          case 0:
        	  // USART6
              m_port_pointer = reinterpret_cast<POINTER_CAST>(&huart6);
              // TODO: Decide where should the serial USART be initialised: here or at stm32ide_main.c
              //MX_USART6_UART_Init();
              break;
          default:
              // Unsupported port
              m_port_pointer = reinterpret_cast<POINTER_CAST>(nullptr);
        	  break;
      }
  }


  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void SerialDriverComponentImpl ::
    write_data(
        Fw::Buffer &fwBuffer
    )
  {
      switch (m_port_number)
      {
          case 0:
        	  // USART6
        	  usart6_send(reinterpret_cast<const U8*>(fwBuffer.getData()),fwBuffer.getSize());
              break;
          default:
              // Unsupported port
        	  break;
      }
  }

  void SerialDriverComponentImpl ::
    read_data(Fw::Buffer& fwBuffer)
  {
      NATIVE_UINT_TYPE count = 0;
      U8* raw_data = reinterpret_cast<U8*>(fwBuffer.getData());

      switch (m_port_number)
      {
          case 0:
        	  // USART6
        	  count = usart6_recv(raw_data, fwBuffer.getSize());
              break;
          default:
              // Unsupported port
        	  break;
      }

      fwBuffer.setSize(count);
  }
} // end namespace Nucleo

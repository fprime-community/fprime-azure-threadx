/*
 * StreamLog.hpp:
 *
 * A file defining the functions used to set and configure the Stream used in the Nucleo Log::Os
 * handler function.
 *
 *  Created on: Apr 13, 2019
 *      Author: lestarch
 */
//TODO: #include <Arduino.h> replace by STM32 equivalent
#ifndef OS_NUCLEO_STREAMLOG_HPP_
#define OS_NUCLEO_STREAMLOG_HPP_

// TODO: (START) namespace for Arduino related code which needs to be replaced by STM32 equivalent
namespace _ARDUINO_NS_ {

#include <cstdlib>
#include <stdint.h>

class Stream
{
  public:
    virtual size_t write(char* buffer, size_t size) = 0;

    Stream() {}
};

} // TODO: (END) namespace for Arduino related code which needs to be replaced by STM32 equivalent

namespace Os {
/**
 * Sets the global A_STREAM arduino stream used to log Arudino messages through a hardware "Steam"
 * like UART or I2C.
 */
void setNucleoStreamLogHandler(_ARDUINO_NS_::Stream* stream);
}
#endif /* OS_NUCLEO_STREAMLOG_HPP_ */

/*
 * StreamLog.hpp:
 *
 * A file defining the functions used to set and configure the Stream used in the Nucleo Log::Os
 * handler function.
 *
 *  Created on: Apr 13, 2019
 *      Author: lestarch
 */
#include <Arduino.h>
#ifndef OS_NUCLEO_STREAMLOG_HPP_
#define OS_NUCLEO_STREAMLOG_HPP_

namespace Os {
/**
 * Sets the global A_STREAM arduino stream used to log Arudino messages through a hardware "Steam"
 * like UART or I2C.
 */
void setNucleoStreamLogHandler(Stream* stream);
}
#endif /* OS_NUCLEO_STREAMLOG_HPP_ */

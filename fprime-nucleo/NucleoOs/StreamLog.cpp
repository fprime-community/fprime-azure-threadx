/**
 * LogNucleoStream:
 *
 * Implementation of Os::Log that defers to Nucleo's Stream type. This allows for a hardware based
 * stream capture. Note: in order to set the stream, the companion header Os/Nucleo/StreamLog.hpp
 * should be used.
 *
 *  Created on: Apr 13, 2019
 *      Author: lestarch
 */

#include <Os/Log.hpp>
#include <fprime-nucleo/NucleoOs/StreamLog.hpp>

extern "C" {
    #include <stdio.h>
    #include <string.h>
}

// TODO: (START) namespace for Arduino related code which needs to be replaced by STM32 equivalent
namespace _ARDUINO_NS_ {

size_t Stream::write(char* buffer, size_t size)
{
  (void)buffer;
  (void)size;
  return 0;
}

} // TODO: (END) namespace for Arduino related code which needs to be replaced by STM32 equivalent

namespace Os {
    _ARDUINO_NS_::Stream* A_STREAM = NULL; // I love it when a plan comes together.
    void setNucleoStreamLogHandler(_ARDUINO_NS_::Stream* stream) {
        A_STREAM = stream;
    }
    /**
     * Register as the default logger
     */
    Log::Log() {
        // Register myself as a logger at construction time. If used in unison with LogDefault.cpp, this will
        // automatically create this as a default logger.
        this->registerLogger(this);
    }
    /**
     * An Nucleo Stream implementation of the Os::Log function. Essentially, this log message
     * handler formats a message an sends it out to the serial port/Wire port or Nucleo stream
     * backing the stream object. This allows for a side-channel serial console.
     */
    void Log::log(
        const char* format,
        POINTER_CAST a0,
        POINTER_CAST a1,
        POINTER_CAST a2,
        POINTER_CAST a3,
        POINTER_CAST a4,
        POINTER_CAST a5,
        POINTER_CAST a6,
        POINTER_CAST a7,
        POINTER_CAST a8,
        POINTER_CAST a9
    ) {
        char message[128];
        // If stream is defined, then write to it.
        if (A_STREAM != NULL) {
            U32 used = snprintf(message, sizeof(message), format, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
	    used = (used > sizeof(message)) ? sizeof(message) : used;
            A_STREAM->write(message, used);
        }
    }
}

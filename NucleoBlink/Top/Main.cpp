#include <NucleoBlink/Top/Components.hpp>
#include <fprime-nucleo/NucleoTypes/GenericLogAssert.hpp>
#include <Os/Log.hpp>
#ifdef NUCLEO
    #include <fprime-nucleo/NucleoOs/StreamLog.hpp>
//TODO: #include <Arduino.h> replace by STM32 equivalent
#else
    #include <fprime-nucleo/NucleoDrv/SerialDriver/SerialDriver.hpp>
#endif

// Global handlers for this Topology
Fw::LogAssertHook assert;

#define STARTUP_DELAY_MS 2000

/**
 * Main function.
 */
int main(int argc, char* argv[]) {
    Os::Log logger;
    assert.registerHook();
#ifdef NUCLEO
    // Start Serial for logging, and give logger time to connect
    Serial.begin(9600);
    delay(STARTUP_DELAY_MS);
    // Setup log handler
    Os::setNucleoStreamLogHandler(&Serial);
    Fw::Logger::logMsg("[SETUP] Logger registered, hello world!\n", 0, 0, 0, 0, 0, 0);
#else
    // Set serial port
    FW_ASSERT(argc <= 2);
    if (argc == 2) {
        Nucleo::SERIAL_PORT = reinterpret_cast<char**>(&argv[1]);
    }
#endif
    Fw::Logger::logMsg("[SETUP] Constructing system\n", 0, 0, 0, 0, 0, 0);
    constructApp();
    Fw::Logger::logMsg("[SETUP] Lanuching rate groups\n", 0, 0, 0, 0, 0, 0);
    // Start the task for the rate group
    while (1) {
        taskRunner.run();
    }
    return 0;
}

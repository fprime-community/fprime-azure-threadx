#include <cstdio>
#include <cstdlib>
#include <ctype.h>
#include <getopt.h>
#include <signal.h>

#include <Top/NucleoTopologyAc.hpp>

Nucleo::TopologyState state;
// Enable the console logging provided by Os::Log
Os::Log logger;

void print_usage(const char* app) {
    (void) printf("Usage: ./%s [options]\n-p\tport_number\n-a\thostname/IP address\n",app);
}

// Handle a signal, e.g. control-C
static void sighandler(int signum) {
    // Call the teardown function
    // This causes the Linux timer to quit
    RPI::teardown(state);
}

int main(int argc, char* argv[]) {
    I32 option = 0;

    while ((option = getopt(argc, argv, "hp:a:")) != -1){
        switch(option) {
            case 'h':
                print_usage(argv[0]);
                return 0;
                break;
            case 'p':
                state.portNumber = static_cast<U32>(atoi(optarg));
                break;
            case 'a':
                state.hostName = optarg;
                break;
            case '?':
                return 1;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    (void) printf("Hit Ctrl-C to quit\n");
    Nucleo::setup(state);

    // register signal handlers to exit program
    signal(SIGINT,sighandler);
    signal(SIGTERM,sighandler);

    // Start the Linux timer.
    // The timer runs on the main thread until it quits
    // in the teardown function, called from the signal
    // handler.
    Nucleo::linuxTimer.startTimer(100); //!< 10Hz

    // Signal handler was called, and linuxTimer quit.
    // Time to exit the program.
    // Give time for threads to exit.
    (void) printf("Waiting for threads...\n");
    Os::Task::delay(1000);

    (void) printf("Exiting...\n");

    return 0;
}


//#include <Top/Components.hpp>
//#include <fprime-nucleo/NucleoTypes/GenericLogAssert.hpp>
//#include <Os/Log.hpp>
//#ifdef NUCLEO
//    #include <fprime-nucleo/NucleoOs/StreamLog.hpp>
////TODO: #include <Arduino.h> replace by STM32 equivalent
//#else
//    #include "../fprime-nucleo/NucleoDrv/SerialDriver/SerialDriverComponentImpl.hpp"
//#endif
//
//// Global handlers for this Topology
//Fw::LogAssertHook assert;
//
//#define STARTUP_DELAY_MS 2000
//
///**
// * Main function.
// */
//int main(int argc, char* argv[]) {
//    Os::Log logger;
//    assert.registerHook();
//#ifdef NUCLEO
//    // Start Serial for logging, and give logger time to connect
//    Serial.begin(9600);
//    delay(STARTUP_DELAY_MS);
//    // Setup log handler
//    Os::setNucleoStreamLogHandler(&Serial);
//    Fw::Logger::logMsg("[SETUP] Logger registered, hello world!\n", 0, 0, 0, 0, 0, 0);
//#else
//    // Set serial port
//    FW_ASSERT(argc <= 2);
//    if (argc == 2) {
//        Nucleo::SERIAL_PORT = reinterpret_cast<char**>(&argv[1]);
//    }
//#endif
//    Fw::Logger::logMsg("[SETUP] Constructing system\n", 0, 0, 0, 0, 0, 0);
//    constructApp();
//    Fw::Logger::logMsg("[SETUP] Lanuching rate groups\n", 0, 0, 0, 0, 0, 0);
//    // Start the task for the rate group
//    while (1) {
//        taskRunner.run();
//    }
//    return 0;
//}

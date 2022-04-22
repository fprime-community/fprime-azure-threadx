module Nucleo {

  @ An interrupt based driver for Nucleo.
  passive component SerialDriver {
  
    # ----------------------------------------------------------------------
    # Types
    # ----------------------------------------------------------------------

    # ----------------------------------------------------------------------
    # General Ports
    # ----------------------------------------------------------------------
    
    @ TODO: Comment
    sync input port write: Fw.BufferSend
    
    @ TODO: Comment
    sync input port readPoll: Fw.BufferSend
    
    @ TODO: Comment
    output port readCallback: Fw.BufferSend
    
    @ Input Scheduler port - called at 10Hz
    sync input port schedIn: Svc.Sched

    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    # ----------------------------------------------------------------------
    # Parameters
    # ----------------------------------------------------------------------

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

  }

}


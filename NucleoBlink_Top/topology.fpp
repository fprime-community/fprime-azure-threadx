module Nucleo {

  topology Nucleo {

    # ----------------------------------------------------------------------
    # Instances used in the topology
    # ----------------------------------------------------------------------

    instance $health
    instance nucleoTime
    instance chanTlm
    instance cmdDisp
    instance comm
    instance downlink
    instance uplink
    instance eventLogger
    instance hardwareRateDriver
    instance ledBlinker
    instance rateGroupDriverComp
    instance rateGroup10HzComp
    instance rateGroup1HzComp

    # ----------------------------------------------------------------------
    # Pattern graph specifiers
    # ----------------------------------------------------------------------

    command connections instance cmdDisp

    event connections instance eventLogger

    health connections instance $health

    telemetry connections instance chanTlm

    time connections instance nucleoTime

    # ----------------------------------------------------------------------
    # Direct graph specifiers
    # ----------------------------------------------------------------------
    
    # *** Command Registration/Dispatch/Reply Connections ***
    
    # Command Registration Ports - Registration port number must match dispatch port for each component
    connections RegistrationPortsConn {
      eventLogger.CmdReg[0] -> cmdDisp.compCmdReg[0]
      cmdDisp.CmdReg[0] -> cmdDisp.compCmdReg[1]
      $health.CmdReg[0] -> cmdDisp.compCmdReg[2]
    }
    
    # Command Dispatch Ports - Dispatch port number must match registration port for each component
    connections DispatchPortsConn {
      cmdDisp.compCmdSend[0] -> eventLogger.CmdDisp[0]
      cmdDisp.compCmdSend[1] -> cmdDisp.CmdDisp[0]
      cmdDisp.compCmdSend[2] -> $health.CmdDisp[0]
    }
    
    # Command Reply Ports - Go to the same response port on the dispatcher
    connections ReplyPortsConn {
      eventLogger.CmdStatus[0] -> cmdDisp.compCmdStat[0]
      cmdDisp.CmdStatus[0] -> cmdDisp.compCmdStat[0]
      $health.CmdStatus[0] -> cmdDisp.compCmdStat[0]
    }
    
    # Event Logger Binary Connections
    connections EventLoggerConn {
      eventLogger.Log[0] -> eventLogger.LogRecv[0]
      cmdDisp.Log[0] -> eventLogger.LogRecv[0]
      $health.Log[0] -> eventLogger.LogRecv[0]
      rateGroup10HzComp.Log[0] -> eventLogger.LogRecv[0]
      rateGroup1HzComp.Log[0] -> eventLogger.LogRecv[0]
    }
    
    # *** Event Logger Text Connections ***
    
    # Telemetry Connections
    connections TelemetryConn {
      cmdDisp.Tlm[0] -> chanTlm.TlmRecv[0]
      rateGroup1HzComp.Tlm[0] -> chanTlm.TlmRecv[0]
      rateGroup10HzComp.Tlm[0] -> chanTlm.TlmRecv[0]
      $health.Tlm[0] -> chanTlm.TlmRecv[0]
    }
    
    # Time Connections
    connections Time {
      eventLogger.Time[0] -> nucleoTime.timeGetPort[0]
      rateGroup10HzComp.Time[0] -> nucleoTime.timeGetPort[0]
      rateGroup1HzComp.Time[0] -> nucleoTime.timeGetPort[0]
      $health.Time[0] -> nucleoTime.timeGetPort[0]
      cmdDisp.Time[0] -> nucleoTime.timeGetPort[0]
    }

    connections RateGroupsConn {
      # Linux Timer Connection
      # linuxTimer.CycleOut[0] -> rateGroupDriverComp.CycleIn[0]
    
      # Connect the Hardware Driver to the rate group driver
      hardwareRateDriver.CycleOut[0] -> rateGroupDriverComp.CycleIn[0]
    
      # 10Hz Rate Group
      rateGroupDriverComp.CycleOut[0] -> rateGroup10HzComp.CycleIn[0]
      rateGroup10HzComp.RateGroupMemberOut[0] -> uplink.schedIn[0]
      rateGroup10HzComp.RateGroupMemberOut[1] -> ledBlinker.schedIn[0]
      rateGroup10HzComp.RateGroupMemberOut[2] -> comm.schedIn[0]
    
      # 1Hz Rate Group
      rateGroupDriverComp.CycleOut[1] -> rateGroup1HzComp.CycleIn[0]
      rateGroup1HzComp.RateGroupMemberOut[0] -> chanTlm.Run[0]
      rateGroup1HzComp.RateGroupMemberOut[1] -> $health.Run[0]
    }

    # *** Health Connections ***

    # The PingSend output port number should match the PingReturn input port number
    # Each port number pair must be unique
    # This order must match the table in Topology.cpp
    connections HealthConn {
      $health.PingSend[0] -> rateGroup10HzComp.PingIn[0]
      rateGroup10HzComp.PingOut[0] -> $health.PingReturn[0]
      $health.PingSend[1] -> rateGroup1HzComp.PingIn[0]
      rateGroup1HzComp.PingOut[0] -> $health.PingReturn[1]
      $health.PingSend[2] -> cmdDisp.pingIn[0]
      cmdDisp.pingOut[0] -> $health.PingReturn[2]
      $health.PingSend[3] -> chanTlm.pingIn[0]
      chanTlm.pingOut[0] -> $health.PingReturn[3]
      $health.PingSend[4] -> eventLogger.pingIn[0]
      eventLogger.pingOut[0] -> $health.PingReturn[4]
    }

    # Ground Interface Connections
    connections DownlinkConn {
      groundInterface.uplinkPort[0] -> cmdDisp.seqCmdBuff[0]    ## DONE
      eventLogger.PktSend[0] -> groundInterface.downlinkPort[0] ## DONE
      chanTlm.PktSend[0] -> groundInterface.downlinkPort[0]     ## DONE
      groundInterface.write[0] -> comm.write[0]                 ## TODO: Connect Svc::Deframer     output  to Nucleo.SerialDriver
      groundInterface.readPoll[0] -> comm.readPoll[0]           ## TODO: Connect Svc::Deframer     output  to Nucleo.SerialDriver
    }
    
    # Uplink (Deframer) connections
    connections Uplink {
      cmdDisp.seqCmdStatus -> uplink.cmdResponseIn[0]
      uplink.comOut[0] -> cmdDisp.seqCmdBuff[0]
      
      ## Drv.Serial(UART) -> Deframer
    }
    
    # Downlink (Framer) connections
    connections Dowlink {
      chanTlm.PktSend[0] -> downlink.comIn[0]
      eventLogger.PktSend[0] -> downlink.comIn[0]
      
      ## Framer -> Drv.Serial(UART) 
    }

    # *** Fault Connectionss ***
    #connections FaultProtectionConn {
    #  eventLogger.FatalAnnounce -> fatalHandler.FatalReceive
    #}

  }

}


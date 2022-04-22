module Nucleo {

  # ----------------------------------------------------------------------
  # Defaults
  # ----------------------------------------------------------------------

  module Default {

    constant queueSize = 10

    constant stackSize = 16 * 1024

  }

  # ----------------------------------------------------------------------
  # Active component instances
  # ----------------------------------------------------------------------

  instance rateGroup10HzComp: Svc.ActiveRateGroup base id 200 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 40 \
  {

    phase Fpp.ToCpp.Phases.configObjects """
    NATIVE_UINT_TYPE context[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    """

    phase Fpp.ToCpp.Phases.instances """
    Svc::ActiveRateGroup rateGroup10HzComp(
        FW_OPTIONAL_NAME("rateGroup10HzComp"),
        ConfigObjects::rateGroup10HzComp::context,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::rateGroup10HzComp::context)
    );
    """

  }
  
  instance rateGroup1HzComp: Svc.ActiveRateGroup base id 300 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 40 \
  {

    phase Fpp.ToCpp.Phases.configObjects """
    NATIVE_UINT_TYPE context[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    """

    phase Fpp.ToCpp.Phases.instances """
    Svc::ActiveRateGroup rateGroup1HzComp(
        FW_OPTIONAL_NAME("rateGroup1HzComp"),
        ConfigObjects::rateGroup1HzComp::context,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::rateGroup1HzComp::context)
    );
    """

  }

  instance chanTlm: Svc.TlmChan base id 400 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 25

  instance cmdDisp: Svc.CommandDispatcher base id 500 \
    queue size 20 \
    stack size Default.stackSize \
    priority 30

  instance eventLogger: Svc.ActiveLogger base id 1400 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 25

  # ----------------------------------------------------------------------
  # Queued component instances
  # ----------------------------------------------------------------------

  instance $health: Svc.Health base id 1100 \
    queue size 25 \
  {

    phase Fpp.ToCpp.Phases.configConstants """
    enum {
      WATCHDOG_CODE = 0x123
    };
    """

    phase Fpp.ToCpp.Phases.configComponents """
    health.setPingEntries(
        ConfigObjects::health::pingEntries,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::health::pingEntries),
        ConfigConstants::health::WATCHDOG_CODE
    );
    """

  }

  # ----------------------------------------------------------------------
  # Passive component instances
  # ----------------------------------------------------------------------

  instance nucleoTime: Svc.Time base id 1500 \
    at "../NucleoTime/NucleoTime.hpp" \
  {

    phase Fpp.ToCpp.Phases.instances """
    Svc::NucleoTime nucleoTime(FW_OPTIONAL_NAME("Time"));
    """

  }

  instance rateGroupDriverComp: Svc.RateGroupDriver base id 1700 \
  {

    phase Fpp.ToCpp.Phases.configObjects """
    NATIVE_INT_TYPE rgDivs[Svc::RateGroupDriver::DIVIDER_SIZE] = { 1, 10, 0 };
    """

    phase Fpp.ToCpp.Phases.instances """
    Svc::RateGroupDriver rateGroupDriverComp(
        FW_OPTIONAL_NAME("rateGroupDriverComp"),
        ConfigObjects::rateGroupDriverComp::rgDivs,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::rateGroupDriverComp::rgDivs)
    );
    """

  }
  
  instance uplink: Svc.Deframer base id 1800 {

    phase Fpp.ToCpp.Phases.configObjects """
    Svc::FprimeDeframing deframing;
    """

    phase Fpp.ToCpp.Phases.configComponents """
    uplink.setup(ConfigObjects::uplink::deframing);
    """

  }
  
  instance ledBlinker: Nucleo.LedBlinker base id 1900 \
  {

    phase Fpp.ToCpp.Phases.instances """
    Nucleo::LedBlinker ledBlinker(FW_OPTIONAL_NAME("ledBlinker"));
    """

  }
  
  instance hardwareRateDriver: Nucleo.HardwareRateDriver base id 2000 \
  {

    phase Fpp.ToCpp.Phases.configConstants """
    enum {
      INTERVAL_MS = 100
    };
    """

    phase Fpp.ToCpp.Phases.instances """
    Nucleo::HardwareRateDriver hardwareRateDriver(
        FW_OPTIONAL_NAME("hardwareRateDriver"),
        ConfigConstants::hardwareRateDriver::INTERVAL_MS,
    );
    """

  }
  
  instance comm: Nucleo.SerialDriver base id 2000 \
  {

    phase Fpp.ToCpp.Phases.configConstants """
    enum {
      PORT_NUM = 0
    };
    """

    phase Fpp.ToCpp.Phases.instances """
    Nucleo::SerialDriver comm(
        FW_OPTIONAL_NAME("comm"),
        ConfigConstants::comm::PORT_NUM,
    );
    """

  }
  
  instance downlink: Svc.Framer base id 0x4100 {

    phase Fpp.ToCpp.Phases.configObjects """
    Svc::FprimeFraming framing;
    """

    phase Fpp.ToCpp.Phases.configComponents """
    downlink.setup(ConfigObjects::downlink::framing);
    """

  }


}

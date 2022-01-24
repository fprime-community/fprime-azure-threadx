# ======================================================================
# AcConstants.fpp
# Nucleo Blink configuration constants
# ======================================================================

@ Number of rate group member output ports for ActiveRateGroup
constant ActiveRateGroupOutputPorts = 4

@ Used to drive rate groups
constant RateGroupDriverRateGroupPorts = 2

@ Used for command and registration ports
constant CmdDispatcherComponentCommandPorts = 10

@ Used for uplink/sequencer buffer/response ports
constant CmdDispatcherSequencePorts = 1

@ Outputs from the generic repeater
constant GenericRepeaterOutputPorts = 2

@ Number of static memory allocations
constant StaticMemoryAllocations = 4

@ Used to ping active components
constant HealthPingPorts = 10

@ Used for broadcasting completed file downlinks
constant FileDownCompletePorts = 1

# ----------------------------------------------------------------------
# Hub connections. Connections on all deployments should mirror these settings.
# ----------------------------------------------------------------------

constant GenericHubInputPorts = 10
constant GenericHubOutputPorts = 10
constant GenericHubInputBuffers = 10
constant GenericHubOutputBuffers = 10

#include <Arduino.h>
#include <openevse.h>

#ifndef DEBUG_PORT
#define DEBUG_PORT Serial
#endif

#ifndef RAPI_PORT
#define RAPI_PORT Serial1
#endif

#define POLL_TIME (5 * 1000)

RapiSender rapiSender(&RAPI_PORT);

uint32_t next_status = 0;

const char *get_state_name(uint8_t state)
{
  const char *estate = "";
  switch (state) {
    case OPENEVSE_STATE_STARTING:
      estate = "Starting";
      break;
    case OPENEVSE_STATE_NOT_CONNECTED:
      estate = "Not Connected";
      break;
    case OPENEVSE_STATE_CONNECTED:
      estate = "EV Connected";
      break;
    case OPENEVSE_STATE_CHARGING:
      estate = "Charging";
      break;
    case OPENEVSE_STATE_VENT_REQUIRED:
      estate = "Vent Required";
      break;
    case OPENEVSE_STATE_DIODE_CHECK_FAILED:
      estate = "Diode Check Failed";
      break;
    case OPENEVSE_STATE_GFI_FAULT:
      estate = "GFCI Fault";
      break;
    case OPENEVSE_STATE_NO_EARTH_GROUND:
      estate = "No Earth Ground";
      break;
    case OPENEVSE_STATE_STUCK_RELAY:
      estate = "Stuck Relay";
      break;
    case OPENEVSE_STATE_GFI_SELF_TEST_FAILED:
      estate = "GFCI Self Test Failed";
      break;
    case OPENEVSE_STATE_OVER_TEMPERATURE:
      estate = "Over Temperature";
      break;
    case OPENEVSE_STATE_OVER_CURRENT:
      estate = "Over Current";
      break;
    case OPENEVSE_STATE_SLEEPING:
      estate = "Sleeping";
      break;
    case OPENEVSE_STATE_DISABLED:
      estate = "Disabled";
      break;
    default:
      estate = "Invalid";
      break;

  }

  return estate;
}

void setup()
{
  RAPI_PORT.begin(115200);
  DEBUG_PORT.begin(115200);

  DEBUG_PORT.println("");
  DEBUG_PORT.println("OpenEVSE");
  DEBUG_PORT.println("");
}

void loop()
{
  rapiSender.loop();

  if(millis() > next_status)
  {
    next_status = millis() + POLL_TIME;
    if(OpenEVSE.isConnected())
    {
      OpenEVSE.getStatus([](int ret, uint8_t evse_state, uint32_t session_time, uint8_t pilot_state, uint32_t vflags)
      {
        if(RAPI_RESPONSE_OK == ret)
        {
          DEBUG_PORT.printf("evse_state = %02x, session_time = %d, pilot_state = %02x, vflags = %08x\n", evse_state, session_time, pilot_state, vflags);
          DEBUG_PORT.printf("EVSE state: %s\n", get_state_name(evse_state));
          DEBUG_PORT.printf("Pilot state: %s\n", get_state_name(pilot_state));
        }
      });
    }
    else
    {
      OpenEVSE.begin(rapiSender, [](bool connected)
      {
        if(connected)
        {
          DEBUG_PORT.printf("Connected to OpenEVSE\n");
        } else {
          DEBUG_PORT.println("OpenEVSE not responding or not connected");
        }
      });
    }
  }
}

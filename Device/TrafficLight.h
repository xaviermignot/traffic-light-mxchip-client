#ifndef __TRAFFIC_LIGHT_H__
#define __TRAFFIC_LIGHT_H__

#include <iothub_client_core_common.h>
#include <WString.h>

enum TrafficLightState
{
    Off,
    Green,
    Orange,
    Red
};

enum TrafficLightMode
{
    Static,
    Flashing
};

TrafficLightState ParseState(const char *state);
String StateToString(TrafficLightState state);
TrafficLightState GetFromDeviceTwin(char *deviceTwin, DEVICE_TWIN_UPDATE_STATE updateState);

class TrafficLight
{
public:
    TrafficLight();
    TrafficLightState CurrentState;
    TrafficLightMode CurrentMode;
    void MoveToNextState();
    void ApplyMode();
};

#endif
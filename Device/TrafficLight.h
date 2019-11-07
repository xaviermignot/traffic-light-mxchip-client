#ifndef __TRAFFIC_LIGHT_H__
#define __TRAFFIC_LIGHT_H__

#include <WString.h>

enum TrafficLightState
{
    Off,
    Green,
    Orange,
    Red
};

TrafficLightState ParseState(const char *state);
String StateToString(TrafficLightState state);
TrafficLightState GetFromDeviceTwin(char *deviceTwin);

class TrafficLight
{
public:
    TrafficLight();
    TrafficLightState CurrentState;
    void MoveToNextState();
};

#endif
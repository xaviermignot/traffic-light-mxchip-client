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

enum TrafficLightMode
{
    Static,
    Flashing
};

TrafficLightState ParseState(const char *state);
String StateToString(TrafficLightState state);

class TrafficLight
{
public:
    TrafficLight();
    TrafficLightState CurrentState;
    TrafficLightMode CurrentMode;
    void MoveToNextState();
    void ChangeMode();
    void ApplyMode();
    bool ApplyDeviceTwin(JSON_Object *desiredTwin);
};

#endif
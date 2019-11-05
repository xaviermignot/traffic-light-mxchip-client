#include "TrafficLight.h"

TrafficLightState ParseState(const char *state)
{
    if (strcmp(state, "Green") == 0)
    {
        return Green;
    }
    if (strcmp(state, "Red") == 0)
    {
        return Red;
    }
    if (strcmp(state, "Orange") == 0)
    {
        return Orange;
    }
    return Off;
}

String StateToString(TrafficLightState state)
{
    switch (state)
    {
    case Off:
        return "Off";
    case Red:
        return "Red";
    case Orange:
        return "Orange";
    case Green:
        return "Green";
    }
}

TrafficLight::TrafficLight()
{
    CurrentState = Off;
}

void TrafficLight::MoveToNextState()
{
    switch (CurrentState)
    {
    case Off:
        CurrentState = Red;
        break;
    case Red:
        CurrentState = Orange;
        break;
    case Orange:
        CurrentState = Green;
        break;
    case Green:
        CurrentState = Off;
        break;
    }
}
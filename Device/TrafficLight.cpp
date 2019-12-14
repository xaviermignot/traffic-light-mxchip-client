#include <DevKitMQTTClient.h>
#include <parson.h>
#include <string.h>
#include <WString.h>

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

TrafficLightMode ParseMode(const char *mode)
{
    if (strcmp(mode, "Flashing") == 0)
    {
        return Flashing;
    }
    return Static;
}

String ModeToString(TrafficLightMode mode)
{
    switch (mode)
    {
    case Flashing:
        return "Flashing";
    case Static:
        return "Static";
    }
}

TrafficLight::TrafficLight()
{
    CurrentState = Off;
    CurrentMode = Static;
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

void TrafficLight::ChangeMode()
{
    switch (CurrentMode)
    {
    case Flashing:
        CurrentMode = Static;
        break;
    case Static:
        CurrentMode = Flashing;
        break;
    }
}

void TrafficLight::ApplyMode()
{
    if (CurrentMode == Flashing)
    {
        MoveToNextState();
    }
}

bool TrafficLight::ApplyDeviceTwin(JSON_Object *desiredTwin)
{
    if (desiredTwin == NULL)
    {
        return false;
    }

    const char *desiredStateJson = json_object_get_string(desiredTwin, "state");
    bool hasChanged = false;
    if (desiredStateJson != NULL)
    {
        TrafficLightState desiredState = ParseState(desiredStateJson);
        if (desiredState != CurrentState)
        {
            CurrentState = desiredState;
            hasChanged = true;
        }
    }

    const char *desiredModeJson = json_object_get_string(desiredTwin, "mode");
    if (desiredModeJson != NULL)
    {
        TrafficLightMode desiredMode = ParseMode(desiredModeJson);
        if (desiredMode != CurrentMode)
        {
            CurrentMode = desiredMode;
            hasChanged = true;
        }
    }

    return hasChanged;
}
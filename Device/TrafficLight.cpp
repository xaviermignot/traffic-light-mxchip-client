#include <DevKitMQTTClient.h>
#include <iothub_client_core_common.h>
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

void TrafficLight::ApplyMode()
{
    if (CurrentMode == Flashing)
    {
        MoveToNextState();
    }
}

TrafficLightState GetFromDeviceTwin(char *deviceTwin, DEVICE_TWIN_UPDATE_STATE updateState)
{
    JSON_Value *root_value;
    root_value = json_parse_string(deviceTwin);
    if (json_value_get_type(root_value) != JSONObject)
    {
        if (root_value != NULL)
        {
            json_value_free(root_value);
        }
        return Off;
    }

    JSON_Object *root_object = json_value_get_object(root_value);
    JSON_Object *desiredTrafficLight;

    if (updateState == DEVICE_TWIN_UPDATE_STATE::DEVICE_TWIN_UPDATE_COMPLETE)
    {
        JSON_Object *desired_object = json_object_get_object(root_object, "desired");
        if (desired_object != NULL)
        {
            desiredTrafficLight = json_object_dotget_object(desired_object, "trafficLight");
        }
    }
    else
    {
        desiredTrafficLight = json_object_get_object(root_object, "trafficLight");
    }

    json_value_free(root_value);
    const char *desiredLight = json_object_get_string(desiredTrafficLight, "state");
    return ParseState(desiredLight);
}
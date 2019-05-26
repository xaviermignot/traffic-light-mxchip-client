#include "DevKitMQTTClient.h"
#include "IoT_DevKit_HW.h"
#include "parson.h"

static bool hasWifi = false;
static bool hasIotHub = false;

static bool hasBeenInitializedWithDeviceTwin = false;

enum TrafficLightState
{
  Off,
  Green,
  Orange,
  Red
};

static TrafficLightState currentLightState = Off;
static bool lightStateChanged = false;

void setup()
{
  Screen.init();
  Screen.print("Traffic light !");

  InitWifi();
  if (!hasWifi)
  {
    return;
  }

  InitIotHub();
  if (!hasIotHub)
  {
    return;
  }
}

void loop()
{
  if (!hasWifi || !hasIotHub)
  {
    Screen.print(3, "Hit reset :-/");
    return;
  }

  checkButtons();

  DevKitMQTTClient_Check();

  printTrafficLightState(currentLightState);

  delay(100);
}

static void InitWifi()
{
  Screen.print(1, "Wifi...");

  hasWifi = initIoTDevKit(false) == 0;
  if (hasWifi)
  {
    Screen.print(1, "Wifi Ok !");
  }
  else
  {
    Screen.print(1, "Wifi Ko :(");
  }
}

static void InitIotHub()
{
  Screen.print(2, "IoT Hub...");
  hasIotHub = DevKitMQTTClient_Init(true);
  if (hasIotHub)
  {
    DevKitMQTTClient_SetDeviceTwinCallback(DeviceTwinCallBack);
    Screen.print(2, "IoT Hub Ok !");
  }
  else
  {
    Screen.print(2, "IoT Hub Ko :(");
  }
}

static void DeviceTwinCallBack(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, int length)
{
  char *temp = (char *)malloc(length + 1);
  if (temp == NULL)
  {
    return;
  }

  memcpy(temp, payLoad, length);
  temp[length] = '\0';
  JSON_Object *desiredTrafficLight = getTrafficLightFromDeviceTwin(updateState, temp);

  const char *desiredLight = json_object_get_string(desiredTrafficLight, "state");
  const TrafficLightState desiredLightState = parseState(desiredLight);
  if (desiredLightState != currentLightState)
  {
    currentLightState = desiredLightState;
    if (!hasBeenInitializedWithDeviceTwin)
    {
      Serial.println(F("First Device Twin callback"));
      hasBeenInitializedWithDeviceTwin = true;
    }
    else
    {
      // TODO: Fix update of reported state on twin callback
      // Serial.println(F("Device Twin callback, report state..."));
      // reportState();
    }
  }

  free(temp);
}

JSON_Object *getTrafficLightFromDeviceTwin(DEVICE_TWIN_UPDATE_STATE updateState, const char *message)
{
  JSON_Value *root_value;
  root_value = json_parse_string(message);
  if (json_value_get_type(root_value) != JSONObject)
  {
    if (root_value != NULL)
    {
      json_value_free(root_value);
    }
    LogError("parse %s failed", message);
    return NULL;
  }
  JSON_Object *root_object = json_value_get_object(root_value);
  JSON_Object *traffic_light_object;

  if (updateState == DEVICE_TWIN_UPDATE_COMPLETE)
  {
    JSON_Object *desired_object = json_object_get_object(root_object, "desired");
    if (desired_object != NULL)
    {
      traffic_light_object = json_object_dotget_object(desired_object, "trafficLight");
    }
  }
  else
  {
    traffic_light_object = json_object_dotget_object(root_object, "trafficLight");
  }
  json_value_free(root_value);
  return traffic_light_object;
}

void checkButtons()
{
  if (getButtonBState())
  {
    if (!lightStateChanged)
    {
      currentLightState = getNextState();
      lightStateChanged = true;
      reportState();
    }
  }
  else
  {
    lightStateChanged = false;
  }
}

void reportState()
{
  String reportedTwin = "{\"trafficLight\": { \"state\": \"" + stateToString(currentLightState) + "\" } }";
  Serial.println(reportedTwin);
  
  if (DevKitMQTTClient_ReportState(reportedTwin.c_str()))
  {
    Serial.println(F("Reporting twin success !"));
  }
  else
  {
    Serial.println(F("Reporting twin failed :("));
  }
}

String stateToString(TrafficLightState state)
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

TrafficLightState getNextState()
{
  switch (currentLightState)
  {
  case Off:
    return Red;
  case Red:
    return Orange;
  case Orange:
    return Green;
  case Green:
    return Off;
  }
}

TrafficLightState parseState(const char *state)
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

void printTrafficLightState(TrafficLightState state)
{
  Screen.print(1, state == Red ? "Red: On" : "Red: Off");
  Screen.print(2, state == Orange ? "Orange: On" : "Orange: Off");
  Screen.print(3, state == Green ? "Green: On" : "Green: Off");
}
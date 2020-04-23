#include <AZ3166WiFi.h>
#include <DevKitMQTTClient.h>
#include <IoT_DevKit_HW.h>
#include <parson.h>

#include "TrafficLight.h"

static bool hasWifi = false;
static bool hasIotHub = false;

static bool hasBeenInitializedWithDeviceTwin = false;
static bool shouldReportTwin = false;
static bool shouldChangeMode = false;
static bool shouldChangeState = false;

const int userLedPin = 45;

static TrafficLight trafficLight;

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

  trafficLight.ApplyMode();
  printTrafficLightState();

  if (shouldReportTwin)
  {
    reportTwin();
    shouldReportTwin = false;
  }

  delay(100);
}

static void InitWifi()
{
  Screen.print(1, "Wifi...");

  hasWifi = WiFi.begin() == WL_CONNECTED;
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
    DevKitMQTTClient_SetDeviceMethodCallback(DeviceMethodCallback);
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

  JSON_Object *trafficLightJson = ExtractTrafficLightFromTwin(temp, updateState);
  if (trafficLight.ApplyDeviceTwin(trafficLightJson))
  {
    if (!hasBeenInitializedWithDeviceTwin)
    {
      Serial.println(F("First Device Twin callback"));
      hasBeenInitializedWithDeviceTwin = true;
    }
    else
    {
      Serial.println(F("Device Twin callback, report state on next loop..."));
      shouldReportTwin = true;
    }
  }

  free(temp);
}

static JSON_Object *ExtractTrafficLightFromTwin(char *deviceTwin, DEVICE_TWIN_UPDATE_STATE updateState)
{
  JSON_Value *root_value;
  root_value = json_parse_string(deviceTwin);
  if (json_value_get_type(root_value) != JSONObject)
  {
    if (root_value != NULL)
    {
      json_value_free(root_value);
    }
    return NULL;
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
  return desiredTrafficLight;
}

static int DeviceMethodCallback(const char *methodName, const unsigned char *payload, int size, unsigned char **response, int *response_size)
{
  const char *responseMessage = "\"Successfully invoke device method\"";
  int result = 200;

  if (strcmp(methodName, "start") == 0)
  {
    Serial.println(F("Turn user led on from direct method"));
    digitalWrite(userLedPin, HIGH);
  }
  else if (strcmp(methodName, "stop") == 0)
  {
    Serial.println(F("Turn user led off from direct method"));
    digitalWrite(userLedPin, LOW);
  }
  else
  {
    Serial.println(F("Unkown direct method call"));
    responseMessage = "\"No method found\"";
    result = 404;
  }

  *response_size = strlen(responseMessage) + 1;
  *response = (unsigned char *)strdup(responseMessage);

  return result;
}

void checkButtons()
{
  checkButtonA();
  checkButtonB();
}

void checkButtonA()
{
  if (getButtonAState())
  {
    if (!shouldChangeMode)
    {
      trafficLight.ChangeMode();
      shouldChangeMode = true;
      shouldReportTwin = true;
    }
  }
  else
  {
    shouldChangeMode = false;
  }
}

void checkButtonB()
{
  if (getButtonBState())
  {
    if (!shouldChangeState)
    {
      trafficLight.MoveToNextState();
      shouldChangeState = true;
      shouldReportTwin = true;
    }
  }
  else
  {
    shouldChangeState = false;
  }
}

void reportTwin()
{
  String reportedTwin = "{\"trafficLight\": { \"state\": \"" + StateToString(trafficLight.CurrentState) + "\", \"mode\": \"" + ModeToString(trafficLight.CurrentMode) + "\" } }";
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

void printTrafficLightState()
{
  Screen.print(1, trafficLight.CurrentState == Red ? "Red: On" : "Red: Off");
  Screen.print(2, trafficLight.CurrentState == Orange ? "Orange: On" : "Orange: Off");
  Screen.print(3, trafficLight.CurrentState == Green ? "Green: On" : "Green: Off");
}
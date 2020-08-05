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

const int redLightPin = 29;
const int orangeLightPin = 30;
const int greenLightPin = 31;

static TrafficLight trafficLight;

void setup()
{
  Screen.init();
  Screen.print("Traffic light !");

  InitPins();

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

static void InitPins()
{
  pinMode(redLightPin, OUTPUT);
  pinMode(orangeLightPin, OUTPUT);
  pinMode(greenLightPin, OUTPUT);
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
    turnOnUserLED();
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

  JSON_Object *trafficLightJson;
  if (!hasBeenInitializedWithDeviceTwin)
  {
    // Device twin is retrieved after device boot, initialize using last reported twin
    Serial.println(F("First Device Twin callback"));
    trafficLightJson = ExtractTrafficLightFromTwin(temp, updateState, true);
    trafficLight.ApplyDeviceTwin(trafficLightJson);
    hasBeenInitializedWithDeviceTwin = true;
    turnOffUserLED();
  }
  else
  {
    trafficLightJson = ExtractTrafficLightFromTwin(temp, updateState, false);
    if (trafficLight.ApplyDeviceTwin(trafficLightJson))
    {
      // Change from desired twin update has been applied, report the change
      Serial.println(F("Device Twin callback, report state on next loop..."));
      indicateTwinToReport();
    }
  }

  free(temp);
}

static JSON_Object *ExtractTrafficLightFromTwin(char *deviceTwin, DEVICE_TWIN_UPDATE_STATE updateState, bool useReported)
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
  JSON_Object *trafficLightJson;

  if (updateState == DEVICE_TWIN_UPDATE_STATE::DEVICE_TWIN_UPDATE_COMPLETE)
  {
    JSON_Object *desired_object = json_object_get_object(root_object, useReported ? "reported" : "desired");
    if (desired_object != NULL)
    {
      trafficLightJson = json_object_dotget_object(desired_object, "trafficLight");
    }
  }
  else
  {
    trafficLightJson = json_object_get_object(root_object, "trafficLight");
  }

  json_value_free(root_value);
  return trafficLightJson;
}

static int DeviceMethodCallback(const char *methodName, const unsigned char *payload, int size, unsigned char **response, int *response_size)
{
  const char *responseMessage = "\"Successfully invoke device method\"";
  int result = 200;

  if (strcmp(methodName, "flash") == 0)
  {
    Serial.println(F("flash direct method invoked"));
    DoFlash();
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

static void DoFlash()
{
  TrafficLightState currentState = trafficLight.CurrentState;
  for (int i = 0; i < 5; i++)
  {
    trafficLight.CurrentState = currentState == Off ? All : Off;
    printTrafficLightState();
    delay(500);
    trafficLight.CurrentState = currentState;
    printTrafficLightState();
    delay(500);
  }
}

void indicateTwinToReport()
{
  shouldReportTwin = true;
  turnOnUserLED();
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
      indicateTwinToReport();
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
      indicateTwinToReport();
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
    turnOffUserLED();
  }
  else
  {
    Serial.println(F("Reporting twin failed :("));
  }
}

void printTrafficLightState()
{
  printLight(Red, redLightPin, 1);
  printLight(Orange, orangeLightPin, 2);
  printLight(Green, greenLightPin, 3);
}

void printLight(TrafficLightState lightToCheck, const int pinNumber, unsigned int lineNumber)
{
  bool lightValue = trafficLight.CurrentState == lightToCheck || trafficLight.CurrentState == All;
  digitalWrite(pinNumber, lightValue ? HIGH : LOW);
  char buffer[20];
  sprintf(buffer, "%s: %s", StateToString(lightToCheck).c_str(), lightValue ? "On" : "Off");
  Screen.print(lineNumber, buffer);
}
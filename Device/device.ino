#include <AZ3166WiFi.h>
#include <DevKitMQTTClient.h>
#include <IoT_DevKit_HW.h>
#include <parson.h>

#include "TrafficLight.h"

static bool hasWifi = false;
static bool hasIotHub = false;

static bool hasBeenInitializedWithDeviceTwin = false;
static bool shouldReportState = false;
static bool lightStateChanged = false;

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

  printTrafficLightState();

  if (shouldReportState)
  {
    reportState();
    shouldReportState = false;
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

  TrafficLightState desiredLightState = GetFromDeviceTwin(temp);

  if (desiredLightState != trafficLight.CurrentState)
  {
    trafficLight.CurrentState = desiredLightState;
    if (!hasBeenInitializedWithDeviceTwin)
    {
      Serial.println(F("First Device Twin callback"));
      hasBeenInitializedWithDeviceTwin = true;
    }
    else
    {
      Serial.println(F("Device Twin callback, report state on next loop..."));
      shouldReportState = true;
    }
  }

  free(temp);
}

void checkButtons()
{
  if (getButtonBState())
  {
    if (!lightStateChanged)
    {
      trafficLight.MoveToNextState();
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
  String reportedTwin = "{\"trafficLight\": { \"state\": \"" + StateToString(trafficLight.CurrentState) + "\" } }";
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
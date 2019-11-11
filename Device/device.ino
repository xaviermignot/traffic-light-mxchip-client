#include <Arduino.h>
#include <AZ3166WiFi.h>
#include <AzureIotHub.h>
#include <DevKitMQTTClient.h>
#include <iothub_client_core_common.h>
#include <IoT_DevKit_HW.h>
#include <stdlib.h>
#include <wiring.h>

#include "DevKitInit.h"
#include "TrafficLight.h"

static bool hasBeenInitializedWithDeviceTwin = false;

static bool shouldReportState = false;
static bool lightStateChanged = false;

TrafficLight trafficLight;
DevKitInit devKitInit;

void setup()
{
  devKitInit.Begin("Traffic light !");

  devKitInit.InitWifi();
  if (!devKitInit.HasWifi)
  {
    return;
  }

  devKitInit.InitIotHub();

  if (devKitInit.HasIotHub)
  {
    // DevKitMQTTClient_SetDeviceTwinCallback(DeviceTwinCallBack);
  }
  // devKitInit.InitIotHub(DeviceTwinCallBack);
}

void loop()
{
  if (!devKitInit.HasWifi || !devKitInit.HasIotHub)
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

static void DeviceTwinCallBack(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, int length)
{
  char *temp = (char *)malloc(length + 1);
  if (temp == NULL)
  {
    return;
  }

  memcpy(temp, payLoad, length);
  temp[length] = '\0';

  TrafficLightState desiredLightState = GetFromDeviceTwin(temp, updateState == DEVICE_TWIN_UPDATE_STATE::DEVICE_TWIN_UPDATE_COMPLETE);

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
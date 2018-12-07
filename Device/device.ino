#include "AZ3166WiFi.h"
#include "DevKitMQTTClient.h"

static bool hasWifi = false;
static bool hasIotHub = false;

enum TrafficLightState
{
  Off,
  Green,
  Orange,
  Red
};

void initWifi()
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

void initIotHub()
{
  Screen.print(2, "IoT Hub...");
  hasIotHub = DevKitMQTTClient_Init(true);
  if (hasIotHub)
  {
    Screen.print(2, "IoT Hub Ok !");
  }
  else
  {
    Screen.print(2, "IoT Hub Ko :(");
  }
}

void setup()
{
  Screen.print("Traffic light !");

  initWifi();
  if (!hasWifi)
  {
    return;
  }

  initIotHub();
  if (!hasIotHub)
  {
    return;
  }
}

void printTrafficLightState(TrafficLightState state)
{
  Screen.print(1, state == Red ? "Red: On" : "Red : Off");
  Screen.print(2, state == Orange ? "Orange: On" : "Orange : Off");
  Screen.print(3, state == Green ? "Green: On" : "Green : Off");
}

void loop()
{
  if (!hasWifi || !hasIotHub)
  {
    Screen.print(3, "Hit reset :-/");
    return;
  }

  printTrafficLightState(Off);
}

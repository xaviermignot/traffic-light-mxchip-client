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

static TrafficLightState currentLightState = Off;

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

  memcpy(temp, payLoad, length);
  temp[length] = '\0';
  parseTwinMessage(temp);
  free(temp);
}

void parseTwinMessage(const char *message)
{
  //TODO: Parse twins and take action
}

void setup()
{
  Screen.init();
  Screen.print("Traffic light !");

  pinMode(USER_BUTTON_A, INPUT);
  pinMode(USER_BUTTON_B, INPUT);

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

void printTrafficLightState(TrafficLightState state)
{
  Screen.print(1, state == Red ? "Red: On" : "Red: Off");
  Screen.print(2, state == Orange ? "Orange: On" : "Orange: Off");
  Screen.print(3, state == Green ? "Green: On" : "Green: Off");
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

void loop()
{
  if (!hasWifi || !hasIotHub)
  {
    Screen.print(3, "Hit reset :-/");
    return;
  }

  if (digitalRead(USER_BUTTON_B) == LOW)
  {
    currentLightState = getNextState();
  }

  DevKitMQTTClient_Check();

  printTrafficLightState(currentLightState);

  delay(100);
}

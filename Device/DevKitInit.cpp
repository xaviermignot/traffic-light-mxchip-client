// #include <Arduino.h>
#include <AZ3166WiFi.h>
#include <AzureIotHub.h>
#include <DevKitMQTTClient.h>
#include "DevKitInit.h"

DevKitInit::DevKitInit()
{
    HasWifi = false;
    HasIotHub = false;
}

void DevKitInit::Begin(const char *welcomeMsg)
{
    Screen.init();
    Screen.print(welcomeMsg);
}

void DevKitInit::InitWifi()
{
    Screen.print(1, "Wifi...");

    HasWifi = WiFi.begin() == WL_CONNECTED;
    if (HasWifi)
    {
        Screen.print(1, "Wifi Ok !");
    }
    else
    {
        Screen.print(1, "Wifi Ko :(");
    }
}

void DevKitInit::InitIotHub()
{
    Screen.print(2, "IoT Hub...");
    HasIotHub = DevKitMQTTClient_Init(true);
    if (HasIotHub)
    {
        Screen.print(2, "IoT Hub Ok !");
    }
    else
    {
        Screen.print(2, "IoT Hub Ko :(");
    }
}

// void DevKitInit::InitIotHub(DEVICE_TWIN_CALLBACK deviceTwinCallBack)
// {
//     Screen.print(2, "IoT Hub...");
//     HasIotHub = DevKitMQTTClient_Init(true);
//     if (HasIotHub)
//     {
//         DevKitMQTTClient_SetDeviceTwinCallback(deviceTwinCallBack);
//         Screen.print(2, "IoT Hub Ok !");
//     }
//     else
//     {
//         Screen.print(2, "IoT Hub Ko :(");
//     }
// }
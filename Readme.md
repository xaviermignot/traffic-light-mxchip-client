# Traffic Light MXChip Client


## What is this thing ?

This is an implementation of a traffic light for the MXChip Azure IoT DevKit (see dedicated website [here](https://microsoft.github.io/azure-iot-developer-kit/)).  
Why a traffic light ? Well, blinking a light is the *Hello World* of IoT, so 3 lights is just the next level. And I needed a pet project to experiment with the DevKit and with the Azure IoT platform, so here it is !


## How to get started ?

If you want to run this, you need to buy a DevKit first, and follow the get started guide [here](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-arduino-iot-devkit-az3166-get-started). The project can't work in the [simulator tool](https://azure-samples.github.io/iot-devkit-web-simulator/) as it doesn't allow to create additional files.  
Once you have everything set-up, just follow this simple steps:
1. Clone the repo
2. Open the `TrafficLight.code-workspace` file a a workspace in VS Code
3. Plug your DevKit to your computer (it's already plugged right ?)
4. Build & upload the code to the board (use the *Azure IoT Device Workbench: Upload Device Code* command in VS Code for that)
5. VS Code will build and deploy the code to the device just like it did with the *GetStarted* project of the tutorial
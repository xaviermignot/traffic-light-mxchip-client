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


## What does this thing do ?

### The basics
Once the device is connected to the IoT Hub and the code is running, the screen displays the following lines:
```
Traffic Light !
Red: Off
Orange: Off
Green: Off
```
The status of each light can then change from `Off` to `On` depending on the state of the traffic light.  

### Reacting to device twin changes
The *desired* and *reported* sections of the device twin of the device contain a `trafficLight` object like this one :
```json
"trafficLight": {
    "state": "Off",
    "mode": "Static"
}
```
The device can use a different *mode*, depending on the value of the `mode` property:
- With the `Static` mode, the lights don't change, the value of the `state` property is applied
- With the `Flashing` mode, the lights are constantly moving from `Red` to `Orange` to `Green` to `Off`, again and again and again. The `state` property is ignored in this mode.

Updating the *desired* section of the device twin from the cloud will reflect the changes from the device. The device also *reports* all changes by updating the *reported* section of the device twin.  

If you are not familiar with the concept of device twin in the IoT Hub, [this page](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-device-twins) is a must read.

### Invoking direct methods
The device will respond to call to the `flash` direct method by making the already lit lights flash for 5 seconds.

Once again if you're not familiar with direct methods of Azure IoT Hub, [read this](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-direct-methods).

### Pushing buttons !
A push on the *A* button of the device will change its `mode`, from `static` to `flashing`.  
A push on the *B* button of the device will change the lights, from `Off` to `All` through all single light values.  
All changes initiated from a button push are *reported* to the IoT Hub through the *reported* section of the device twin.


## How is the code organized ?

### The files
I have tried to make the code as tidy as possible, I didn't want to put everything in a huge *device.ino* file even if it's *just* a pet project.  

The ultimate goal is to put all the "business" (lol) code in the `TrafficLight` class and keep only the boilerplate code in the *device.ino* file. But as C is not my main language I had hard time trying to move some parts of the code outside of the *device.ino* file, especially the *json* parsing stuff.

### Boot sequence
When the device boots, the `init` method initiates the pins (which is instant), then the Wifi (takes some time), and finally the connection to the IoT Hub (take some more time).  The devkit screen displays messages in accordance of this sequence.  
If everything is fine, the `loop` method is repeatedly invoked to check for device twin changes, direct method invocations or button pushed events.

## So what's next ?

Here is a list of improvements and things to try for this project:
- Use of Azure IoT DPS instead of manual configuration
- Support for firmware updates
- Interaction with Azure Digital Twins V2 (there is a *[DTDL](https://github.com/Azure/opendigitaltwins-dtdl/blob/master/DTDL/v2/dtdlv2.md)* model already, that's a first step)
- Imagine how a *user* could *claim* the device, and associate it to his/her account (that's always a challenging thing in IoT projects)
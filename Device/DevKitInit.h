#ifndef __DEV_KIT_INIT_H__
#define __DEV_KIT_INIT_H__

// #include <AzureIotHub.h>

class DevKitInit
{
public:
    DevKitInit();
    bool HasWifi;
    bool HasIotHub;
    void Begin(const char *welcomeMsg);
    void InitWifi();
    void InitIotHub();
    // void InitIotHub(DEVICE_TWIN_CALLBACK deviceTwinCallBack);
};

#endif
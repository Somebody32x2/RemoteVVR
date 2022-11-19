#include <DeviceProvider.h>
//#include "HMDDriver.h"

EVRInitError DeviceProvider::Init(IVRDriverContext* pDriverContext)
{
    EVRInitError initError = InitServerDriverContext(pDriverContext);
    if (initError != EVRInitError::VRInitError_None)
    {
        return initError;
    }
    
    VRDriverLog()->Log("Initializing example controller"); //this is how you log out Steam's log file.

    controllerDriver = new ControllerDriver();
    VRServerDriverHost()->TrackedDeviceAdded("example_controller",
                                             TrackedDeviceClass_Controller,
                                             controllerDriver); //add all your devices like this.

//    hmdDriver = new HMDDriver();
//    VRServerDriverHost()->TrackedDeviceAdded("example_hmd",
//                                             TrackedDeviceClass_HMD,
//                                             hmdDriver);



    return vr::VRInitError_None;
}

void DeviceProvider::Cleanup()
{
    delete controllerDriver;
    controllerDriver = nullptr;

//    delete hmdDriver;
//    hmdDriver = nullptr;
}
const char* const* DeviceProvider::GetInterfaceVersions()
{
    return k_InterfaceVersions;
}

void DeviceProvider::RunFrame()
{
    controllerDriver->RunFrame();
//    hmdDriver->RunFrame();
}

bool DeviceProvider::ShouldBlockStandbyMode()
{
    return false;
}

void DeviceProvider::EnterStandby() {}

void DeviceProvider::LeaveStandby() {}
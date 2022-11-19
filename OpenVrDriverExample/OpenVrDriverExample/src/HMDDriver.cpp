//
// Created by samue on 11/11/2022.
//



#include <fstream>
#include <sstream>
#include "HMDDriver.h"


EVRInitError HMDDriver::Activate(uint32_t unObjectId) {

    driverId = unObjectId;
    VRDriverLog()->Log("-=-=-=-=-=-=-=-=-=-=-Activating HMD Driver!-=-=-=-=-=-=-=-=-=-=-");

    EVRSettingsError error;
    try {
        int window_x = VRSettings()->GetInt32("driver_example", "window_x", &error);
        if (window_x > 0)
            windowX = window_x;
    }
    catch (std::exception &e) {
        std::string logMessage = "Error getting window_x: " + std::string(e.what());
        VRDriverLog()->Log(logMessage.c_str());
    }; // Wrong type or doesn't exist

    try {

        int window_y = VRSettings()->GetInt32("driver_example", "window_y", &error);
        if (window_y > 0)
            windowY = window_y;
    }
    catch (std::exception &e) {
        std::string logMessage = "Error getting window_y: " + std::string(e.what());
        VRDriverLog()->Log(logMessage.c_str());
    }; // Wrong type or doesn't exist

    try {
        int window_width = VRSettings()->GetInt32("driver_example", "window_width", &error);
        if (window_width > 0)
            windowWidth = window_width;
    }
    catch (std::exception &e) {
        std::string logMessage = "Error getting window_width: " + std::string(e.what());
        VRDriverLog()->Log(logMessage.c_str());
    }; // Wrong type or doesn't exist

    try {
        int window_height = VRSettings()->GetInt32("driver_example", "window_height", &error);
        if (window_height > 0)
            windowHeight = window_height;
    }
    catch (std::exception &e) {
        std::string logMessage = "Error getting window_height: " + std::string(e.what());
        VRDriverLog()->Log(logMessage.c_str());
    }; // Wrong type or doesn't exist

    PropertyContainerHandle_t props = VRProperties()->TrackedDeviceToPropertyContainer(driverId);

    // Set some universe ID (Must be 2 or higher) SHOULD BE SAME AS CONTROLLER UNIVERSE ID
    VRProperties()->SetUint64Property(props, vr::Prop_CurrentUniverseId_Uint64, 2);

    // Set the IPD to be whatever steam has configured
    VRProperties()->SetFloatProperty(props, vr::Prop_UserIpdMeters_Float,
                                     vr::VRSettings()->GetFloat(vr::k_pch_SteamVR_Section,
                                                                vr::k_pch_SteamVR_IPD_Float));

    // Set the display FPS
    VRProperties()->SetFloatProperty(props, vr::Prop_DisplayFrequency_Float, 60.f);

    // Set up a model "number" (not needed but good to have)
    VRProperties()->SetStringProperty(props, vr::Prop_ModelNumber_String, "EXAMPLE_HMD_DEVICE");

    VRDriverLog()->Log("-=-=-=-=-=-=-=-=-=-=-HMD Driver Activated!-=-=-=-=-=-=-=-=-=-=-");
    return VRInitError_None;
}


void HMDDriver::RunFrame() {

//    VRDriverLog()->Log("-=-=-=-=-=-=-=-=-=-=-Running HMD Driver Frame!-=-=-=-=-=-=-=-=-=-=-");
    // Get the new inputs from the file
    try {
//    VRDriverLog()->Log("About to read file");

//    std::ifstream file("inputs.txt");
        // Open "C:\Users\samue\Desktop\Coding_Files\C OWO\RemoteVVR\finalDriverDir\example\bin\win64\inputs.txt"
        std::ifstream file(
                R"(C:\Users\samue\Desktop\Coding_Files\C OWO\RemoteVVR\finalDriverDir\example\bin\win64\HMDInputs.txt)");
//    Handle the file already being open
        if (!file.is_open()) {
            VRDriverLog()->Log("File not open");
            return;
        }
//     Get the first line, split it into 6 strings, and convert them to floats
        std::string line;
        std::getline(file, line); // Get the first line
        std::istringstream iss(line); // Create a string stream from the line
        std::string posXString, posYString, posZString,
                rotWString, rotXString, rotYString, rotZString,
                aString, bString, xString, yString, sysString; //strings to hold the values
        iss >> posXString >> posYString >> posZString
            >> rotWString >> rotXString >> rotYString >> rotZString; //split the line into strings


        // Save the position and rotation of the controller to nextPosition and nextRotation
        position[0] = std::stof(posXString);
        position[1] = std::stof(posYString);
        position[2] = std::stof(posZString);
        rotation[0] = std::stof(rotWString);
        rotation[1] = std::stof(rotXString);
        rotation[2] = std::stof(rotYString);
        rotation[3] = std::stof(rotZString);


//    nextPosition = { std::stof(poxXString), std::stof(posYString), std::stof(posZString)};


        // Log the values

//    VRDriverLog()->Log(line.c_str());
        // std::string logMessage =
        // "Joystick X: " + joystickXString + " Y: " + joystickYString + " Trackpad X: " + trackpadXString + " Y: " +
        // trackpadYString + " Trigger Value: " + triggerValueString + " Trigger Click: " + triggerClickString;

        // VRDriverLog()->Log(logMessage.c_str());

//     VRDriverLog()->Log(triggerValueString.c_str());

//     Close the file
        file.close();

        // VRDriverHost is act
        VRServerDriverHost()->TrackedDevicePoseUpdated(driverId, GetPose(), sizeof(DriverPose_t));
        VRDriverLog()->Log("-=-=-=-=-=-=-=-=-=-=-HMD Driver Frame Ran!-=-=-=-=-=-=-=-=-=-=-");

    }
    catch (std::exception &e) {
        VRDriverLog()->Log("Error reading file");
        VRDriverLog()->Log(e.what());
        VRDriverLog()->Log("-=-=-=-=-=-=-=-=-=-=-HMD Driver Frame Failed!-=-=-=-=-=-=-=-=-=-=-");
        return;




        // Update the pose
//        VRServerDriverHost()->TrackedDevicePoseUpdated(driverId, GetPose(), sizeof(DriverPose_t));
    }
}

DriverPose_t HMDDriver::GetPose() {

//    VRDriverLog()->Log("-=-=-=-=-=-=-=-=-=-=-Getting HMD Driver Pose!-=-=-=-=-=-=-=-=-=-=-");

    DriverPose_t pose = {0};
    pose.poseIsValid = true;
    pose.result = TrackingResult_Running_OK;
    pose.deviceIsConnected = true;

    HmdQuaternion_t quat;
    HmdQuaternion_t zeroQuat;
    quat.w = rotation[0];
    quat.x = rotation[1];
    quat.y = rotation[2];
    quat.z = rotation[3];

    zeroQuat.w = 1;
    zeroQuat.x = 0;
    zeroQuat.y = 0;
    zeroQuat.z = 0;

    // Set the position of the controller
    pose.vecPosition[0] = position[0];
    pose.vecPosition[1] = position[1];
    pose.vecPosition[2] = position[2];

    pose.qWorldFromDriverRotation = zeroQuat;
    pose.qDriverFromHeadRotation = zeroQuat;
    pose.qRotation = quat;

    // Log the position of the controller
//    std::string logString = "Controller Position: " + std::to_string(pose.vecPosition[0]) + ", " + std::to_string(pose.vecPosition[1]) + ", " + std::to_string(pose.vecPosition[2]);
//    VRDriverLog()->Log(logString.c_str());

//    VRDriverLog()->Log("-=-=-=-=-=-=-=-=-=-=-HMD Driver Pose Got!-=-=-=-=-=-=-=-=-=-=-");

    return pose;
}

void HMDDriver::Deactivate() {
    VRDriverLog()->Log("-=-=-=-=-=-=-=-=-=-=-Deactivating HMD Driver!-=-=-=-=-=-=-=-=-=-=-");
}

void HMDDriver::EnterStandby() {
    VRDriverLog()->Log("-=-=-=-=-=-=-=-=-=-=-Entering Standby!-=-=-=-=-=-=-=-=-=-=-");
}

void *HMDDriver::GetComponent(const char *pchComponentNameAndVersion) {
    if (!_stricmp(pchComponentNameAndVersion, vr::IVRDisplayComponent_Version)) {
        return static_cast<vr::IVRDisplayComponent *>(this);
    }
    return nullptr;

}

void HMDDriver::DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize) {
    if (unResponseBufferSize >= 1)
        pchResponseBuffer[0] = 0;
    VRDriverLog()->Log("-=-=-=-=-=-=-=-=-=-=-Debug Request!-=-=-=-=-=-=-=-=-=-=-");
}

void HMDDriver::GetWindowBounds(int32_t *pnX, int32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight) {

    *pnX = windowX;
    *pnY = windowY;
    *pnWidth = windowWidth;
    *pnHeight = windowHeight;
    VRDriverLog()->Log("-=-=-=-=-=-=-=-=-=-=-Getting Window Bounds!-=-=-=-=-=-=-=-=-=-=-");
}

bool HMDDriver::IsDisplayOnDesktop() {
    return true;
}

bool HMDDriver::IsDisplayRealDisplay() {
    return false;
}

void HMDDriver::GetRecommendedRenderTargetSize(uint32_t *pnWidth, uint32_t *pnHeight) {
    *pnWidth = windowWidth;
    *pnHeight = windowHeight;
}

void HMDDriver::GetEyeOutputViewport(vr::EVREye eEye, uint32_t *pnX, uint32_t *pnY, uint32_t *pnWidth,
                                     uint32_t *pnHeight) {
    *pnY = 0;
    *pnWidth = this->windowWidth / 2;
    *pnHeight = this->windowHeight;

    if (eEye == vr::EVREye::Eye_Left) {
        *pnX = 0;
    } else {
        *pnX = this->windowWidth / 2;
    }
}

void HMDDriver::GetProjectionRaw(vr::EVREye eEye, float *pfLeft, float *pfRight, float *pfTop, float *pfBottom) {
    *pfLeft = -1;
    *pfRight = 1;
    *pfTop = -1;
    *pfBottom = 1;
}

vr::DistortionCoordinates_t HMDDriver::ComputeDistortion(vr::EVREye eEye, float fU, float fV) {
    vr::DistortionCoordinates_t coordinates;
    coordinates.rfBlue[0] = fU;
    coordinates.rfBlue[1] = fV;
    coordinates.rfGreen[0] = fU;
    coordinates.rfGreen[1] = fV;
    coordinates.rfRed[0] = fU;
    coordinates.rfRed[1] = fV;
    return coordinates;

}





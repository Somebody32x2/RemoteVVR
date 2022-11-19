#include <ControllerDriver.h>
#include <sstream>
#include <fstream>
//#include "openvr.h"

EVRInitError ControllerDriver::Activate(uint32_t unObjectId) {
    driverId = unObjectId; //unique ID for your driver

    PropertyContainerHandle_t props = VRProperties()->TrackedDeviceToPropertyContainer(
            driverId); //this gets a container object where you store all the information about your driver

    VRProperties()->SetStringProperty(props, Prop_InputProfilePath_String,
                                      "{example}/input/controller_profile.json"); //tell OpenVR where to get your driver's Input Profile

    VRProperties()->SetInt32Property(props, Prop_ControllerRoleHint_Int32,
                                     ETrackedControllerRole::TrackedControllerRole_RightHand); //tells OpenVR what kind of device this is

    VRDriverInput()->CreateScalarComponent(props, "/input/joystick/y", &joystickYHandle,
                                           EVRScalarType::VRScalarType_Absolute,
                                           EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); //sets up handler you'll use to send joystick commands to OpenVR with, in the Y direction (forward/backward)
    VRDriverInput()->CreateScalarComponent(props, "/input/trackpad/y", &trackpadYHandle,
                                           EVRScalarType::VRScalarType_Absolute,
                                           EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); //sets up handler you'll use to send trackpad commands to OpenVR with, in the Y direction
    VRDriverInput()->CreateScalarComponent(props, "/input/joystick/x", &joystickXHandle,
                                           EVRScalarType::VRScalarType_Absolute,
                                           EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); //Why VRScalarType_Absolute? Take a look at the comments on EVRScalarType.
    VRDriverInput()->CreateScalarComponent(props, "/input/trackpad/x", &trackpadXHandle,
                                           EVRScalarType::VRScalarType_Absolute,
                                           EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); //Why VRScalarUnits_NormalizedTwoSided? Take a look at the comments on EVRScalarUnits.

//                                           Right Trigger
    VRDriverInput()->CreateScalarComponent(props, "/input/trigger/value", &triggerHandle,
                                           EVRScalarType::VRScalarType_Absolute,
                                           EVRScalarUnits::VRScalarUnits_NormalizedOneSided);
    VRDriverInput()->CreateBooleanComponent(props, "/input/trigger/click", &triggerClickHandle);
    // ABXY Buttons
    VRDriverInput()->CreateBooleanComponent(props, "/input/a/click", &aHandle);
    VRDriverInput()->CreateBooleanComponent(props, "/input/b/click", &bHandle);
    VRDriverInput()->CreateBooleanComponent(props, "/input/x/click", &xHandle);
    VRDriverInput()->CreateBooleanComponent(props, "/input/y/click", &yHandle);
    VRDriverInput()->CreateBooleanComponent(props, "/input/system/click", &systemHandle);

    //The following properites are ones I tried out because I saw them in other samples, but I found they were not needed to get the sample working.
    //There are many samples, take a look at the openvr_header.h file. You can try them out.

    //VRProperties()->SetUint64Property(props, Prop_CurrentUniverseId_Uint64, 2);
    //VRProperties()->SetBoolProperty(props, Prop_HasControllerComponent_Bool, true);
    //VRProperties()->SetBoolProperty(props, Prop_NeverTracked_Bool, true);
    //VRProperties()->SetInt32Property(props, Prop_Axis0Type_Int32, k_eControllerAxis_TrackPad);
    //VRProperties()->SetInt32Property(props, Prop_Axis2Type_Int32, k_eControllerAxis_Joystick);
    //VRProperties()->SetStringProperty(props, Prop_SerialNumber_String, "example_controler_serial");
    //VRProperties()->SetStringProperty(props, Prop_RenderModelName_String, "vr_controller_vive_1_5");
    //uint64_t availableButtons = ButtonMaskFromId(k_EButton_SteamVR_Touchpad) |
    //	ButtonMaskFromId(k_EButton_IndexController_JoyStick);
    //VRProperties()->SetUint64Property(props, Prop_SupportedButtons_Uint64, availableButtons);

    nextPosition[0] = 0;
    nextPosition[1] = 0;
    nextPosition[2] = 0;

    nextRotation[0] = 1;
    nextRotation[1] = 0;
    nextRotation[2] = 0;
    nextRotation[3] = 0;

    // Other Props                     ---------------------------------THIS MUST BE SAME AS IN HMD DRIVER---------------------------------
    VRProperties()->SetUint64Property(props, vr::Prop_CurrentUniverseId_Uint64, 2);
    VRProperties()->SetStringProperty(props, vr::Prop_ModelNumber_String, "example_controller");
    VRProperties()->SetStringProperty(props, vr::Prop_RenderModelName_String, "{example}example_controller");
//    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_ModelNumber_String, m_sModelNumber.c_str());
//    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_RenderModelName_String, m_sModelNumber.c_str());
//    RenderModel_t model;
//    LoadRenderModel("", model);


    return VRInitError_None;
}

DriverPose_t ControllerDriver::GetPose() {
    DriverPose_t pose = {0};
    pose.poseIsValid = true;
    pose.result = TrackingResult_Running_OK;
    pose.deviceIsConnected = true;

    HmdQuaternion_t quat;
    HmdQuaternion_t zeroQuat;
    quat.w = nextRotation[0];
    quat.x = nextRotation[1];
    quat.y = nextRotation[2];
    quat.z = nextRotation[3];

    zeroQuat.w = 1;
    zeroQuat.x = 0;
    zeroQuat.y = 0;
    zeroQuat.z = 0;

    // Set the position of the controller
    pose.vecPosition[0] = nextPosition[0];
    pose.vecPosition[1] = nextPosition[1];
    pose.vecPosition[2] = nextPosition[2];

    pose.qWorldFromDriverRotation = zeroQuat;
    pose.qDriverFromHeadRotation = zeroQuat;
    pose.qRotation = quat;

    // Log the position of the controller
//    std::string logString = "Controller Position: " + std::to_string(pose.vecPosition[0]) + ", " + std::to_string(pose.vecPosition[1]) + ", " + std::to_string(pose.vecPosition[2]);
//    VRDriverLog()->Log(logString.c_str());

    return pose;
}

void ControllerDriver::RunFrame() {
    // Read the file inputs.txt for the joystick and trackpad values - the format is: joystickX joystickY trackpadX trackpadY triggerValue triggerClick
    // The values are in the range -1 to 1 exept for triggerValue which is 0 to 1 and triggerClick which is 0 or 1


    try {
//    VRDriverLog()->Log("About to read file");

//    std::ifstream file("inputs.txt");
        // Open "C:\Users\samue\Desktop\Coding_Files\C OWO\RemoteVVR\finalDriverDir\example\bin\win64\inputs.txt"
        std::ifstream file(
                R"(C:\Users\samue\Desktop\Coding_Files\C OWO\RemoteVVR\finalDriverDir\example\bin\win64\inputs.txt)");
//    Handle the file already being open
        if (!file.is_open()) {
            VRDriverLog()->Log("File not open");
            return;
        }
//     Get the first line, split it into 6 strings, and convert them to floats
        std::string line;
        std::getline(file, line); // Get the first line
        std::istringstream iss(line); // Create a string stream from the line
        std::string joystickXString, joystickYString, trackpadXString, trackpadYString,
                triggerValueString, triggerClickString,
                poxXString, posYString, posZString,
                rotWString, rotXString, rotYString, rotZString,
                aString, bString, xString, yString, sysString; //strings to hold the values
        iss >> joystickXString >> joystickYString >> trackpadXString >> trackpadYString
            >> triggerValueString >> triggerClickString
            >> poxXString >> posYString >> posZString
            >> rotWString >> rotXString >> rotYString >> rotZString
            >> aString >> bString >> xString >> yString >> sysString; //split the line into strings

        float joystickX = std::stof(joystickXString);
        float joystickY = std::stof(joystickYString);
        float trackpadX = std::stof(trackpadXString);
        float trackpadY = std::stof(trackpadYString);
        float triggerValue = std::stof(triggerValueString);
        bool triggerClick = std::stoi(triggerClickString);
        bool a = std::stoi(aString);
        bool b = std::stoi(bString);
        bool x = std::stoi(xString);
        bool y = std::stoi(yString);
        bool sys = std::stoi(sysString);


        // Save the position and rotation of the controller to nextPosition and nextRotation
        nextPosition[0] = std::stof(poxXString);
        nextPosition[1] = std::stof(posYString);
        nextPosition[2] = std::stof(posZString);

        nextRotation[0] = std::stof(rotWString);
        nextRotation[1] = std::stof(rotXString);
        nextRotation[2] = std::stof(rotYString);
        nextRotation[3] = std::stof(rotZString);


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

//    Since we used VRScalarUnits_NormalizedTwoSided as the unit, the range is -1 to 1.
        VRDriverInput()->UpdateScalarComponent(joystickYHandle, joystickY, 0); //move forward
        VRDriverInput()->UpdateScalarComponent(trackpadYHandle, trackpadY, 0); //move foward
        VRDriverInput()->UpdateScalarComponent(joystickXHandle, joystickX, 0); //change the value to move sideways
        VRDriverInput()->UpdateScalarComponent(trackpadXHandle, trackpadX, 0); //change the value to move sideways
        VRDriverInput()->UpdateScalarComponent(triggerHandle, triggerValue, 0);
        VRDriverInput()->UpdateBooleanComponent(triggerClickHandle, triggerClick, 0);
        VRDriverInput()->UpdateBooleanComponent(aHandle, a, 0);
        VRDriverInput()->UpdateBooleanComponent(bHandle, b, 0);
        VRDriverInput()->UpdateBooleanComponent(xHandle, x, 0);
        VRDriverInput()->UpdateBooleanComponent(yHandle, y, 0);
        VRDriverInput()->UpdateBooleanComponent(systemHandle, sys, 0);

        // VRDriverHost is act
        VRServerDriverHost()->TrackedDevicePoseUpdated(driverId, GetPose(), sizeof(DriverPose_t));
    }
    catch (std::exception &e) {
        VRDriverLog()->Log("Error reading file");
        VRDriverLog()->Log(e.what());
        return;
    }


}

void ControllerDriver::Deactivate() {
    driverId = k_unTrackedDeviceIndexInvalid;
}

void *ControllerDriver::GetComponent(const char *pchComponentNameAndVersion) {
    //I found that if this method just returns null always, it works fine. But I'm leaving the if statement in since it doesn't hurt.
    //Check out the IVRDriverInput_Version declaration in openvr_driver.h. You can search that file for other _Version declarations
    //to see other components that are available. You could also put a log in this class and output the value passed into this
    //method to see what OpenVR is looking for.
    if (strcmp(IVRDriverInput_Version, pchComponentNameAndVersion) == 0) {
        return this;
    }
    return NULL;
}

void ControllerDriver::EnterStandby() {}

void ControllerDriver::DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize) {
    if (unResponseBufferSize >= 1) {
        pchResponseBuffer[0] = 0;
    }
}
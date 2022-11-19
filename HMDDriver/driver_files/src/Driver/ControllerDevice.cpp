#include "ControllerDevice.hpp"
#include <Windows.h>
#include <fstream>

ExampleDriver::ControllerDevice::ControllerDevice(std::string serial, ControllerDevice::Handedness handedness) :
        serial_(serial),
        handedness_(handedness) {
}

std::string ExampleDriver::ControllerDevice::GetSerial() {
    return this->serial_;
}

void ExampleDriver::ControllerDevice::Update() {
    if (this->device_index_ == vr::k_unTrackedDeviceIndexInvalid)
        return;
// region oldCode
//    // Check if this device was asked to be identified
//    auto events = GetDriver()->GetOpenVREvents();
//    for (auto event : events) {
//        // Note here, event.trackedDeviceIndex does not necissarily equal this->device_index_, not sure why, but the component handle will match so we can just use that instead
//        //if (event.trackedDeviceIndex == this->device_index_) {
//        if (event.eventType == vr::EVREventType::VREvent_Input_HapticVibration) {
//            if (event.data.hapticVibration.componentHandle == this->haptic_component_) {
//                this->did_vibrate_ = true;
//            }
//        }
//        //}
//    }
//
//    // Check if we need to keep vibrating
//    if (this->did_vibrate_) {
//        this->vibrate_anim_state_ += (GetDriver()->GetLastFrameTime().count()/1000.f);
//        if (this->vibrate_anim_state_ > 1.0f) {
//            this->did_vibrate_ = false;
//            this->vibrate_anim_state_ = 0.0f;
//        }
//    }
//
//    // Setup pose for this frame
//    auto pose = IVRDevice::MakeDefaultPose();
//
//    // Find a HMD
//    auto devices = GetDriver()->GetDevices();
//    auto hmd = std::find_if(devices.begin(), devices.end(), [](const std::shared_ptr<IVRDevice>& device_ptr) {return device_ptr->GetDeviceType() == DeviceType::HMD; });
//    if (hmd != devices.end()) {
//        // Found a HMD
//        vr::DriverPose_t hmd_pose = (*hmd)->GetPose();
//
//        // Here we setup some transforms so our controllers are offset from the headset by a small amount so we can see them
//        linalg::vec<float, 3> hmd_position{ (float)hmd_pose.vecPosition[0], (float)hmd_pose.vecPosition[1], (float)hmd_pose.vecPosition[2] };
//        linalg::vec<float, 4> hmd_rotation{ (float)hmd_pose.qRotation.x, (float)hmd_pose.qRotation.y, (float)hmd_pose.qRotation.z, (float)hmd_pose.qRotation.w };
//
//        // Do shaking animation if haptic vibration was requested
//        float controller_y = -0.2f + 0.01f * std::sinf(8 * 3.1415f * vibrate_anim_state_);
//
//        // Left hand controller on the left, right hand controller on the right, any other handedness sticks to the middle
//        float controller_x = this->handedness_ == Handedness::LEFT ? -0.2f : (this->handedness_ == Handedness::RIGHT ? 0.2f : 0.f);
//
//        linalg::vec<float, 3> hmd_pose_offset = { controller_x, controller_y, -0.5f };
//
//        hmd_pose_offset = linalg::qrot(hmd_rotation, hmd_pose_offset);
//
//        linalg::vec<float, 3> final_pose = hmd_pose_offset + hmd_position;
//
//        pose.vecPosition[0] = final_pose.x;
//        pose.vecPosition[1] = final_pose.y;
//        pose.vecPosition[2] = final_pose.z;
//
//        pose.qRotation.w = hmd_rotation.w;
//        pose.qRotation.x = hmd_rotation.x;
//        pose.qRotation.y = hmd_rotation.y;
//        pose.qRotation.z = hmd_rotation.z;
//    }
//
//    // Check if we need to press any buttons (I am only hooking up the A button here but the process is the same for the others)
//    // You will still need to go into the games button bindings and hook up each one (ie. a to left click, b to right click, etc.) for them to work properly
//    if (GetAsyncKeyState(0x45 /* E */) != 0) {
//        GetDriver()->GetInput()->UpdateBooleanComponent(this->a_button_click_component_, true, 0);
//        GetDriver()->GetInput()->UpdateBooleanComponent(this->a_button_touch_component_, true, 0);
//    }
//    else {
//        GetDriver()->GetInput()->UpdateBooleanComponent(this->a_button_click_component_, false, 0);
//        GetDriver()->GetInput()->UpdateBooleanComponent(this->a_button_touch_component_, false, 0);
//    }
// endregion

    try {
        // Read and process the appropriate input file
        std::ifstream input_file;
        if (this->handedness_ == Handedness::LEFT) {
            input_file.open(
                    R"(C:\Users\samue\Desktop\Coding_Files\C OWO\RemoteVVR\finalDriverDir\inputs\leftControllerInput.txt)");
        } else if (this->handedness_ == Handedness::RIGHT) {
            input_file.open(
                    R"(C:\Users\samue\Desktop\Coding_Files\C OWO\RemoteVVR\finalDriverDir\inputs\rightControllerInput.txt)");
        }
        // The format is as follows: touchpadX touchpadY touchpadClick joystickX joystickY joystickClick posX posY posZ rotX rotY rotZ rotW triggerVal triggerPressed gripVal gripForce a b x y system
        //                           float     float     bool          float    float      bool          floatfloatfloatfloatfloatfloatfloatfloat      bool           float   float     bool bool bool bool bool

        // Read the input line
        std::string input_line;
        std::getline(input_file, input_line);
        input_file.close();

        // Parse the input line

        std::stringstream input_stream(input_line);
        float touchpadX, touchpadY, joystickX, joystickY, posX, posY, posZ, rotX, rotY, rotZ, rotW, triggerVal, gripVal, gripForce;
        bool touchpadClick, joystickClick, triggerPressed, a, b, x, y, system;
        input_stream >> touchpadX >> touchpadY >> touchpadClick >> joystickX >> joystickY >> joystickClick >> posX
                     >> posY >> posZ >> rotX >> rotY >> rotZ >> rotW >> triggerVal >> triggerPressed >> gripVal
                     >> gripForce >> a >> b >> x >> y >> system;

        // Pass the input to the appropriate components
        GetDriver()->GetInput()->UpdateScalarComponent(this->trackpad_x_component_, touchpadX, 0);
        GetDriver()->GetInput()->UpdateScalarComponent(this->trackpad_y_component_, touchpadY, 0);
        GetDriver()->GetInput()->UpdateBooleanComponent(this->trackpad_click_component_, touchpadClick, 0);
        GetDriver()->GetInput()->UpdateScalarComponent(this->joystick_x_component_, joystickX, 0);
        GetDriver()->GetInput()->UpdateScalarComponent(this->joystick_y_component_, joystickY, 0);
        GetDriver()->GetInput()->UpdateBooleanComponent(this->joystick_click_component_, joystickClick, 0);
        GetDriver()->GetInput()->UpdateScalarComponent(this->trigger_value_component_, triggerVal, 0);
        GetDriver()->GetInput()->UpdateBooleanComponent(this->trigger_click_component_, triggerPressed, 0);
        GetDriver()->GetInput()->UpdateScalarComponent(this->grip_value_component_, gripVal, 0);
        GetDriver()->GetInput()->UpdateScalarComponent(this->grip_force_component_, gripForce, 0);
        GetDriver()->GetInput()->UpdateBooleanComponent(this->a_button_click_component_, a, 0);
        GetDriver()->GetInput()->UpdateBooleanComponent(this->b_button_click_component_, b, 0);
        GetDriver()->GetInput()->UpdateBooleanComponent(this->x_button_click_component_, x, 0);
        GetDriver()->GetInput()->UpdateBooleanComponent(this->y_button_click_component_, y, 0);
        GetDriver()->GetInput()->UpdateBooleanComponent(this->system_click_component_, system, 0);

        // Log the grip and controller hand
        if (gripVal > 0.5f) {
            GetDriver()->Log("grip pressed" + std::to_string(gripVal) + " " + std::to_string(gripForce));
        }

        // Update the pose
//        vr::DriverPose_t pose = {0};
//        pose.poseIsValid = true;
//        pose.result = vr::TrackingResult_Running_OK;
//        pose.deviceIsConnected = true;
//
//        // Initialize Quaternions qRotaton and zeroQuat (dont use linalg)
//        vr::HmdQuaternion_t qRotation = {0};
//        vr::HmdQuaternion_t zeroQuat = {0};
//
//
//        pose.qWorldFromDriverRotation = zeroQuat;
//        pose.qDriverFromHeadRotation = zeroQuat;
//
//
//        qRotation.w = rotW;
//        qRotation.x = rotX;
//        qRotation.y = rotY;
//        qRotation.z = rotZ;
//
//        pose.qRotation = qRotation;
//
//
//        pose.vecWorldFromDriverTranslation[0] = posX;
//        pose.vecWorldFromDriverTranslation[1] = posY;
//        pose.vecWorldFromDriverTranslation[2] = posZ;

        // If the pose has changed since last pose log
//        if (pose.vecWorldFromDriverTranslation != last_pose_.vecWorldFromDriverTranslation){
//            GetDriver()->Log("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-Controller Pose changed!-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-");
//        }

        nextPosition[0] = posX;
        nextPosition[1] = posY;
        nextPosition[2] = posZ;
        nextRotation[1] = rotX;
        nextRotation[2] = rotY;
        nextRotation[3] = rotZ;
        nextRotation[0] = rotW;

        // Post pose
        GetDriver()->GetDriverHost()->TrackedDevicePoseUpdated(this->device_index_, this->GetPose(), sizeof(vr::DriverPose_t));
//        this->last_pose_ = pose;


    }
    catch (std::exception &e) {
        GetDriver()->Log("Error reading file");
        GetDriver()->Log(e.what());
        return;
    }


}

DeviceType ExampleDriver::ControllerDevice::GetDeviceType() {
    return DeviceType::CONTROLLER;
}

ExampleDriver::ControllerDevice::Handedness ExampleDriver::ControllerDevice::GetHandedness() {
    return this->handedness_;
}

vr::TrackedDeviceIndex_t ExampleDriver::ControllerDevice::GetDeviceIndex() {
    return this->device_index_;
}

vr::EVRInitError ExampleDriver::ControllerDevice::Activate(uint32_t unObjectId) {
    this->device_index_ = unObjectId;

    GetDriver()->Log("Activating controller " + this->serial_);

    // Get the properties handle
    auto props = GetDriver()->GetProperties()->TrackedDeviceToPropertyContainer(this->device_index_);

    // Setup inputs and outputs
//    GetDriver()->GetInput()->CreateHapticComponent(props, "/output/haptic", &this->haptic_component_);

    GetDriver()->GetInput()->CreateBooleanComponent(props, "/input/a/click", &this->a_button_click_component_);
//    GetDriver()->GetInput()->CreateBooleanComponent(props, "/input/a/touch", &this->a_button_touch_component_);

    GetDriver()->GetInput()->CreateBooleanComponent(props, "/input/b/click", &this->b_button_click_component_);
//    GetDriver()->GetInput()->CreateBooleanComponent(props, "/input/b/touch", &this->b_button_touch_component_);

    GetDriver()->GetInput()->CreateBooleanComponent(props, "/input/x/click", &this->x_button_click_component_);

    GetDriver()->GetInput()->CreateBooleanComponent(props, "/input/y/click", &this->y_button_click_component_);

    GetDriver()->GetInput()->CreateBooleanComponent(props, "/input/trigger/click", &this->trigger_click_component_);
//    GetDriver()->GetInput()->CreateBooleanComponent(props, "/input/trigger/touch", &this->trigger_touch_component_);
    GetDriver()->GetInput()->CreateScalarComponent(props, "/input/trigger/value", &this->trigger_value_component_,
                                                   vr::EVRScalarType::VRScalarType_Absolute,
                                                   vr::EVRScalarUnits::VRScalarUnits_NormalizedOneSided);

//    GetDriver()->GetInput()->CreateBooleanComponent(props, "/input/grip/touch", &this->grip_touch_component_);
    GetDriver()->GetInput()->CreateScalarComponent(props, "/input/grip/value", &this->grip_value_component_,
                                                   vr::EVRScalarType::VRScalarType_Absolute,
                                                   vr::EVRScalarUnits::VRScalarUnits_NormalizedOneSided);
    GetDriver()->GetInput()->CreateScalarComponent(props, "/input/grip/force", &this->grip_force_component_,
                                                   vr::EVRScalarType::VRScalarType_Absolute,
                                                   vr::EVRScalarUnits::VRScalarUnits_NormalizedOneSided);

    GetDriver()->GetInput()->CreateBooleanComponent(props, "/input/system/click", &this->system_click_component_);
//    GetDriver()->GetInput()->CreateBooleanComponent(props, "/input/system/touch", &this->system_touch_component_);

    GetDriver()->GetInput()->CreateBooleanComponent(props, "/input/trackpad/click", &this->trackpad_click_component_);
//    GetDriver()->GetInput()->CreateBooleanComponent(props, "/input/trackpad/touch", &this->trackpad_touch_component_);
    GetDriver()->GetInput()->CreateScalarComponent(props, "/input/trackpad/x", &this->trackpad_x_component_,
                                                   vr::EVRScalarType::VRScalarType_Absolute,
                                                   vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided);
    GetDriver()->GetInput()->CreateScalarComponent(props, "/input/trackpad/y", &this->trackpad_y_component_,
                                                   vr::EVRScalarType::VRScalarType_Absolute,
                                                   vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided);

    GetDriver()->GetInput()->CreateBooleanComponent(props, "/input/joystick/click", &this->joystick_click_component_);
//    GetDriver()->GetInput()->CreateBooleanComponent(props, "/input/joystick/touch", &this->joystick_touch_component_);
    GetDriver()->GetInput()->CreateScalarComponent(props, "/input/joystick/x", &this->joystick_x_component_,
                                                   vr::EVRScalarType::VRScalarType_Absolute,
                                                   vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided);
    GetDriver()->GetInput()->CreateScalarComponent(props, "/input/joystick/y", &this->joystick_y_component_,
                                                   vr::EVRScalarType::VRScalarType_Absolute,
                                                   vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided);

    // Set some universe ID (Must be 2 or higher)
    GetDriver()->GetProperties()->SetUint64Property(props, vr::Prop_CurrentUniverseId_Uint64, 2);

    // Set up a model "number" (not needed but good to have)
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_ModelNumber_String, "example_controller");

    // Set up a render model path
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_RenderModelName_String,
                                                    "{example}example_controller");

    // Give SteamVR a hint at what hand this controller is for
    if (this->handedness_ == Handedness::LEFT) {
        GetDriver()->GetProperties()->SetInt32Property(props, vr::Prop_ControllerRoleHint_Int32,
                                                       vr::ETrackedControllerRole::TrackedControllerRole_LeftHand);
    } else if (this->handedness_ == Handedness::RIGHT) {
        GetDriver()->GetProperties()->SetInt32Property(props, vr::Prop_ControllerRoleHint_Int32,
                                                       vr::ETrackedControllerRole::TrackedControllerRole_RightHand);
    } else {
        GetDriver()->GetProperties()->SetInt32Property(props, vr::Prop_ControllerRoleHint_Int32,
                                                       vr::ETrackedControllerRole::TrackedControllerRole_OptOut);
    }

    // Set controller profile
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_InputProfilePath_String,
                                                    "{example}/input/example_controller_bindings.json");

    // Change the icon depending on which handedness this controller is using (ANY uses right)
    std::string controller_handedness_str = this->handedness_ == Handedness::LEFT ? "left" : "right";
    std::string controller_ready_file = "{example}/icons/controller_ready_" + controller_handedness_str + ".png";
    std::string controller_not_ready_file =
            "{example}/icons/controller_not_ready_" + controller_handedness_str + ".png";

    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReady_String,
                                                    controller_ready_file.c_str());

    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceOff_String,
                                                    controller_not_ready_file.c_str());
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearching_String,
                                                    controller_not_ready_file.c_str());
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearchingAlert_String,
                                                    controller_not_ready_file.c_str());
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReadyAlert_String,
                                                    controller_not_ready_file.c_str());
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceNotReady_String,
                                                    controller_not_ready_file.c_str());
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceStandby_String,
                                                    controller_not_ready_file.c_str());
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceAlertLow_String,
                                                    controller_not_ready_file.c_str());

    return vr::EVRInitError::VRInitError_None;
}

void ExampleDriver::ControllerDevice::Deactivate() {
    this->device_index_ = vr::k_unTrackedDeviceIndexInvalid;
}

void ExampleDriver::ControllerDevice::EnterStandby() {
}

void *ExampleDriver::ControllerDevice::GetComponent(const char *pchComponentNameAndVersion) {
    return nullptr;
}

void ExampleDriver::ControllerDevice::DebugRequest(const char *pchRequest, char *pchResponseBuffer,
                                                   uint32_t unResponseBufferSize) {
    if (unResponseBufferSize >= 1)
        pchResponseBuffer[0] = 0;
}

vr::DriverPose_t ExampleDriver::ControllerDevice::GetPose() {
    vr::DriverPose_t pose = {0};
    pose.poseIsValid = true;
    pose.result = vr::TrackingResult_Running_OK;
    pose.deviceIsConnected = true;

    vr::HmdQuaternion_t quat;
    vr::HmdQuaternion_t zeroQuat;
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

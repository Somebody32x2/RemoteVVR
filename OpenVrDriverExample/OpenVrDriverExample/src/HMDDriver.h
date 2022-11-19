//
// Created by samue on 11/11/2022.
//
#pragma once
#include <openvr_driver.h>
#include <windows.h>

#ifndef EXAMPLE_HMDDRIVER_H
#define EXAMPLE_HMDDRIVER_H

using namespace vr;


class HMDDriver : public ITrackedDeviceServerDriver, vr::IVRDisplayComponent{

    public:
        EVRInitError Activate(uint32_t unObjectId) override;

        void RunFrame();

        void Deactivate() override;

        void EnterStandby() override;

        void* GetComponent(const char* pchComponentNameAndVersion) override;

        void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) override;

        DriverPose_t GetPose() override;

        void GetWindowBounds(int32_t* pnX, int32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight) override;
        bool IsDisplayOnDesktop() override;
        bool IsDisplayRealDisplay() override;
        void GetRecommendedRenderTargetSize(uint32_t* pnWidth, uint32_t* pnHeight) override;
        void GetEyeOutputViewport(vr::EVREye eEye, uint32_t* pnX, uint32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight) override;
        void GetProjectionRaw(vr::EVREye eEye, float* pfLeft, float* pfRight, float* pfTop, float* pfBottom) override;
        vr::DistortionCoordinates_t ComputeDistortion(vr::EVREye eEye, float fU, float fV) override;



private:
    uint32_t driverId;
    std::string serial;
    vr::TrackedDeviceIndex_t deviceIndex;

    uint32_t windowX = 0;
    uint32_t windowY = 0;
    uint32_t windowWidth = 1920;
    uint32_t windowHeight = 1080;

    float position[3];
    float rotation[4];


};


#endif //EXAMPLE_HMDDRIVER_H

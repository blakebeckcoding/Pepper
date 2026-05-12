#pragma once
#include <openvr_driver.h>
#include "InputSystem.h"
#include "MathUtils.h"

# fake hmd
class FakeHMD
    : public vr::ITrackedDeviceServerDriver
    , public vr::IVRDisplayComponent
{
public:
    explicit FakeHMD(InputState* input) : m_input(input) {}

    # ITrackedDeviceServerDriver

    vr::EVRInitError Activate(uint32_t unObjectId) override
    {
        m_id = unObjectId;

        auto props = vr::VRProperties();

        # basic identity strings
        props->SetStringProperty(m_id, vr::Prop_ManufacturerName_String,     "FakeVR Inc.");
        props->SetStringProperty(m_id, vr::Prop_ModelNumber_String,          "FakeHMD v1.0");
        props->SetStringProperty(m_id, vr::Prop_SerialNumber_String,         "FAKEHMD001");
        props->SetStringProperty(m_id, vr::Prop_TrackingSystemName_String,   "fake_tracking");
        props->SetStringProperty(m_id, vr::Prop_RenderModelName_String,      "generic_hmd");
        props->SetStringProperty(m_id, vr::Prop_DriverVersion_String,        "1.0.0");

        # required hardware flags
        props->SetBoolProperty  (m_id, vr::Prop_IsOnDesktop_Bool,            false);
        props->SetBoolProperty  (m_id, vr::Prop_DeviceIsWireless_Bool,       true);
        props->SetFloatProperty (m_id, vr::Prop_UserIpdMeters_Float,         0.063f);
        props->SetFloatProperty (m_id, vr::Prop_DisplayFrequency_Float,      90.f);
        props->SetFloatProperty (m_id, vr::Prop_SecondsFromVsyncToPhotons_Float, 0.011f);

        return vr::VRInitError_None;
    }

    void Deactivate() override { m_id = vr::k_unTrackedDeviceIndexInvalid; }
    void EnterStandby() override {}
    void* GetComponent(const char* pchComponentNameAndVersion) override
    {
        if (!strcmp(pchComponentNameAndVersion, vr::IVRDisplayComponent_Version))
            return static_cast<IVRDisplayComponent*>(this);
        return nullptr;
    }
    void DebugRequest(const char*, char* pOut, uint32_t sz) override { if (sz) pOut[0] = '\0'; }

    vr::DriverPose_t GetPose() override
    {
        return MakePose(m_input->headX, m_input->headY, m_input->headZ,
                        m_input->headYaw, m_input->headPitch);
    }

    # IVRDisplayComponent

    void GetWindowBounds(int32_t* pnX, int32_t* pnY,
                         uint32_t* pnW, uint32_t* pnH) override
    {
        *pnX = 0; *pnY = 0; *pnW = 2160; *pnH = 1200;
    }

    bool IsDisplayOnDesktop() override { return false; }
    bool IsDisplayRealDisplay() override { return false; }

    void GetRecommendedRenderTargetSize(uint32_t* pnW, uint32_t* pnH) override
    {
        *pnW = 1512; *pnH = 1680;   // per-eye
    }

    void GetEyeOutputViewport(vr::EVREye eEye,
                              uint32_t* pnX, uint32_t* pnY,
                              uint32_t* pnW, uint32_t* pnH) override
    {
        *pnY = 0; *pnH = 1200;
        if (eEye == vr::Eye_Left)  { *pnX = 0;    *pnW = 1080; }
        else                       { *pnX = 1080;  *pnW = 1080; }
    }

    void GetProjectionRaw(vr::EVREye, float* pfLeft, float* pfRight,
                          float* pfTop, float* pfBottom) override
    {
        *pfLeft  = -1.f; *pfRight = 1.f;
        *pfTop   = -1.f; *pfBottom = 1.f;
    }

    vr::DistortionCoordinates_t ComputeDistortion(vr::EVREye, float fU, float fV) override
    {
        vr::DistortionCoordinates_t d{};
        d.rfRed[0] = d.rfGreen[0] = d.rfBlue[0] = fU;
        d.rfRed[1] = d.rfGreen[1] = d.rfBlue[1] = fV;
        return d;
    }

    # called every frame by the provider
    void Update()
    {
        if (m_id == vr::k_unTrackedDeviceIndexInvalid) return;
        vr::VRServerDriverHost()->TrackedDevicePoseUpdated(
            m_id, GetPose(), sizeof(vr::DriverPose_t));
    }

private:
    uint32_t    m_id    = vr::k_unTrackedDeviceIndexInvalid;
    InputState* m_input = nullptr;
};

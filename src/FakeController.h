#pragma once
#include <openvr_driver.h>
#include "InputSystem.h"
#include "MathUtils.h"
#include <string>

# fake controller
class FakeController : public vr::ITrackedDeviceServerDriver
{
public:
    FakeController(InputState* input, bool isLeft)
        : m_input(input), m_isLeft(isLeft) {}

    # ITrackedDeviceServerDriver

    vr::EVRInitError Activate(uint32_t unObjectId) override
    {
        m_id = unObjectId;
        auto props = vr::VRProperties();

        const char* side   = m_isLeft ? "left"  : "right";
        std::string serial = m_isLeft ? "FAKECTRL_L001" : "FAKECTRL_R001";

        props->SetStringProperty(m_id, vr::Prop_ManufacturerName_String,   "FakeVR Inc.");
        props->SetStringProperty(m_id, vr::Prop_ModelNumber_String,        "FakeController v1.0");
        props->SetStringProperty(m_id, vr::Prop_SerialNumber_String,       serial.c_str());
        props->SetStringProperty(m_id, vr::Prop_TrackingSystemName_String, "fake_tracking");
        props->SetStringProperty(m_id, vr::Prop_RenderModelName_String,    "{indexcontroller}system/rendermodels/valve_controller_knu_1_0_left");
        props->SetStringProperty(m_id, vr::Prop_ControllerType_String,     "knuckles");
        props->SetBoolProperty  (m_id, vr::Prop_DeviceIsWireless_Bool,     true);

        # tells SteamVR the hand it is
        props->SetInt32Property (m_id, vr::Prop_ControllerRoleHint_Int32,
            m_isLeft ? vr::TrackedControllerRole_LeftHand
                     : vr::TrackedControllerRole_RightHand);

        # register imput components
        auto* input = vr::VRDriverInput();

        # trigger
        input->CreateScalarComponent(m_id, "/input/trigger/value",
            &m_triggerHandle, vr::VRScalarType_Absolute,
            vr::VRScalarUnits_NormalizedOneSided);

        # grip
        input->CreateScalarComponent(m_id, "/input/grip/value",
            &m_gripHandle, vr::VRScalarType_Absolute,
            vr::VRScalarUnits_NormalizedOneSided);

        # joystick
        input->CreateScalarComponent(m_id, "/input/thumbstick/x",
            &m_joyXHandle, vr::VRScalarType_Absolute,
            vr::VRScalarUnits_NormalizedTwoSided);
        input->CreateScalarComponent(m_id, "/input/thumbstick/y",
            &m_joyYHandle, vr::VRScalarType_Absolute,
            vr::VRScalarUnits_NormalizedTwoSided);

        # buttons
        input->CreateBooleanComponent(m_id, "/input/a/click",    &m_btnA);
        input->CreateBooleanComponent(m_id, "/input/b/click",    &m_btnB);
        input->CreateBooleanComponent(m_id, "/input/system/click", &m_btnSys);

        return vr::VRInitError_None;
    }

    void Deactivate() override { m_id = vr::k_unTrackedDeviceIndexInvalid; }
    void EnterStandby() override {}
    void* GetComponent(const char*) override { return nullptr; }
    void DebugRequest(const char*, char* pOut, uint32_t sz) override { if (sz) pOut[0] = '\0'; }

    vr::DriverPose_t GetPose() override
    {
        return MakeControllerPose(
            m_input->headX, m_input->headY, m_input->headZ,
            m_input->headYaw, m_input->headPitch,
            m_isLeft);
    }

    # called every frame by the provider
    void Update()
    {
        if (m_id == vr::k_unTrackedDeviceIndexInvalid) return;

        auto* input = vr::VRDriverInput();
        const double now = 0.0; // 0 = "right now"

        if (m_isLeft)
        {
            input->UpdateScalarComponent (m_triggerHandle, m_input->leftTrigger,  now);
            input->UpdateScalarComponent (m_gripHandle,    m_input->leftGrip,     now);
            input->UpdateScalarComponent (m_joyXHandle,    m_input->leftJoyX,     now);
            input->UpdateScalarComponent (m_joyYHandle,    m_input->leftJoyY,     now);
            input->UpdateBooleanComponent(m_btnA,          m_input->buttonX,      now);
            input->UpdateBooleanComponent(m_btnB,          m_input->buttonY,      now);
            input->UpdateBooleanComponent(m_btnSys,        m_input->leftMenu,     now);
        }
        else
        {
            input->UpdateScalarComponent (m_triggerHandle, m_input->rightTrigger, now);
            input->UpdateScalarComponent (m_gripHandle,    m_input->rightGrip,    now);
            input->UpdateScalarComponent (m_joyXHandle,    m_input->rightJoyX,    now);
            input->UpdateScalarComponent (m_joyYHandle,    m_input->rightJoyY,    now);
            input->UpdateBooleanComponent(m_btnA,          m_input->buttonA,      now);
            input->UpdateBooleanComponent(m_btnB,          m_input->buttonB,      now);
            input->UpdateBooleanComponent(m_btnSys,        m_input->rightMenu,    now);
        }

        vr::VRServerDriverHost()->TrackedDevicePoseUpdated(
            m_id, GetPose(), sizeof(vr::DriverPose_t));
    }

private:
    uint32_t    m_id     = vr::k_unTrackedDeviceIndexInvalid;
    InputState* m_input  = nullptr;
    bool        m_isLeft = true;

    vr::VRInputComponentHandle_t m_triggerHandle = vr::k_ulInvalidInputComponentHandle;
    vr::VRInputComponentHandle_t m_gripHandle    = vr::k_ulInvalidInputComponentHandle;
    vr::VRInputComponentHandle_t m_joyXHandle    = vr::k_ulInvalidInputComponentHandle;
    vr::VRInputComponentHandle_t m_joyYHandle    = vr::k_ulInvalidInputComponentHandle;
    vr::VRInputComponentHandle_t m_btnA          = vr::k_ulInvalidInputComponentHandle;
    vr::VRInputComponentHandle_t m_btnB          = vr::k_ulInvalidInputComponentHandle;
    vr::VRInputComponentHandle_t m_btnSys        = vr::k_ulInvalidInputComponentHandle;
};

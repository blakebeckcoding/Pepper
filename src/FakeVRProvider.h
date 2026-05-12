#pragma once
#include <openvr_driver.h>
#include "InputSystem.h"
#include "FakeHMD.h"
#include "FakeController.h"
#include <memory>

# fake VR provider
class FakeVRProvider : public vr::IServerTrackedDeviceProvider
{
public:
    # IServerTrackedDeviceProvider

    vr::EVRInitError Init(vr::IVRDriverContext* pDriverContext) override
    {
        # initialises the OpenVR driver context
        VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

        # build devices
        m_hmd        = std::make_unique<FakeHMD>       (&m_input.state);
        m_leftCtrl   = std::make_unique<FakeController>(&m_input.state, true);
        m_rightCtrl  = std::make_unique<FakeController>(&m_input.state, false);

        # register with steamVR
        vr::VRServerDriverHost()->TrackedDeviceAdded(
            "FakeHMD001",       vr::TrackedDeviceClass_HMD,        m_hmd.get());
        vr::VRServerDriverHost()->TrackedDeviceAdded(
            "FakeCtrl_L001",    vr::TrackedDeviceClass_Controller, m_leftCtrl.get());
        vr::VRServerDriverHost()->TrackedDeviceAdded(
            "FakeCtrl_R001",    vr::TrackedDeviceClass_Controller, m_rightCtrl.get());

        # start polling input
        m_input.Start();

        return vr::VRInitError_None;
    }

    void Cleanup() override
    {
        m_input.Stop();
        m_hmd.reset();
        m_leftCtrl.reset();
        m_rightCtrl.reset();
        VR_CLEANUP_SERVER_DRIVER_CONTEXT();
    }

    const char* const* GetInterfaceVersions() override
    {
        return vr::k_InterfaceVersions;
    }

    # main update loop
    void RunFrame() override
    {
        
        if (m_hmd)       m_hmd->Update();
        if (m_leftCtrl)  m_leftCtrl->Update();
        if (m_rightCtrl) m_rightCtrl->Update();

       
        vr::VREvent_t event{};
        while (vr::VRServerDriverHost()->PollNextEvent(&event, sizeof(event)))
        {
            if (event.eventType == vr::VREvent_Input_HapticVibration)
            {
                
            }
        }
    }

    bool ShouldBlockStandbyMode() override { return false; }
    void EnterStandby()           override {}
    void LeaveStandby()           override {}

private:
    InputThread  m_input;

    std::unique_ptr<FakeHMD>        m_hmd;
    std::unique_ptr<FakeController> m_leftCtrl;
    std::unique_ptr<FakeController> m_rightCtrl;
};

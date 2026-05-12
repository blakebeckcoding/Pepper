#pragma once
#include <cmath>
#include <openvr_driver.h>



struct Quat { double w, x, y, z; };


inline Quat EulerToQuat(float yaw, float pitch)
{
    float hy = yaw   * 0.5f;
    float hp = pitch * 0.5f;

    float cy = cosf(hy), sy = sinf(hy);
    float cp = cosf(hp), sp = sinf(hp);

    Quat q;
    
    q.w =  cy * cp;
    q.x =  cy * sp;
    q.y =  sy * cp;
    q.z = -sy * sp;
    return q;
}


inline vr::DriverPose_t MakePose(float px, float py, float pz, float yaw, float pitch)
{
    vr::DriverPose_t pose{};
    pose.poseIsValid           = true;
    pose.result                = vr::TrackingResult_Running_OK;
    pose.deviceIsConnected     = true;

    
    pose.qWorldFromDriverRotation.w = 1; pose.qWorldFromDriverRotation.x = 0;
    pose.qWorldFromDriverRotation.y = 0; pose.qWorldFromDriverRotation.z = 0;
    pose.qDriverFromHeadRotation.w  = 1; pose.qDriverFromHeadRotation.x  = 0;
    pose.qDriverFromHeadRotation.y  = 0; pose.qDriverFromHeadRotation.z  = 0;

    pose.vecPosition[0] = px;
    pose.vecPosition[1] = py;
    pose.vecPosition[2] = pz;

    Quat q = EulerToQuat(yaw, pitch);
    pose.qRotation.w = q.w;
    pose.qRotation.x = q.x;
    pose.qRotation.y = q.y;
    pose.qRotation.z = q.z;

    return pose;
}


inline vr::DriverPose_t MakeControllerPose(
    float headX, float headY, float headZ,
    float headYaw, float headPitch,
    bool  leftSide)
{
    float side   = leftSide ? -1.f : 1.f;
    float offX   =  side  * 0.20f;   
    float offY   = -0.20f;           
    float offZ   = -0.40f;           

    
    float cy = cosf(headYaw);
    float sy = sinf(headYaw);

    float wx = offX * cy - offZ * (-sy);   
    float wz = offX * sy + offZ * cy;
    

    return MakePose(headX + wx, headY + offY, headZ + wz,
                    headYaw, headPitch * 0.3f);  
}

/*
Posture checker examine the depth data
and calculates data regarding user's real-time posture
*/

#ifndef XNV_POINT_DRAWER_H_
#define XNV_POINT_DRAWER_H_

#include <XnCppWrapper.h>

void CheckPosture(const xn::DepthMetaData& dmd, const xn::SceneMetaData& smd);

onCapability& capability, const XnChar* strPose, XnUserID id, XnPoseDetectionStatus poseError, void* pCookie);
#endif
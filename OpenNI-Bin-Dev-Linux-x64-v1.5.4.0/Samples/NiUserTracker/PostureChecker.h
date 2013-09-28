/*
Posture checker examine the depth data
and calculates data regarding user's real-time posture
*/

#ifndef XNV_POSTURE_CHECKER_H_
#define XNV_POSTURE_CHECKER_H_

#include <XnCppWrapper.h>

void CheckPosture(const xn::DepthMetaData& dmd, const xn::SceneMetaData& smd);

#endif
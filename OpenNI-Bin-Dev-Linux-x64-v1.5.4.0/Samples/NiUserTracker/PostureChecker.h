/*
Posture checker examine the depth data
and calculates data regarding user's real-time posture
*/

#ifndef XNV_POSTURE_CHECKER_H_
#define XNV_POSTURE_CHECKER_H_

#include <XnCppWrapper.h>
#include <stdio.h>

void CheckPosture(const xn::DepthMetaData& dmd, const xn::SceneMetaData& smd, FILE *fp);
float CalculateDistanceBetweenPoints(XnPoint3D leftPoint, XnPoint3D rightPoint);
float CalculateJointAngle(XnUserID player, XnSkeletonJoint eJointLeft, XnSkeletonJoint eJointMid, XnSkeletonJoint eJointRight);
float CalculateJointDistance(XnUserID player, XnSkeletonJoint eJointLeft, XnSkeletonJoint eJointRight);
float CalculateAngleInTriangle(XnPoint3D leftPoint, XnPoint3D midPoint, XnPoint3D rightPoint);
float CalculateHeadNeckAngle(XnUserID player);
void RecordAngle(XnUserID player, FILE *fp);


#endif
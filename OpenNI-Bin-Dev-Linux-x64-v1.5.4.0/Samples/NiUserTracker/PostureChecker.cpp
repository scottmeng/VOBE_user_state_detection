#include <math.h>
#include "PostureChecker.h"
#include <stdio.h>

extern xn::UserGenerator g_UserGenerator;
extern xn::DepthGenerator g_DepthGenerator;

float CalculateJointAngle(XnUserID player, XnSkeletonJoint eJointLeft, 
	XnSkeletonJoint eJointMid, XnSkeletonJoint eJointRight)
{
	if (!g_UserGenerator.GetSkeletonCap().IsTracking(player))
	{
		printf("not tracked!\n");
		return -1;
	}

	if (!g_UserGenerator.GetSkeletonCap().IsJointActive(eJointLeft)||
		!g_UserGenerator.GetSkeletonCap().IsJointActive(eJointMid) ||
		!g_UserGenerator.GetSkeletonCap().IsJointActive(eJointRight))
	{
		return -1;
	}

	XnSkeletonJointPosition jointLeft, jointMid, jointRight;
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJointLeft, jointLeft);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJointMid, jointMid);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJointRight, jointRight);

	if (jointLeft.fConfidence < 0.5 || jointMid.fConfidence < 0.5 || jointRight.fConfidence < 0.5)
	{
		return -1;
	}

	return CalculateAngleInTriangle(jointLeft.position, jointMid.position, jointRight.position);
}

float CalculateJointDistance(XnUserID player, XnSkeletonJoint eJointLeft, XnSkeletonJoint eJointRight)
{
	if (!g_UserGenerator.GetSkeletonCap().IsTracking(player))
	{
		printf("not tracked!\n");
		return -1;
	}

	if (!g_UserGenerator.GetSkeletonCap().IsJointActive(eJointLeft) ||
		!g_UserGenerator.GetSkeletonCap().IsJointActive(eJointRight))
	{
		return -1;
	}

	XnSkeletonJointPosition jointLeft, jointRight;
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJointLeft, jointLeft);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJointRight, jointRight);

	if (jointLeft.fConfidence < 0.5 || jointRight.fConfidence < 0.5)
	{
		return -1;
	}

	return CalculateDistanceBetweenPoints(jointLeft.position, jointRight.position);
}

// calculate the angle in a provided triangle
float CalculateAngleInTriangle(XnPoint3D leftPoint, XnPoint3D midPoint, XnPoint3D rightPoint)
{
	float leftSide, midSide, rightSide;
	double cosAngle;

	leftSide = CalculateDistanceBetweenPoints(leftPoint, midPoint);
	
	rightSide = CalculateDistanceBetweenPoints(rightPoint, midPoint);

	midSide = CalculateDistanceBetweenPoints(leftPoint, rightPoint);
	
	cosAngle = (pow(leftSide, 2.0) + pow(rightSide, 2.0) - pow(midSide, 2.0))/(2 * leftSide * rightSide);

	return acos(cosAngle)*57.3;	
}

// calculate the distance given two points in 3D space
float CalculateDistanceBetweenPoints(XnPoint3D leftPoint, XnPoint3D rightPoint)
{
	return sqrt(pow(leftPoint.X - rightPoint.X, 2.0) +
			    pow(leftPoint.Y - rightPoint.Y, 2.0) +
			    pow(leftPoint.Z - rightPoint.Z, 2.0));
}

// track two users 
// take down users' joint positions
// calculate joint angles and etc.
void CheckPosture(const xn::DepthMetaData& dmd, const xn::SceneMetaData& smd, FILE *fp)
{
	XnUserID aUsers[2];
	XnUInt16 nUsers = 2;
	g_UserGenerator.GetUsers(aUsers, nUsers);

	for(int i = 0; i < nUsers; ++i)
	{
		// if the user is being tracked
		// take down his/her joint positions
		if (g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i]))
		{
			// check user posture
			RecordAngle(aUsers[i], fp);
		}
	}
}

// calculate the angle formed by head and neck
// with regard to the horizon
float CalculateHeadNeckAngle(XnUserID player)
{

	if (!g_UserGenerator.GetSkeletonCap().IsTracking(player))
	{
		printf("not tracked!\n");
		return -1;
	}

	if (!g_UserGenerator.GetSkeletonCap().IsJointActive(XN_SKEL_NECK) ||
		!g_UserGenerator.GetSkeletonCap().IsJointActive(XN_SKEL_HEAD))
	{
		return -1;
	}

	XnSkeletonJointPosition posHead, posNeck;
	
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, XN_SKEL_HEAD, posHead);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, XN_SKEL_NECK, posNeck);

	float longSide = CalculateDistanceBetweenPoints(posHead.position, posNeck.position);

	float shortSide = abs(posHead.position.Y - posNeck.position.Y);

	return asin(shortSide/longSide) * 57.3;
}

// record 8 joint angles and one head-neck orientation
void RecordAngle(XnUserID player, FILE *fp)
{
	XnBool current;
	float angleHeadNeckTorse, 
		  angleHeadNeckLeftShoulder, 
		  angleHeadNeckRightShoulder, 
		  angleNeckHipLeftKnee, 
		  angleNeckHipRightKnee,
		  angleNeckLeftShoulderLeftArm,
		  angleNeckRightShoulderRightArm;

	angleHeadNeckTorse = CalculateJointAngle(player, XN_SKEL_HEAD, XN_SKEL_NECK, XN_SKEL_TORSO);
	angleHeadNeckLeftShoulder = CalculateJointAngle(player, XN_SKEL_HEAD, XN_SKEL_NECK, XN_SKEL_LEFT_SHOULDER);
	angleHeadNeckRightShoulder = CalculateJointAngle(player, XN_SKEL_HEAD, XN_SKEL_NECK, XN_SKEL_RIGHT_SHOULDER);
	angleNeckHipLeftKnee = CalculateJointAngle(player, XN_SKEL_NECK, XN_SKEL_LEFT_HIP, XN_SKEL_LEFT_KNEE);
	angleNeckHipRightKnee = CalculateJointAngle(player, XN_SKEL_NECK, XN_SKEL_RIGHT_HIP, XN_SKEL_RIGHT_KNEE);
	angleNeckLeftShoulderLeftArm = CalculateHeadNeckAngle(player, XN_SKEL_NECK, XN_SKEL_LEFT_SHOULDER, XN_SKEL_LEFT_ELBOW);
	angleNeckRightShoulderRightArm = CalculateHeadNeckAngle(player, XN_SKEL_NECK, XN_SKEL_RIGHT_SHOULDER, XN_SKEL_RIGHT_ELBOW);


	float angleHeadNeck = CalculateHeadNeckAngle(player);

	char output[100] = "";
	xnOSMemSet(output, 0, sizeof(output));
	sprintf(output, "%.2f	%.2f	%.2f	%.2f	%.2f	%.2f	%.2f	%.2f\n", angleHeadNeckTorse, angleHeadNeckLeftShoulder, 
		angleHeadNeckRightShoulder, angleNeckHipLeftKnee, angleNeckHipRightKnee,, angleNeckLeftShoulderLeftArm, 
		angleNeckRightShoulderRightArm, angleHeadNeck);

	fprintf(fp, output);
}
#include <math.h>
#include "PostureChecker.h"


extern xn::UserGenerator g_UserGenerator;
extern xn::DepthGenerator g_DepthGenerator;

float DrawLimb(XnUserID player, XnSkeletonJoint eJoint1, XnSkeletonJoint eJoint2)
{
	if (!g_UserGenerator.GetSkeletonCap().IsTracking(player))
	{
		printf("not tracked!\n");
		return true;
	}

	if (!g_UserGenerator.GetSkeletonCap().IsJointActive(eJoint1) ||
		!g_UserGenerator.GetSkeletonCap().IsJointActive(eJoint2))
	{
		return false;
	}

	XnSkeletonJointPosition joint1, joint2;
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJoint1, joint1);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJoint2, joint2);

	if (joint1.fConfidence < 0.5 || joint2.fConfidence < 0.5)
	{
		return true;
	}

	XnPoint3D pt[2];
	pt[0] = joint1.position;
	pt[1] = joint2.position;

	g_DepthGenerator.ConvertRealWorldToProjective(2, pt, pt);

	return true;
}

// calculate the angle in a provided triangle
float CalculateAngleInTriangle(XnPoint3D leftPoint, XnPoint3D midPoint, XnPoint3D rightPoint)
{
	float leftSide, midSide, rightSide;
	double cosAngle;

	leftSide = sqrt(pow(leftPoint.X - midPoint.X, 2.0) + 
					pow(leftPoint.Y - midPoint.Y, 2.0) + 
					pow(leftPoint.Z - midPoint.Z, 2.0));

	rightSide = sqrt(pow(rightPoint.X - midPoint.X, 2.0) +
					 pow(rightPoint.Y - midPoint.Y, 2.0) +
					 pow(rightPoint.Z - midPoint.Z, 2.0));

	midSide = sqrt(pow(leftPoint.X - rightPoint.X, 2.0) +
				   pow(leftPoint.Y - rightPoint.Y, 2.0) +
				   pow(leftPoint.Z - rightPoint.Z, 2.0));

	cosAngle = (pow(leftSide, 2.0) + pow(rightSide, 2.0) - pow(midSide, 2.0))/(2 * leftSide * rightSide);

	return acos(cosAngle)*57.3;	
}

void DrawJoint(XnUserID player, XnSkeletonJoint eJoint)
{
	if (!g_UserGenerator.GetSkeletonCap().IsTracking(player))
	{
		printf("not tracked!\n");
		return;
	}

	if (!g_UserGenerator.GetSkeletonCap().IsJointActive(eJoint))
	{
		return;
	}

	XnSkeletonJointPosition joint;
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJoint, joint);

	if (joint.fConfidence < 0.5)
	{
		return;
	}

	XnPoint3D pt;
	pt = joint.position;

	g_DepthGenerator.ConvertRealWorldToProjective(1, &pt, &pt);

	drawCircle(pt.X, pt.Y, 2);
}


// track two users 
// take down users' joint positions
// calculate joint angles and etc.
void CheckPosture(const xn::DepthMetaData& dmd, const xn::SceneMetaData& smd)
{
	nUserID aUsers[2];
	XnUInt16 nUsers = 2;
	g_UserGenerator.GetUsers(aUsers, nUsers);

	for(int i = 0; i < nUsers; ++i)
	{
		// if the user is being tracked
		// take down his/her joint positions
		if (g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i]))
		{
			// check user posture
		}
	}
}
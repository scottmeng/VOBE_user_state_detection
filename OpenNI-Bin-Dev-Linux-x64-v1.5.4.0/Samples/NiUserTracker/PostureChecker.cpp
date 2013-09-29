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

#ifndef USE_GLES
	glVertex3i(pt[0].X, pt[0].Y, 0);
	glVertex3i(pt[1].X, pt[1].Y, 0);
#else
	GLfloat verts[4] = {pt[0].X, pt[0].Y, pt[1].X, pt[1].Y};
	glVertexPointer(2, GL_FLOAT, 0, verts);
	glDrawArrays(GL_LINES, 0, 2);
	glFlush();
#endif

	return true;
}

// calculate the angle in a provided triangle
float CalculateAngleInTriangle(XnPoint3D leftPoint, XnPoint3D midPoint, XnPoint3D rightPoint)
{
	float leftSide, midSide, rightSide;

	leftSide = sqrt(pow(leftPoint.X - midPoint.X, 2.0) + 
					pow(leftPoint.Y - midPoint.Y, 2.0) + 
					pow(leftPoint.Z - midPoint.Z, 2.0));

	rightSide = sqrt(pow(rightPoint.X - midPoint.X, 2.0) +
					 pow(rightPoint.Y - midPoint.Y, 2.0) +
					 pow(rightPoint.Z - midPoint.Z, 2.0));

	midSide = sqrt(pow(leftPoint.X - rightPoint.X, 2.0) +
				   pow(leftPoint.Y - rightPoint.Y, 2.0) +
				   pow(leftPoint.Z - rightPoint.Z, 2.0));

	
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
			// Draw Joints
			// Try to draw all joints
			DrawJoint(aUsers[i], XN_SKEL_HEAD);
			DrawJoint(aUsers[i], XN_SKEL_NECK);
			DrawJoint(aUsers[i], XN_SKEL_TORSO);
			DrawJoint(aUsers[i], XN_SKEL_WAIST);

			DrawJoint(aUsers[i], XN_SKEL_LEFT_COLLAR);
			DrawJoint(aUsers[i], XN_SKEL_LEFT_SHOULDER);
			DrawJoint(aUsers[i], XN_SKEL_LEFT_ELBOW);
			DrawJoint(aUsers[i], XN_SKEL_LEFT_WRIST);
			DrawJoint(aUsers[i], XN_SKEL_LEFT_HAND);
			DrawJoint(aUsers[i], XN_SKEL_LEFT_FINGERTIP);

			DrawJoint(aUsers[i], XN_SKEL_RIGHT_COLLAR);
			DrawJoint(aUsers[i], XN_SKEL_RIGHT_SHOULDER);
			DrawJoint(aUsers[i], XN_SKEL_RIGHT_ELBOW);
			DrawJoint(aUsers[i], XN_SKEL_RIGHT_WRIST);
			DrawJoint(aUsers[i], XN_SKEL_RIGHT_HAND);
			DrawJoint(aUsers[i], XN_SKEL_RIGHT_FINGERTIP);

			DrawJoint(aUsers[i], XN_SKEL_LEFT_HIP);
			DrawJoint(aUsers[i], XN_SKEL_LEFT_KNEE);
			DrawJoint(aUsers[i], XN_SKEL_LEFT_ANKLE);
			DrawJoint(aUsers[i], XN_SKEL_LEFT_FOOT);

			DrawJoint(aUsers[i], XN_SKEL_RIGHT_HIP);
			DrawJoint(aUsers[i], XN_SKEL_RIGHT_KNEE);
			DrawJoint(aUsers[i], XN_SKEL_RIGHT_ANKLE);
			DrawJoint(aUsers[i], XN_SKEL_RIGHT_FOOT);
		}
	}
}
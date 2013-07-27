/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
//---------------------------------------------------------------------------
// Includes
// include glut and openGL ES for drawing
//---------------------------------------------------------------------------
#include "SceneDrawer.h"
#include <math.h>
#include <stdio.h>

#ifndef USE_GLES
#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
#endif
#else
	#include "opengles.h"
#endif

// external variable declaration
extern xn::UserGenerator g_UserGenerator;
extern xn::DepthGenerator g_DepthGenerator;

extern XnBool g_bDrawBackground;
extern XnBool g_bDrawPixels;
extern XnBool g_bDrawSkeleton;
extern XnBool g_bPrintID;
extern XnBool g_bPrintState;

XnBool isUserResting; 
XnBool restingHistory[100];
int historyIndex; 

XnVector3D prevHead, prevShoulderLeft, prevShoulderRight, prevHandLeft, prevHandRight, prevKneeLeft, prevKneeRight;

XnBool isUserMoving;
XnBool movementHistory[100];
int movementIndex;


#include <map>
std::map<XnUInt32, std::pair<XnCalibrationStatus, XnPoseDetectionStatus> > m_Errors;
void XN_CALLBACK_TYPE MyCalibrationInProgress(xn::SkeletonCapability& capability, XnUserID id, XnCalibrationStatus calibrationError, void* pCookie)
{
	m_Errors[id].first = calibrationError;
}
void XN_CALLBACK_TYPE MyPoseInProgress(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID id, XnPoseDetectionStatus poseError, void* pCookie)
{
	m_Errors[id].second = poseError;
}

#define MAX_DEPTH 10000
float g_pDepthHist[MAX_DEPTH];
unsigned int getClosestPowerOfTwo(unsigned int n)
{
	unsigned int m = 2;
	// left shift unsigned integer 
	// to get the closest power of two value for n
	while(m < n) 
		m<<=1;

	return m;
}

// GLuint is very similar to unsigned int
// where as GLint is more like normal signed int

GLuint initTexture(void** buf, int& width, int& height)
{
	GLuint texID = 0;
	glGenTextures(1,&texID);

	width = getClosestPowerOfTwo(width);
	height = getClosestPowerOfTwo(height); 
	*buf = new unsigned char[width*height*4];
	glBindTexture(GL_TEXTURE_2D,texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	isUserResting = false;
	isUserMoving = true;

	historyIndex = 0;
	movementIndex = 0;

	xnOSMemSet(restingHistory, 100, sizeof(XnBool));
	xnOSMemSet(movementHistory, 100, sizeof(XnBool));

	for(int i=0; i<100; i++)
	{
		restingHistory[i] = false;
		movementHistory[i] = true;
	}

	return texID;
}

GLfloat texcoords[8];
void DrawRectangle(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY)
{
	GLfloat verts[8] = {	topLeftX, topLeftY,
		topLeftX, bottomRightY,
		bottomRightX, bottomRightY,
		bottomRightX, topLeftY
	};
	glVertexPointer(2, GL_FLOAT, 0, verts);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	//TODO: Maybe glFinish needed here instead - if there's some bad graphics crap
	glFlush();
}

void DrawCircle(float centerX, float centerY, float radius)
{
   glBegin(GL_LINE_LOOP);
 
   for (int i=0; i < 360; i++)
   {
      float degInRad = i*0.017;
      glVertex2f(cos(degInRad)*radius + centerX, sin(degInRad)*radius + centerY);
   }
 
   glEnd();
}

void DrawTexture(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY)
{
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texcoords);

	DrawRectangle(topLeftX, topLeftY, bottomRightX, bottomRightY);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

XnFloat Colors[][3] =
{
	{0,1,1},
	{0,0,1},
	{0,1,0},
	{1,1,0},
	{1,0,0},
	{1,.5,0},
	{.5,1,0},
	{0,.5,1},
	{.5,0,1},
	{1,1,.5},
	{1,1,1}
};

XnUInt32 nColors = 10;

#ifndef USE_GLES
void glPrintString(void *font, char *str)
{
	int i,l = strlen(str);

	for(i=0; i<l; i++)
	{
		glutBitmapCharacter(font,*str++);
	}
}
#endif

void ShowPosition(XnPoint3D point, FILE *fp)
{
	XnPoint3D position;
	char output[50] = "this is very long";

	g_DepthGenerator.ConvertRealWorldToProjective(1, &point, &position);

	xnOSMemSet(output, 0, sizeof(output));

	sprintf(output, "x: %.2f; y: %.2f; z: %.2f\n", point.X, point.Y, point.Z);


	glColor4f(1.0, 1.0, 1.0, 1.0);

	glRasterPos2i(position.X, position.Y);

	glPrintString(GLUT_BITMAP_HELVETICA_18, output);

	glFlush();

	fprintf(fp, output);
}

void DrawJoint(XnUserID player, XnSkeletonJoint joint)
{
	if(!g_UserGenerator.GetSkeletonCap().IsTracking(player))
	{
		printf("not tracked!\n");
		return;
	}
	
	XnSkeletonJointPosition position;
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, joint, position);

	if (position.fConfidence < 0.5)
	{
		return;
	}
	
	XnPoint3D point = position.position;

	// convert to projective position
	g_DepthGenerator.ConvertRealWorldToProjective(1, &point, &point);

	DrawCircle(point.X, point.Y, 10);
}

void DrawLimb(XnUserID player, XnSkeletonJoint eJoint1, XnSkeletonJoint eJoint2)
{
	if (!g_UserGenerator.GetSkeletonCap().IsTracking(player))
	{
		//printf("not tracked!\n");
		return;
	}

	// get skeleton joint positions
	XnSkeletonJointPosition joint1, joint2;
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJoint1, joint1);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJoint2, joint2);

	// if either joints has very low confidence rate
	// stop drawing limb
	if (joint1.fConfidence < 0.5 || joint2.fConfidence < 0.5)
	{
		return;
	}

	XnPoint3D pt[2];
	pt[0] = joint1.position;
	pt[1] = joint2.position;

	// convert a list of points from real world coordinates into projective coordinates
	// first argument: XnUInt32 -- number of points
	// second argument: const XnPoint3D -- source array of XnPoint3D
	// third argument: XnPoint3D -- destination array of XnPoint3D
	g_DepthGenerator.ConvertRealWorldToProjective(2, pt, pt);

#ifndef USE_GLES
	// specify the vertex value
	glVertex3i(pt[0].X, pt[0].Y, 0);
	glVertex3i(pt[1].X, pt[1].Y, 0);
#else
	// an array of float coordinates specifying the starting and ending positions
	// note that every position is specified by X and Y coordinates
	// which are retrieved from skeleton joint position
	GLfloat verts[4] = {pt[0].X, pt[0].Y, pt[1].X, pt[1].Y};
	// glVertexPointer — define an array of vertex data
	// first argument: size -- specifies the number of coordinates per vertex
	// second argument: type -- Specifies the data type of each coordinate in the array
	// third argument: stride -- Specifies the byte offset between consecutive vertices
	// fourth argument: pointer -- Specifies a pointer to the first coordinate of the first vertex in the array
	glVertexPointer(2, GL_FLOAT, 0, verts);

	// render primitives from array data
	// first argument: GLenum -- specifies what kind of primitives to render
	// second argument: GLint -- specifies the starting index in the enabled array
	// third argument: GLsizei --  specifies the number of indices to be rendered
	glDrawArrays(GL_LINES, 0, 2);
	// forces execution of OpenGL functions in finite time
	glFlush();
#endif
}

const XnChar* GetCalibrationErrorString(XnCalibrationStatus error)
{
	switch (error)
	{
	case XN_CALIBRATION_STATUS_OK:
		return "OK";
	case XN_CALIBRATION_STATUS_NO_USER:
		return "NoUser";
	case XN_CALIBRATION_STATUS_ARM:
		return "Arm";
	case XN_CALIBRATION_STATUS_LEG:
		return "Leg";
	case XN_CALIBRATION_STATUS_HEAD:
		return "Head";
	case XN_CALIBRATION_STATUS_TORSO:
		return "Torso";
	case XN_CALIBRATION_STATUS_TOP_FOV:
		return "Top FOV";
	case XN_CALIBRATION_STATUS_SIDE_FOV:
		return "Side FOV";
	case XN_CALIBRATION_STATUS_POSE:
		return "Pose";
	default:
		return "Unknown";
	}
}
const XnChar* GetPoseErrorString(XnPoseDetectionStatus error)
{
	switch (error)
	{
	case XN_POSE_DETECTION_STATUS_OK:
		return "OK";
	case XN_POSE_DETECTION_STATUS_NO_USER:
		return "NoUser";
	case XN_POSE_DETECTION_STATUS_TOP_FOV:
		return "Top FOV";
	case XN_POSE_DETECTION_STATUS_SIDE_FOV:
		return "Side FOV";
	case XN_POSE_DETECTION_STATUS_ERROR:
		return "General error";
	default:
		return "Unknown";
	}
}

// function to calculate the distance between two joints
GLfloat GetJointDistance(XnUserID player, XnSkeletonJoint eJoint1, XnSkeletonJoint eJoint2)
{
	// if xtion is not tracking this user
	// return dummy data
	if (!g_UserGenerator.GetSkeletonCap().IsTracking(player))
	{
		//printf("not tracked!\n");
		return -1.0;
	}

	// get skeleton joint positions
	XnSkeletonJointPosition joint1, joint2;
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJoint1, joint1);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJoint2, joint2);

	// if either joints has very low confidence rate
	// stop calculation
	if (joint1.fConfidence < 0.5 || joint2.fConfidence < 0.5)
	{
		return -1.0;
	} 

	XnVector3D pos1, pos2;
	pos1 = joint1.position;
	pos2 = joint2.position;

 	GLfloat distPow = pow(pos1.X - pos2.X, 2.0) + pow(pos1.Y - pos2.Y, 2.0) + pow(pos1.Z - pos2.Z, 2.0);
 	GLfloat dist = sqrt(distPow);

 	return dist;
}

double GetJointAngle(XnUserID player, XnSkeletonJoint jointLeft, XnSkeletonJoint jointMid, XnSkeletonJoint jointRight)
{
	// if xtion is not tracking the user
	// return dummy data
	if (!g_UserGenerator.GetSkeletonCap().IsTracking(player))
	{
		//printf("not tracked!\n");
		return -1.0;
	}

	// get skeleton joint positions
	XnSkeletonJointPosition posLeft, posMid, posRight;
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, jointLeft, posLeft);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, jointMid, posMid);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, jointRight, posRight);

	GLfloat sideLeft, sideMid, sideRight;
	double cosAngle, angle;

	// calculate the three sides of the triangle formed by the three joint points
	sideLeft = GetJointDistance(player, jointMid, jointRight);
	sideMid = GetJointDistance(player, jointLeft, jointRight);
	sideRight = GetJointDistance(player, jointMid, jointLeft);

	cosAngle = (pow(sideLeft, 2.0) + pow(sideRight, 2.0) - pow(sideMid, 2.0))/(2 * sideLeft * sideRight);

	// get the angle is radians
	angle = acos(cosAngle) * 57.3;

	return angle;
}

// function to calculate the distance between two points
GLfloat GetPointDistance(XnVector3D pos1, XnVector3D pos2)
{
 	GLfloat distPow = pow(pos1.X - pos2.X, 2.0) + pow(pos1.Y - pos2.Y, 2.0) + pow(pos1.Z - pos2.Z, 2.0);
 	GLfloat dist = sqrt(distPow);

 	return dist;
}

void CheckUserResting(XnUserID player, XnBool current)
{
	restingHistory[historyIndex] = current;
	if(historyIndex == 99)
	{
		historyIndex = 0;
	}
	else
	{
		historyIndex++;
	}

	int restingNum = 0;

	for(int i=0; i<100; i++)
	{
		if(restingHistory[i])
		{
			restingNum++;
		}
	}

	if(restingNum > 50)
	{
		isUserResting = true;
	}
	else
	{
		isUserResting = false;
	}
}

XnBool CheckUserMovement(XnUserID player)
{
	XnSkeletonJointPosition posHead, posShoulderLeft, posShoulderRight, posHandLeft, posHandRight, posKneeLeft, posKneeRight;
	
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, XN_SKEL_HEAD, posHead);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, XN_SKEL_LEFT_SHOULDER, posShoulderLeft);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, XN_SKEL_RIGHT_SHOULDER, posShoulderRight);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, XN_SKEL_LEFT_HAND, posHandLeft);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, XN_SKEL_RIGHT_HAND, posHandRight);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, XN_SKEL_LEFT_KNEE, posKneeLeft);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, XN_SKEL_RIGHT_KNEE, posKneeRight);

	double offsetHead, offsetShoulderLeft, offsetShoulderRight, offsetHandLeft, offsetHandRight, offsetKneeLeft, offsetKneeRight;

	if(posHead.fConfidence > 0.5)
	{
		offsetHead = GetPointDistance(prevHead, posHead.position);
		prevHead = posHead.position;
	}
	else
	{
		offsetHead = 0;
	}

	if(posShoulderLeft.fConfidence > 0.5)
	{
		offsetShoulderLeft = GetPointDistance(prevShoulderLeft, posShoulderLeft.position);
		prevShoulderLeft = posShoulderLeft.position;
	}
	else
	{
		offsetShoulderLeft = 0;
	}

	if(posShoulderRight.fConfidence > 0.5)
	{
		offsetShoulderRight = GetPointDistance(prevShoulderRight, posShoulderRight.position);
		prevShoulderRight =  posShoulderRight.position;
	}
	else
	{
		offsetShoulderRight = 0;
	}

	if(posHandLeft.fConfidence > 0.5)
	{
		offsetHandLeft = GetPointDistance(prevHandLeft, posHandLeft.position);
		prevHandLeft = posHandLeft.position;
	}
	else
	{
		offsetHandLeft = 0;
	}

	if(posHandRight.fConfidence > 0.5)
	{
		offsetHandRight = GetPointDistance(prevHandRight, posHandRight.position);
		prevHandRight = posHandRight.position;
	}
	else
	{
		offsetHandRight = 0;
	}
	
	if(posKneeLeft.fConfidence > 0.5)
	{
		offsetKneeLeft = GetPointDistance(prevKneeLeft, posKneeLeft.position);
		prevKneeLeft = posKneeLeft.position;
	}
	else
	{
		offsetKneeLeft = 0;
	}

	if(posKneeRight.fConfidence > 0.5)
	{
		offsetKneeRight = GetPointDistance(prevKneeRight, posKneeRight.position);
		prevKneeRight = posKneeRight.position;
	}
	else
	{
		offsetKneeRight = 0;
	}
	

	if(offsetHead < 20 && offsetHandLeft < 20 && offsetHandRight < 20 && offsetShoulderLeft < 20 && offsetShoulderRight < 20)
	{
		return false;
	} 
	else
	{
		return true;
	}
}


int CheckUserMoved(XnUserID player)
{
	XnBool current = CheckUserMovement(player);

	movementHistory[movementIndex] = current;
	if(movementIndex == 99)
	{
		movementIndex = 0;
	}
	else
	{
		movementIndex++;
	}

	int movementNum = 0;

	for(int i=0; i<100; i++)
	{
		if(movementHistory[i])
		{
			movementNum++;
		}
	}

	if(movementNum > 70)
	{
		isUserMoving = true;
	}
	else
	{
		isUserMoving = false;
	}

	return movementNum;
}

double GetHeadNeckAngle(XnUserID player)
{
	XnSkeletonJointPosition posHead, posNeck;
	
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, XN_SKEL_HEAD, posHead);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, XN_SKEL_NECK, posNeck);

	double longSide = GetPointDistance(posHead.position, posNeck.position);

	double shortSide = abs(posHead.position.Y - posNeck.position.Y);

	double angle = asin(shortSide/longSide) * 57.3;

	return angle;
}

XnBool CheckUserForward(XnUserID player)
{
	XnSkeletonJointPosition posHead, posNeck;
	
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, XN_SKEL_HEAD, posHead);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, XN_SKEL_NECK, posNeck);

	if(posHead.position.Z < posNeck.position.Z)
	{
		return true;
	}
	else
	{
		return false;
	}
}


void RecordAngle(XnUserID player, FILE *fp)
{
	XnBool current;
	double angleHeadNeckTorse, angleHeadNeckLeftShoulder, angleHeadNeckRightShoulder, angleNeckHipLeftKnee, angleNeckHipRightKnee;

	angleHeadNeckTorse = GetJointAngle(player, XN_SKEL_HEAD, XN_SKEL_NECK, XN_SKEL_TORSO);
	angleHeadNeckLeftShoulder = GetJointAngle(player, XN_SKEL_HEAD, XN_SKEL_NECK, XN_SKEL_LEFT_SHOULDER);
	angleHeadNeckRightShoulder = GetJointAngle(player, XN_SKEL_HEAD, XN_SKEL_NECK, XN_SKEL_RIGHT_SHOULDER);
	angleNeckHipLeftKnee = GetJointAngle(player, XN_SKEL_NECK, XN_SKEL_LEFT_HIP, XN_SKEL_LEFT_KNEE);
	angleNeckHipRightKnee = GetJointAngle(player, XN_SKEL_NECK, XN_SKEL_RIGHT_HIP, XN_SKEL_RIGHT_KNEE);

/*
	if((angleNeckHipRightKnee > 100 && angleNeckHipRightKnee < 140 ) || (angleNeckHipLeftKnee > 90 && angleNeckHipLeftKnee < 140) )
	{
		current = true;
	}
	else
	{
		current = false;
	}
*/	
	double angleHeadNeck = GetHeadNeckAngle(player);

	printf("%.2f\n", angleHeadNeck);

	if(angleHeadNeck < 75 && !CheckUserForward(player))
	{
		current = true;
	}
	else
	{
		current = false;
	}

	CheckUserResting(player, current);
	int count = CheckUserMoved(player);

	char output[100] = "";
	xnOSMemSet(output, 0, sizeof(output));
	sprintf(output, "%.2f   %.2f   %.2f   %.2f   %.2f   %d\n", angleHeadNeckTorse, angleHeadNeckLeftShoulder, angleHeadNeckRightShoulder, angleNeckHipLeftKnee, angleNeckHipRightKnee, count);

	fprintf(fp, output);
}


// DrawDepthMap function draws both the depth map and user skeleton
void DrawDepthMap(const xn::DepthMetaData& dmd, const xn::SceneMetaData& smd, FILE *fp)
{
	// bInitialized is static, and will keep the initialization status
	static bool bInitialized = false;	
	static GLuint depthTexID;
	static unsigned char* pDepthTexBuf;
	static int texWidth, texHeight;

	float topLeftX;
	float topLeftY;
	float bottomRightY;
	float bottomRightX;
	float texXpos;
	float texYpos;

	// initialization process
	// executed only once
	if(!bInitialized)
	{
		texWidth =  getClosestPowerOfTwo(dmd.XRes());
		texHeight = getClosestPowerOfTwo(dmd.YRes());

//		printf("Initializing depth texture: width = %d, height = %d\n", texWidth, texHeight);
		depthTexID = initTexture((void**)&pDepthTexBuf,texWidth, texHeight) ;

//		printf("Initialized depth texture: width = %d, height = %d\n", texWidth, texHeight);
		bInitialized = true;

		topLeftX = dmd.XRes();
		topLeftY = 0;
		bottomRightY = dmd.YRes();
		bottomRightX = 0;
		texXpos =(float)dmd.XRes()/texWidth;
		texYpos  =(float)dmd.YRes()/texHeight;

		memset(texcoords, 0, 8*sizeof(float));
		texcoords[0] = texXpos, texcoords[1] = texYpos, texcoords[2] = texXpos, texcoords[7] = texYpos;
	}

	unsigned int nValue = 0;
	unsigned int nHistValue = 0;
	unsigned int nIndex = 0;
	unsigned int nX = 0;
	unsigned int nY = 0;
	unsigned int nNumberOfPoints = 0;
	XnUInt16 g_nXRes = dmd.XRes();
	XnUInt16 g_nYRes = dmd.YRes();

	unsigned char* pDestImage = pDepthTexBuf;

	// pDepth is a pointer to the first pixel in depth data stream
	const XnDepthPixel* pDepth = dmd.Data();

	// likewise, pLabels is the pointer to the first element in scene data stream
	const XnLabel* pLabels = smd.Data();

	// Calculate the accumulative histogram
	memset(g_pDepthHist, 0, MAX_DEPTH*sizeof(float));
	for (nY=0; nY<g_nYRes; nY++)
	{
		for (nX=0; nX<g_nXRes; nX++)
		{
			nValue = *pDepth;

			if (nValue != 0)
			{
				g_pDepthHist[nValue]++;
				nNumberOfPoints++;
			}

			pDepth++;
		}
	}

	for (nIndex=1; nIndex<MAX_DEPTH; nIndex++)
	{
		g_pDepthHist[nIndex] += g_pDepthHist[nIndex-1];
	}

	if (nNumberOfPoints)
	{
		for (nIndex=1; nIndex<MAX_DEPTH; nIndex++)
		{
			g_pDepthHist[nIndex] = (unsigned int)(256 * (1.0f - (g_pDepthHist[nIndex] / nNumberOfPoints)));
		}
	}

	pDepth = dmd.Data();
	if (g_bDrawPixels)
	{
		XnUInt32 nIndex = 0;
		// Prepare the texture map
		for (nY=0; nY<g_nYRes; nY++)
		{
			for (nX=0; nX < g_nXRes; nX++, nIndex++)
			{

				pDestImage[0] = 0;
				pDestImage[1] = 0;
				pDestImage[2] = 0;
				if (g_bDrawBackground || *pLabels != 0)
				{
					nValue = *pDepth;
					XnLabel label = *pLabels;
					XnUInt32 nColorID = label % nColors;
					if (label == 0)
					{
						nColorID = nColors;
					}

					if (nValue != 0)
					{
						nHistValue = g_pDepthHist[nValue];

						if(nColorID == 1 || nColorID == 2)
						{
							if(!isUserMoving && isUserResting)
							{
								pDestImage[0] = nHistValue * Colors[2][0]; 
								pDestImage[1] = nHistValue * Colors[2][1];
								pDestImage[2] = nHistValue * Colors[2][2];
							}
							else
							{
								pDestImage[0] = nHistValue * Colors[3][0]; 
								pDestImage[1] = nHistValue * Colors[3][1];
								pDestImage[2] = nHistValue * Colors[3][2];	
							}
						}
						else
						{
							pDestImage[0] = nHistValue * Colors[nColorID][0]; 
							pDestImage[1] = nHistValue * Colors[nColorID][1];
							pDestImage[2] = nHistValue * Colors[nColorID][2];
						}
					}
				}

				pDepth++;
				pLabels++;
				pDestImage+=3;
			}

			// bypass the gap between texture and real depthing imgae
			pDestImage += (texWidth - g_nXRes) *3;
		}
	}
	else
	{
		xnOSMemSet(pDepthTexBuf, 0, 3*2*g_nXRes*g_nYRes);
	}

	glBindTexture(GL_TEXTURE_2D, depthTexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pDepthTexBuf);

	// Display the OpenGL texture map
	glColor4f(0.75,0.75,0.75,1);

	// draw texture
	glEnable(GL_TEXTURE_2D);
	DrawTexture(dmd.XRes(),dmd.YRes(),0,0);	
	glDisable(GL_TEXTURE_2D);

	char strLabel[50] = "";
	XnUserID aUsers[15];
	XnUInt16 nUsers = 1;

	// get user id and store in aUsers array 
	g_UserGenerator.GetUsers(aUsers, nUsers);
	for (int i = 0; i < nUsers; ++i)
	{
#ifndef USE_GLES
		if (g_bPrintID)
		{
			XnPoint3D com;
			// get the xyz coordinates of user vector
			g_UserGenerator.GetCoM(aUsers[i], com);

			// map user vector coordinates to screen position
			g_DepthGenerator.ConvertRealWorldToProjective(1, &com, &com);

			xnOSMemSet(strLabel, 0, sizeof(strLabel));

			if(isUserResting && !isUserMoving)
			{
				sprintf(strLabel, "Resting");
			}
			else
			{
				sprintf(strLabel, "Active");
			}

			glColor4f(1-Colors[i%nColors][0], 1-Colors[i%nColors][1], 1-Colors[i%nColors][2], 1);

			glRasterPos2i(com.X, com.Y);

			glPrintString(GLUT_BITMAP_HELVETICA_18, strLabel);
		}
#endif
		if (g_bDrawSkeleton && g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i]))
		{
#ifndef USE_GLES
			glBegin(GL_LINES);
#endif
			if(isUserResting && !isUserMoving)
			{
				glColor4f(0.0, 1.0, 0.0, 1.0);
			}
			else
			{
				glColor4f(1.0, 1.0, 0.0, 1.0);
			}

			// draw user head 
			
			/*DrawLimb(aUsers[i], XN_SKEL_HEAD, XN_SKEL_NECK);
			

			DrawLimb(aUsers[i], XN_SKEL_NECK, XN_SKEL_LEFT_SHOULDER);
			DrawLimb(aUsers[i], XN_SKEL_LEFT_SHOULDER, XN_SKEL_LEFT_ELBOW);
			DrawLimb(aUsers[i], XN_SKEL_LEFT_ELBOW, XN_SKEL_LEFT_HAND);

			DrawLimb(aUsers[i], XN_SKEL_NECK, XN_SKEL_RIGHT_SHOULDER);
			DrawLimb(aUsers[i], XN_SKEL_RIGHT_SHOULDER, XN_SKEL_RIGHT_ELBOW);
			DrawLimb(aUsers[i], XN_SKEL_RIGHT_ELBOW, XN_SKEL_RIGHT_HAND);

			DrawLimb(aUsers[i], XN_SKEL_LEFT_SHOULDER, XN_SKEL_TORSO);
			DrawLimb(aUsers[i], XN_SKEL_RIGHT_SHOULDER, XN_SKEL_TORSO);

			DrawLimb(aUsers[i], XN_SKEL_TORSO, XN_SKEL_LEFT_HIP);
			DrawLimb(aUsers[i], XN_SKEL_LEFT_HIP, XN_SKEL_LEFT_KNEE);
			DrawLimb(aUsers[i], XN_SKEL_LEFT_KNEE, XN_SKEL_LEFT_FOOT);

			DrawLimb(aUsers[i], XN_SKEL_TORSO, XN_SKEL_RIGHT_HIP);
			DrawLimb(aUsers[i], XN_SKEL_RIGHT_HIP, XN_SKEL_RIGHT_KNEE);
			DrawLimb(aUsers[i], XN_SKEL_RIGHT_KNEE, XN_SKEL_RIGHT_FOOT);

			DrawLimb(aUsers[i], XN_SKEL_LEFT_HIP, XN_SKEL_RIGHT_HIP);*/
#ifndef USE_GLES
			glEnd();
#endif
			//DrawJoint(aUsers[i], XN_SKEL_HEAD);

			RecordAngle(aUsers[i], fp);
		}
	}
}

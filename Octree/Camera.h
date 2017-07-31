#ifndef _CAMERA_H
#define _CAMERA_H

#include <mmsystem.h>
#include "quat.h"
#include "matrix.h"



class CCamera {

public:

	CCamera();

	float pi;
	float radian ;
	

	CVector3 Position() {	return m_vPosition;		}
	CVector3 View()		{	return m_vView;			}
	CVector3 UpVector() {	return m_vUpVector;		}
	CVector3 Strafe()	{	return m_vStrafe;		}
	
	void PositionCamera(float positionX, float positionY, float positionZ,
			 		    float viewX,     float viewY,     float viewZ,
						float upVectorX, float upVectorY, float upVectorZ);

	void RotateView(float angle, float X, float Y, float Z);

	void SetViewByMouse(); 

	void RotateAroundPoint(CVector3 vCenter, float X, float Y, float Z);

	void StrafeCamera(float speed);

	void MoveCamera(float speed);

	void CheckForMovement();

	void Update();
	void Update2();

	void Look();

	void QuatRotate();


	void SetPosition(float positionX, float positionY, float positionZ)
	{
		m_vPosition.x = positionX;
		m_vPosition.y = positionY;
		m_vPosition.z = positionZ;
	}

	void SetView(float viewX, float viewY, float viewZ)
	{
		m_vView.x = viewX;
		m_vView.y = viewY;
		m_vView.z = viewZ;

		CVector3 vView = m_vView - m_vPosition;
		float length = vView.MagnitudeVector();
		vView = Normalize(vView);
		//float length2 = vView.MagnitudeVector();

		float length2;

		if(vView.y > 0.8)
		{
			vView.y = 0.79999;
			length2 = vView.MagnitudeVector();
			vView.x *= ((1/length2)*length);
			vView.y *= ((1/length2)*length);
			vView.z *= ((1/length2)*length);

			m_vView = vView + m_vPosition;
		}
	}

	QUAT Orientation;

	float Delta_x;   //Rotation deltas  
    float Delta_y;
    float Delta_z;

    float Multiplier;

	MATRIX Matrix;



	/*void Rotate()
	{
		

		QUAT temp_quat;
		temp_quat.EulerToQuat(Delta_x * radian * Multiplier, Delta_y * radian * Multiplier, Delta_z * radian * Multiplier);
		Orientation.MultQuat(temp_quat);
	}*/

	//void UpdatePosition()
	//{
	//	if (Movement_x != 0)
	//		MoveX();
	//	if (Movement_y != 0)
	//		MoveY();
	//	if (Movement_z != 0)
	//		MoveZ();

	//	Movement_x = 0.0;
	//	Movement_y = 0.0;
	//	Movement_z = 0.0;
	//}

	//VECTOR PositionV;
 //            
 //        float Movement_x;    //Movement displacements
 //       float Movement_y;
 //       float Movement_z;


	//	void MoveX()
	//	{
	//		float DirX;
	//		float DirY;
	//		float DirZ;
	//		float W;
	//		float X;
	//		float Y;
	//		float Z;
	//		QUAT TempQuat;
	//		QUAT TempQuat2;
	//		TempQuat = Orientation;
	//		TempQuat2.EulerToQuat(0.0, -90.0*(pi/180), 0.0);
	//		TempQuat.MultQuat(TempQuat2);
	//		W = TempQuat.w;
	//		X = TempQuat.x;
	//		Y = TempQuat.y;
	//		Z = TempQuat.z;
	//		DirX = 2.0 * ( X * Z - W * Y );
	//		DirY = 2.0 * ( Y * Z + W * X );
	//		DirZ = 1.0 - 2.0 * ( X * X + Y * Y );
	//		PositionV.x += DirX * Movement_x * Multiplier;
	//		PositionV.y += DirY * Movement_x * Multiplier;
	//		PositionV.z += DirZ * Movement_x * Multiplier;
	//	}

	//	void MoveY()
	//	{
	//		float DirX;
	//		float DirY;
	//		float DirZ;
	//		float W;
	//		float X;
	//		float Y;
	//		float Z;
	//		QUAT TempQuat;
	//		QUAT TempQuat2;
	//		TempQuat = Orientation;
	//		TempQuat2.EulerToQuat(90.0*(pi/180), 0.0, 0.0);
	//		TempQuat.MultQuat(TempQuat2);
	//		W = TempQuat.w;
	//		X = TempQuat.x;
	//		Y = TempQuat.y;
	//		Z = TempQuat.z;
	//		DirX = 2.0 * ( X * Z - W * Y );
	//		DirY = 2.0 * ( Y * Z + W * X );
	//		DirZ = 1.0 - 2.0 * ( X * X + Y * Y );
	//		PositionV.x += DirX * Movement_y * Multiplier;
	//		PositionV.y += DirY * Movement_y * Multiplier;
	//		PositionV.z += DirZ * Movement_y * Multiplier;
	//	}

	//	void MoveZ()
	//	{
	//		float DirX;
	//		float DirY;
	//		float DirZ;
	//		float W = Orientation.w;
	//		float X = Orientation.x;
	//		float Y = Orientation.y;
	//		float Z = Orientation.z;
	//		DirX = 2.0 * ( X * Z - W * Y );
	//		DirY = 2.0 * ( Y * Z + W * X );
	//		DirZ = 1.0 - 2.0 * ( X * X + Y * Y );
	//		PositionV.x += DirX * Movement_z * Multiplier;
	//		PositionV.y += DirY * Movement_z * Multiplier;
	//		PositionV.z += DirZ * Movement_z * Multiplier;
	//	}

	//	void Apply();

private:

	CVector3 m_vPosition;					

	CVector3 m_vView;						

	CVector3 m_vUpVector;		
	
	CVector3 m_vStrafe;						
};

#endif

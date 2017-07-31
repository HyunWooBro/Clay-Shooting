#pragma comment(lib, "winmm.lib")

#include "main.h"
#include "Camera.h"
#include "ZFLog.h"

extern float angleY;						
extern float angleZ;		
extern float currentRotX;

extern bool check;

extern game_mode_type game_mode;;


float kSpeed = 5.0f; // �̵� �ӵ� ����

float g_FrameInterval = 0.0f;

extern char g_strFrameRate[255];

void CalculateFrameRate() //������ ������ ���� �������� ��� �ӵ��� ���̸� �����ֱ� ���� �Լ�

{
	static float framesPerSecond    = 0.0f;	
    static float lastTime			= 0.0f;	
	static char strFrameRate[50] = {0};	

	static float frameTime = 0.0f;		

    float currentTime = timeGetTime() * 0.001f;	//timeGetTime()�� mmsystem.h, winmm.lib �߰��ؾ���
    //GetTickTime()�� �����ǵ� cpu�� �ð��� �����ϰ� ���ϰ��� ���̶�µ� �� �𸣰ʹ�. 
			

	// ������ ���͹��� ���� �ð��� ������ Ÿ���� ����...
 	g_FrameInterval = currentTime - frameTime;
	frameTime = currentTime;

    ++framesPerSecond;

    if( currentTime - lastTime > 1.0f )
    {
	    lastTime = currentTime;
		
		// window title bar�� fps���...
		sprintf(g_strFrameRate, "%d", int(framesPerSecond));

        framesPerSecond = 0;
    }
}


CCamera::CCamera() //������....
{
	CVector3 vZero = CVector3(0.0, 0.0, 0.0);	
	CVector3 vView = CVector3(0.0, 1.0, 0.5);	
	CVector3 vUp   = CVector3(0.0, 0.0, 1.0); 

	m_vPosition	= vZero;			
	m_vView		= vView;		
	m_vUpVector	= vUp;			


	/*Orientation.Reset();
    PositionV.Reset();
        Delta_x = 0.0;
        Delta_y = 0.0;
        Delta_z = 0.0;
        Matrix.LoadIdentity();

		pi = 3.141592;
		radian = pi / 180;*/

}



void CCamera::PositionCamera(float positionX, float positionY, float positionZ,
				  		     float viewX,     float viewY,     float viewZ,
							 float upVectorX, float upVectorY, float upVectorZ)
//glLookAt()�� ���� ����� �ϴ� �Լ�....
{
	CVector3 vPosition	= CVector3(positionX, positionY, positionZ);
	CVector3 vView		= CVector3(viewX, viewY, viewZ);
	CVector3 vUpVector	= CVector3(upVectorX, upVectorY, upVectorZ);
    

	m_vPosition = vPosition;				
	m_vView     = vView;				
	m_vUpVector = vUpVector;	
}



void CCamera::SetViewByMouse() //���콺 �����͸� �����ϱ� ���� �Լ�
{
	POINT mousePos;							
	int middleX = SCREEN_WIDTH  >> 1;		
	int middleY = SCREEN_HEIGHT >> 1;	//��Ʈ���� ������ ����Ʈ ����	2�� �����Ͱ� ���� ���
	//���� ���α׷��ֿ����� ����Ʈ������ ����ϴ°� �� ��������.  >>2�� 4�� �����Ͱ� ����. 16������ �ٲٱ� ��Ʈ�� �̵�
	//float angleY = 0.0f;						
	//float angleZ = 0.0f;					
	//static float currentRotX = 0.0f;

	
	//���� ���콺 x,y��ġ ���� �޴´�. 
	GetCursorPos(&mousePos);						
	
	//GetCursorPos()���� ���콺 ��ġ�� middleX,Y�� ��ġ�ǰ� ����
	if( (mousePos.x == middleX) && (mousePos.y == middleY) ) 
	{
		check = false;
		return;
	}
	check = true;

	SetCursorPos(middleX, middleY);	//���콺 �����͸� �׻� �߾����� ������. �׷� ���⼭ �����Ҽ� �ִ°� �� �Լ��� �޽���                           
	// �������� �׻� �޾Ƽ� ó���� �� �شٴ°� �����Ҽ� �ִ�.

	angleY = (float)( (middleX - mousePos.x) ) / 500.0f;		
	angleZ = (float)( (middleY - mousePos.y) ) / 500.0f;		

	currentRotX -= angleZ;  

	CVector3 vView = m_vView - m_vPosition;
	CVector3 vView2 = vView;
	vView2.y = 0;

	//float length = vView.MagnitudeVector();
	vView = Normalize(vView);
	vView2 = Normalize(vView2);

	float degree = vView.Degree(vView, vView2);

	//DotProduc
	//float length2 = vView.MagnitudeVector();

	float length2;
	
	

	//g_pLog->Log(0, "m_vView.y : %f", m_vView.y - m_vPosition.y);

	//if(degree > 70.0)
	{
		//m_vView.y = m_vPosition.y;
		//m_vView = Normalize(m_vView);

		//m_vUpVector = Cross(m_vStrafe  , m_vView - m_vPosition);
		//m_vUpVector = Normalize(m_vUpVector);

		//Look();

		//CVector3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
		//vAxis = Normalize(vAxis);
		//if(m_vView.y > m_vPosition.y)
		{
			//m_vView.y = m_vPosition.y+0.6;

			//m_vView.y = m_vPosition.y;

			//m_vUp

			//Look();
			//RotateView(abs(degree-70.0), vAxis.x, vAxis.y, vAxis.z);
			//RotateView(-70.0, vAxis.x, vAxis.y, vAxis.z);

			//glRotatef(-68.0, vAxis.x, vAxis.y, vAxis.z);

		}
		//else
		{
			//m_vView.y = m_vPosition.y;

			//Look();
			//RotateView(-abs(degree-70.0), vAxis.x, vAxis.y, vAxis.z);
		}
		//RotateView(-70.0, vAxis.x, vAxis.y, vAxis.z);

		

		//vView2
		//vView.y = 0.77;
		/*length2 = vView.MagnitudeVector();
		vView.x *= ((1/length2)*length);
		vView.y *= ((1/length2)*length);
		vView.z *= ((1/length2)*length);*/

		//acosf(
			//radToDeg

		//m_vView = vView + m_vPosition;
	}
	//else if(m_vView.y < m_vPosition.y - 70.0)
	//{
	//	m_vView.y = m_vPosition.y - 69.9;
	//	//g_pLog->Log(0, "degree : %f", degree);
	//	
	//	//vView.y = -0.77;
	//	/*length2 = vView.MagnitudeVector();
	//	vView.x *= ((1/length2)*length);
	//	vView.y *= ((1/length2)*length);
	//	vView.z *= ((1/length2)*length);*/

	//	/*CVector3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
	//	vAxis = Normalize(vAxis);
	//	RotateView(degree, vAxis.x, vAxis.y, vAxis.z);*/
	//	//RotateView(70.0, vAxis.x, vAxis.y, vAxis.z);

	//	//m_vView = vView + m_vPosition;
	//}
	/*if(currentRotX > 1.0f)
		currentRotX = 1.0f;
	else if(currentRotX < -1.0f)
		currentRotX = -1.0f;*/
	//else
	//{
		//g_pLog->Log(0, "degree : %f", degree);
		CVector3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);//�� ������ ������ ���Ѵ�. �׷� ������ �� �ҽ��� ��
        //���� �Ǿ��ڳ׿�!!! �̰� �� �ϳ��� ȸ���� �ϱ� �ϴµ� ȸ���ϴ� ���� ����� �ִ°ſ���.
        //���콺�� ���ؼ� �����̴� ������ y���� �������� �� �׳� �Ϲ����� RotateView(angleY, 0, 1, 0)�� �ֱ� Cross�� 
        //������� ���� ���Ϳ��� ������ ���� �������� �ϰ� middleY - mousePos.y�� ������ �� ȸ�� �� ���� �ʿ��ϴ�.
		//g_pLog->Log(0, "angZ : %f, angY : %f c : %f", angleZ, angleY, currentRotX);
		vAxis = Normalize(vAxis);//�ุ �ʿ��ϱ� ���� 1�̿��� �Ǵѱ� �������ͷ�... ����� �߰��� �Ǿ��ڱ���.
		RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
		RotateView(angleY, 0, 1, 0);
	//}

}


void CCamera::RotateView(float angle, float x, float y, float z)
{
	CVector3 vNewView;

	CVector3 vView = m_vView - m_vPosition;		

	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);

	vNewView.x  = (cosTheta + (1 - cosTheta) * x * x)		* vView.x;
	vNewView.x += ((1 - cosTheta) * x * y - z * sinTheta)	* vView.y;
	vNewView.x += ((1 - cosTheta) * x * z + y * sinTheta)	* vView.z;

	vNewView.y  = ((1 - cosTheta) * x * y + z * sinTheta)	* vView.x;
	vNewView.y += (cosTheta + (1 - cosTheta) * y * y)		* vView.y;
	vNewView.y += ((1 - cosTheta) * y * z - x * sinTheta)	* vView.z;

	vNewView.z  = ((1 - cosTheta) * x * z - y * sinTheta)	* vView.x;
	vNewView.z += ((1 - cosTheta) * y * z + x * sinTheta)	* vView.y;
	vNewView.z += (cosTheta + (1 - cosTheta) * z * z)		* vView.z;

	m_vView = m_vPosition + vNewView;

	if(game_mode == MENU)
	{
		m_vView.x = 0.0;
		m_vView.y = 0.0;
		m_vView.z = -1.0;
	}

}



void CCamera::StrafeCamera(float speed)
{	
	m_vPosition.x += m_vStrafe.x * speed;
	m_vPosition.z += m_vStrafe.z * speed;

	m_vView.x += m_vStrafe.x * speed;
	m_vView.z += m_vStrafe.z * speed;
}



void CCamera::MoveCamera(float speed)
{
	CVector3 vVector = m_vView - m_vPosition;
	vVector = Normalize(vVector);

	m_vPosition.x += vVector.x * speed;		
	m_vPosition.y += vVector.y * speed;	
	m_vPosition.z += vVector.z * speed;		
	m_vView.x += vVector.x * speed;		
	m_vView.y += vVector.y * speed;		
	m_vView.z += vVector.z * speed;		
}



void CCamera::CheckForMovement()
{	
	float speed = kSpeed * g_FrameInterval;

	if(GetKeyState(VK_UP) & 0x80 || GetKeyState('W') & 0x80) {				
		MoveCamera(speed);				
	}

	if(GetKeyState(VK_DOWN) & 0x80 || GetKeyState('S') & 0x80) {			
		MoveCamera(-speed);				
	}

	if(GetKeyState(VK_LEFT) & 0x80 || GetKeyState('A') & 0x80) {			
		StrafeCamera(-speed);
	}

	if(GetKeyState(VK_RIGHT) & 0x80 || GetKeyState('D') & 0x80) {			
		StrafeCamera(speed);
	}	
}



void CCamera::Update() 
{
	CVector3 vCross = Cross(m_vView - m_vPosition, m_vUpVector);

	m_vStrafe = Normalize(vCross);

	SetViewByMouse();

	CheckForMovement();
	
	CalculateFrameRate();
}


void CCamera::Look()
{
	gluLookAt(m_vPosition.x, m_vPosition.y, m_vPosition.z,	
			  m_vView.x,	 m_vView.y,     m_vView.z,	
			  m_vUpVector.x, m_vUpVector.y, m_vUpVector.z);
}

//void CCamera::QuatRotate()
//{
//	float pi = 3.141592;
//	float radian = pi / 180;
//	
//	QUAT temp_quat;
//    temp_quat.EulerToQuat(Delta_x * radian * Multiplier, Delta_y * radian * Multiplier, Delta_z * radian * Multiplier);
//    Orientation.MultQuat(temp_quat);
//}
//
//void CCamera::Update2()
//{
//    Rotate();
//
//    UpdatePosition();
//}
//
//void CCamera::Apply()
//{
//    Matrix.QuatToMatrix(Orientation);
//    Matrix.MatrixInverse();
//
//    glLoadMatrixf(Matrix.Element);
//    glTranslatef(-PositionV.x,-PositionV.y,-PositionV.z);
//}

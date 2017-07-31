#include <windows.h>
//#include "bullet.h"

#include "decal.h"
#include "vector.h"
#include "vertex.h"
//#include "polygon.h"
#include "texture.h"
//#include "listnode.h"
//#include "collision.h"
#include "general.h"
//#include "Camera.h"
//#include "bsp.h"
#include "tll.h"

#include "matrix.h"

#include "main.h"
#include "Camera.h"
#include "3ds.h"
#include "ZFLog.h"


extern TEXTURE* texture;
extern GLuint texture_[1];

extern DECAL muzzleflashdecal;
extern DECAL shootingman;

extern GLfloat lightposition[3];

extern CCamera	g_Camera;

extern float angleY;						
extern float angleZ;
extern float currentRotX;

extern bool check;

DECAL::DECAL()
{
	
}

DECAL::~DECAL()
{
}

int DECAL::Compare(const DECAL& Decal)
{
    if (linkPosition < Decal.linkPosition)
            return smaller;
      if (linkPosition > Decal.linkPosition)
            return bigger;
      else
            return same;
}

void RotateView2(float angle, float x, float y, float z)
{
	/*CVector3 vNewView;

	CVector3 vView = g_Camera.View()-g_Camera.Position();

	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);*/

	MATRIX Matrix;
	memset(Matrix.Element, 0, sizeof(Matrix.Element));

	/*Matrix.Element[0]  = (cosTheta + (1 - cosTheta) * x * x)	;
	Matrix.Element[1] = ((1 - cosTheta) * x * y - z * sinTheta);
	Matrix.Element[2] = ((1 - cosTheta) * x * z + y * sinTheta);

	Matrix.Element[4]  = ((1 - cosTheta) * x * y + z * sinTheta);
	Matrix.Element[5] = (cosTheta + (1 - cosTheta) * y * y);
	Matrix.Element[6] = ((1 - cosTheta) * y * z - x * sinTheta);

	Matrix.Element[8]  = ((1 - cosTheta) * x * z - y * sinTheta);
	Matrix.Element[9] = ((1 - cosTheta) * y * z + x * sinTheta);
	Matrix.Element[10] = (cosTheta + (1 - cosTheta) * z * z);*/

	
	CVector3 vU = g_Camera.Strafe();
	CVector3 vV = Cross(vU, g_Camera.View()-g_Camera.Position());
		//g_Camera.UpVector();
	
	CVector3 vN = Cross(vU, vV);

	vU = Normalize(vU);
	vV = Normalize(vV);
	vN = Normalize(vN);

	Matrix.Element[0] = vU.x;
	Matrix.Element[1] = vU.y;
	Matrix.Element[2] = vU.z;

	Matrix.Element[4] = vV.x;
	Matrix.Element[5] = vV.y;
	Matrix.Element[6] = vV.z;

	Matrix.Element[8] = vN.x;
	Matrix.Element[9] = vN.y;
	Matrix.Element[10] = vN.z;

	Matrix.Element[15] = 1;

	glMultMatrixf(Matrix.Element); // 행렬에 대해 더 확실한 예를 하나 외우자.

	//m_vView = m_vPosition + vNewView;
}

void DrawMuzzleFlash()
{
	CVector3 vVector = g_Camera.View()-g_Camera.Position();
	vVector = Normalize(vVector);

    MATRIX Matrix;
    glPushMatrix();
    glTranslatef(muzzleflashdecal.Position.x,muzzleflashdecal.Position.y,muzzleflashdecal.Position.z);
    //Matrix.QuatToMatrix(muzzleflashdecal.Orientation);
    //glMultMatrixf(Matrix.Element);
    //glTranslatef(-50.0,15.0,0.0);




	//POINT mousePos;							
	//int middleX = SCREEN_WIDTH  >> 1;		
	//int middleY = SCREEN_HEIGHT >> 1;	//비트레벨 오른쪽 쉬프트 연산	2로 나눈것과 같은 기능
	//게임 프로그래밍에서는 쉬프트연산을 사용하는게 더 빠르데요.  >>2는 4로 나눈것과 같다. 16진수로 바꾸구 비트를 이동
	//float angleY = 0.0f;						
	//float angleZ = 0.0f;					
	//static float currentRotX = 0.0f;
	
	//현재 마우스 x,y위치 값을 받는다. 
	//GetCursorPos(&mousePos);						
	
	//GetCursorPos()받은 마우스 위치를 middleX,Y로 일치되게 설정
	//if( (mousePos.x == middleX) && (mousePos.y == middleY) ) return;
	//if(check == true)
	{

	//SetCursorPos(middleX, middleY);	//마우스 포인터를 항상 중앙으로 보낸다. 그럼 여기서 생각할수 있는게 이 함수는 메시지                           
	// 루프에서 항상 받아서 처리를 해 준다는걸 생각할수 있다.

	//angleY = (float)( (middleX - mousePos.x) ) / 500.0f;		
	//angleZ = (float)( (middleY - mousePos.y) ) / 500.0f;		

	//currentRotX -= angleZ;  

	/*if(currentRotX > 1.0f)
		currentRotX = 1.0f;
	else if(currentRotX < -1.0f)
		currentRotX = -1.0f;
	else*/
	{
		/*static float ii=0;
		ii += 0.1;
		if(ii > 360)
			ii = 0;*/

		CVector3 vAxis = Cross(g_Camera.View() - g_Camera.Position(), g_Camera.UpVector());//두 벡터의 외적을 구한다. 그럼 외적이 이 소스에 추
        //가가 되었겠네요!!! 이건 왜 하나면 회전을 하긴 하는데 회전하는 축을 만들어 주는거에요.
        //마우스에 의해서 움직이는 방향은 y축을 기준으로 한 그냥 일반적인 RotateView(angleY, 0, 1, 0)이 있구 Cross로 
        //만들어진 법선 벡터에서 나오는 축을 기준으로 하고 middleY - mousePos.y를 각도로 한 회전 두 개가 필요하다.
		
		//vAxis = Normalize(vAxis);//축만 필요하구 값은 1이여두 되닌까 단위벡터로... 여기두 추가가 되었겠군요.
		//g_pLog->Log(0, "angleZ : %f, angleY : %f", angleZ, angleY);
		/*RotateView2(angleZ*500, vAxis.x, vAxis.y, vAxis.z);
		RotateView2(angleY*500, 0, 1, 0);*/
		//glRotatef(angleZ*500, vAxis.x, vAxis.y, vAxis.z);
		RotateView2(angleY*500, 0, 1, 0);
	}
	}


	glTranslatef(0.0,-4.5,-10.0); // 왜 카메라를 기준으로 할까?

	//glRotatef(, 0, 1, 0);


	/*glTranslatef(DivideVectorByScaler(CVector3(vVector.x,vVector.y,vVector.z), 0.2).x,
		DivideVectorByScaler(CVector3(vVector.x,vVector.y,vVector.z), 0.2).y-2,
		DivideVectorByScaler(CVector3(vVector.x,vVector.y,vVector.z), 0.2).z);*/

	glEnable(GL_TEXTURE_2D);
    glDepthMask(0);
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0);
    glDisable(GL_LIGHTING);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, texture[0].TexID);
	
    glDisable(GL_CULL_FACE);
	//glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-muzzleflashdecal.Size, -muzzleflashdecal.Size, 0.0);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(muzzleflashdecal.Size, -muzzleflashdecal.Size, 0.0);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(muzzleflashdecal.Size, muzzleflashdecal.Size, 0.0);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-muzzleflashdecal.Size, muzzleflashdecal.Size, 0.0);
    glEnd();


	

	// Turn on texture mapping and turn off color
	//glEnable(GL_TEXTURE_2D);

	// Reset the color to normal again
	//glColor3ub(255, 255, 255);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); // DECAL모드
	//glBindTexture(GL_TEXTURE_2D, texture_[0]);
	/*glBegin(GL_TRIANGLES);
	{
		glTexCoord2f(0, 1);
		glVertex3f(-5.01, 5, 5.01);
		glTexCoord2f(0, 0);
		glVertex3f(-5.01, 5, -5.01);
		glTexCoord2f(1, 0);
		glVertex3f(5.01, 5, -5.01);
		glTexCoord2f(0, 1);
		glVertex3f(-5.01, 5, 5.01);
		glTexCoord2f(1, 0);
		glVertex3f(5.01, 5, -5.01);
		glTexCoord2f(1, 1);
		glVertex3f(5.01, 5, 5.01);
	}glEnd();*/


	//Sleep(1000);

    glEnable(GL_CULL_FACE);
    glDepthMask(1);
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
    glPopMatrix();

	////////////////////////////////////////////////////////////
}
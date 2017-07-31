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

	glMultMatrixf(Matrix.Element); // ��Ŀ� ���� �� Ȯ���� ���� �ϳ� �ܿ���.

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
	//int middleY = SCREEN_HEIGHT >> 1;	//��Ʈ���� ������ ����Ʈ ����	2�� �����Ͱ� ���� ���
	//���� ���α׷��ֿ����� ����Ʈ������ ����ϴ°� �� ��������.  >>2�� 4�� �����Ͱ� ����. 16������ �ٲٱ� ��Ʈ�� �̵�
	//float angleY = 0.0f;						
	//float angleZ = 0.0f;					
	//static float currentRotX = 0.0f;
	
	//���� ���콺 x,y��ġ ���� �޴´�. 
	//GetCursorPos(&mousePos);						
	
	//GetCursorPos()���� ���콺 ��ġ�� middleX,Y�� ��ġ�ǰ� ����
	//if( (mousePos.x == middleX) && (mousePos.y == middleY) ) return;
	//if(check == true)
	{

	//SetCursorPos(middleX, middleY);	//���콺 �����͸� �׻� �߾����� ������. �׷� ���⼭ �����Ҽ� �ִ°� �� �Լ��� �޽���                           
	// �������� �׻� �޾Ƽ� ó���� �� �شٴ°� �����Ҽ� �ִ�.

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

		CVector3 vAxis = Cross(g_Camera.View() - g_Camera.Position(), g_Camera.UpVector());//�� ������ ������ ���Ѵ�. �׷� ������ �� �ҽ��� ��
        //���� �Ǿ��ڳ׿�!!! �̰� �� �ϳ��� ȸ���� �ϱ� �ϴµ� ȸ���ϴ� ���� ����� �ִ°ſ���.
        //���콺�� ���ؼ� �����̴� ������ y���� �������� �� �׳� �Ϲ����� RotateView(angleY, 0, 1, 0)�� �ֱ� Cross�� 
        //������� ���� ���Ϳ��� ������ ���� �������� �ϰ� middleY - mousePos.y�� ������ �� ȸ�� �� ���� �ʿ��ϴ�.
		
		//vAxis = Normalize(vAxis);//�ุ �ʿ��ϱ� ���� 1�̿��� �Ǵѱ� �������ͷ�... ����� �߰��� �Ǿ��ڱ���.
		//g_pLog->Log(0, "angleZ : %f, angleY : %f", angleZ, angleY);
		/*RotateView2(angleZ*500, vAxis.x, vAxis.y, vAxis.z);
		RotateView2(angleY*500, 0, 1, 0);*/
		//glRotatef(angleZ*500, vAxis.x, vAxis.y, vAxis.z);
		RotateView2(angleY*500, 0, 1, 0);
	}
	}


	glTranslatef(0.0,-4.5,-10.0); // �� ī�޶� �������� �ұ�?

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
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); // DECAL���
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
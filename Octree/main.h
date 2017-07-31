#ifndef _MAIN_H
#define _MAIN_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gl\gl.h>							
#include <gl\glu.h>							
#include <gl\glaux.h>

#include <vector>

//#include "tll.h"
//#include "matrix.h"
#include <time.h>
using namespace std;

#define SCREEN_WIDTH 640						 
#define SCREEN_HEIGHT 480						

#define SCREEN_DEPTH 16							

#define MAX_TEXTURES 100

#define MAX_TARGET 10

#define MAX_MENU 6

#define MAX_FOVY_DEGREE		45.0
#define MIN_FOVY_DEGREE		25.0
#define DEGREE_PER_FRAME	6.0

#define MAX_STEP1_POSITION 3
#define MAX_STEP2_POSITION 4
#define MAX_STEP3_POSITION 4
#define MAX_STEP4_POSITION 5

#define STEP1_TARGET_RADIUS ((rand()%3+1)*1.5)
#define STEP2_TARGET_RADIUS ((rand()%3+1)*1.3)
#define STEP3_TARGET_RADIUS ((rand()%3+1)*1.1)
#define STEP4_TARGET_RADIUS ((rand()%3+1)*0.8)

#define STEP1_TARGET_VECTOR_Y_INCREMENT 0.5
#define STEP2_TARGET_VECTOR_Y_INCREMENT 0.4
#define STEP3_TARGET_VECTOR_Y_INCREMENT 0.3
#define STEP4_TARGET_VECTOR_Y_INCREMENT 0.2

#define STEP3_TARGET_DELAY (rand()%5500+rand()%500)
#define STEP4_TARGET_DELAY (rand()%6500+rand()%1000)

#define MAX_BULLETS 6
#define LOADING_TIME 1300

#define PI 3.141592

#define TARGET_SIZE 4

#define BULLET_WIDTH 2
#define BULLET_HEIGHT 1

struct CVector3
{
public:
	
	CVector3() {}

	CVector3(float X, float Y, float Z) 
	{ 
		x = X; y = Y; z = Z;
	}

	CVector3 operator+(CVector3 vVector)
	{
		return CVector3(vVector.x + x, vVector.y + y, vVector.z + z);
	}

	CVector3 operator-(CVector3 vVector)
	{
		return CVector3(x - vVector.x, y - vVector.y, z - vVector.z);
	}
	
	CVector3 operator*(float num)
	{
		return CVector3(x * num, y * num, z * num);
	}

	CVector3 operator/(float num)
	{
		return CVector3(x / num, y / num, z / num);
	}

	float MagnitudeVector()
	{
	  return(sqrt(x*x+y*y+z*z));
	}

	float DotProduct(CVector3 vec1, CVector3 vec2)
	{
		/*
		Dot Product of two Vectors.

		U = (Ux, Uy, Uz)
		V = (Vx, Vy, Vz)
		U*V = UxVx + UyVy + UzVz
		U*V = |U||V|cos(t) (where t is the angle theta between the two vectors)
		*/
		  float dot;
		  dot = vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
		  return dot;
	}

	CVector3 Normalize(CVector3 vec)
	{
		double Magnitude;							// This holds the magitude			

		Magnitude = vec.MagnitudeVector();					// Get the magnitude

		vec.x /= (float)Magnitude;				// Divide the vector's X by the magnitude
		vec.y /= (float)Magnitude;				// Divide the vector's Y by the magnitude
		vec.z /= (float)Magnitude;				// Divide the vector's Z by the magnitude

		return vec;
	}

	float Degree(CVector3 vec1, CVector3 vec2)
	{
		vec1 = Normalize(vec1);
		vec2 = Normalize(vec2);

		float dot = DotProduct(vec1, vec2);

		float rad = acosf(dot);

		float degree = rad * 180 / PI;

		return degree;
	}

	float x, y, z;						
};

struct TargetSetting
{
	CVector3 step1_position[MAX_STEP1_POSITION];
	CVector3 step1_vector[MAX_STEP1_POSITION];
	float step1_magnitude;

	CVector3 step2_position[MAX_STEP2_POSITION];
	CVector3 step2_vector[MAX_STEP2_POSITION];
	float step2_magnitude;

	CVector3 step3_position[MAX_STEP3_POSITION];
	CVector3 step3_vector[MAX_STEP3_POSITION];
	float step3_magnitude;

	CVector3 step4_position[MAX_STEP4_POSITION];
	CVector3 step4_vector[MAX_STEP4_POSITION];
	float step4_magnitude;
};


#include "Octree.h"

#include "Frustum.h"

enum game_mode_type {MENU, ANIMATION, SHOOTING};

enum order_type {SEQ, RAN};

enum menu_mode_type {MAIN, OPTION, HELP, CREDIT};

extern bool		g_bFullScreen;					
extern HWND		g_hWnd;						
extern RECT		g_rRect;					
extern HDC		g_hDC;						
extern HGLRC	g_hRC;					

extern class CDebug g_Debug;

extern class CFrustum g_Frustum;

extern UINT g_Texture[MAX_TEXTURES];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow);

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

WPARAM MainLoop();

void CreateTexture(UINT textureArray[], LPSTR strFileName, int textureID);

void ChangeToFullScreen();

HWND CreateMyWindow(LPSTR szWindowName, int width, int height, DWORD dwStyle, BOOL fullScreen, HINSTANCE hInstance);

BOOL bSetupPixelFormat(HDC hdc);

void SizeOpenGLScreen(int width, int height);

void InitializeOpenGL(int width, int height);

void Init(HWND hWnd);

void RenderScene();

void DrawUI();

void ProcessStage();

void ProcessStep();

void Zooming();

void CreateTexture(GLuint textureArray[], LPSTR strFileName, int textureID);

void DeInit();

#endif 


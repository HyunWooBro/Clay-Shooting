#include "GameMenu.h"
#include "Camera.h"
#include "ZFLog.h"
#include "decal.h"
#include "sound.h"

extern CCamera	g_Camera;

extern ZFLog* g_pLog;
extern bool  g_bFullScreen;
extern HWND  g_hWnd;
extern bool lbutten;

extern game_mode_type game_mode;
extern menu_mode_type menu_mode;


GameMenu::GameMenu(void)
{
	menu_rect[0].left = 517;	
	menu_rect[0].right = 573;
	menu_rect[0].bottom = 199;
	menu_rect[0].top = 225;
	is_menu_active[0] = true;

	menu_rect[1].left = 502;	
	menu_rect[1].right = 570;
	menu_rect[1].bottom = 234;
	menu_rect[1].top = 258;
	is_menu_active[1] = false;

	menu_rect[2].left = 524;	
	menu_rect[2].right = 570;
	menu_rect[2].bottom = 265;
	menu_rect[2].top = 291;
	is_menu_active[2] = true;

	menu_rect[3].left = 505;	
	menu_rect[3].right = 569;
	menu_rect[3].bottom = 300;
	menu_rect[3].top = 324;
	is_menu_active[3] = true;

	menu_rect[4].left = 525;	
	menu_rect[4].right = 566;
	menu_rect[4].bottom = 333;
	menu_rect[4].top = 357;
	is_menu_active[4] = true;

	menu_rect[5].left = 525;	
	menu_rect[5].right = 570;
	menu_rect[5].bottom = 358;
	menu_rect[5].top = 384;
	is_menu_active[5] = true;


	select = -1;
}

void RotateView3(float angle, float x, float y, float z)
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


GameMenu::~GameMenu(void)
{
}


void GameMenu::show()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();	
	

	glViewport(0,0,g_rRect.right,g_rRect.bottom);	

	glMatrixMode(GL_PROJECTION);	
	glLoadIdentity();		

	//gluPerspective(degree,(float)g_rRect.right/(float)g_rRect.bottom, 1, 500.0f);
	 //glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);  
	//glOrtho(-320.0,320.0,-120.0,120.0,1.0,500.0);
	//glOrtho(-320.0,320.0,-120.0,120.0,1.0,500.0);
	//gluPerspective(45.0f,(float)width/(float)height, 1, 500.0f);
	glOrtho(-320.0,320.0,-240.0,240.0,1.0,550.0);
	glMatrixMode(GL_MODELVIEW);						
	glLoadIdentity();

	//glTranslatef(g_Camera.Position().x, g_Camera.Position().y, g_Camera.Position().z);
	//RotateView3(500, 0, 1, 0);
	//glTranslatef(0, 0, -500);
 	

	g_Camera.Look();

	int middleX = SCREEN_WIDTH  >> 1;		
	int middleY = SCREEN_HEIGHT >> 1;


	POINT mousePos;							
	

	GetCursorPos(&mousePos);

	if(g_bFullScreen == false)
		ScreenToClient(g_hWnd, &mousePos);

	g_pLog->Log(0, "%d, %d", mousePos.x, mousePos.y);

	int i;
	int j = 0;

	static bool isin = false;
	static int num = -1;
	static int num2 = -1;

	if(menu_mode == MAIN)
	{
		glBindTexture(GL_TEXTURE_2D, g_Texture[9]);


		for(i=0; i<MAX_MENU-1; i++)
		{
			if(is_menu_active[i] == true && isInRect(menu_rect[i], mousePos) == true)
			{
				//BASS_ChannelGetFlags(
				if(isin == false || num != i)
					PlayChannel(channel_hover, false);

				
				j++;
				if(lbutten == false)
				{
					
					if(select == i) 
					{
					
						if(select == 0)			// start
						{
							//g_pLog->Log(0, " ggggggggggggg");
							game_mode = ANIMATION;
							//g_Camera.PositionCamera(-71, 5, 84,  -69.0, 4.6, 75.9,  0, 1, 0);
							g_Camera.PositionCamera(-71, 5, 84,  -73.0, 4.6, 77.9,  0, 1, 0);
							//g_Camera.Look();
							ShowCursor(FALSE);
							PlayChannel(channel_bird, false);
						}
						else if(select == 2)	// help
						{
							menu_mode = HELP;
						}
						else if(select == 3)	// credit
						{
							menu_mode = CREDIT;
						}
						else if(select == 4)	// exit
						{
							// 동적할당 정리
							g_pLog->Log(0, "jk");
							exit(0);
						}
					}
					select = -1;
					glBindTexture(GL_TEXTURE_2D, g_Texture[i+10]);
				}
				else
				{
					if(num2 == -1)
					{
						PlayChannel(channel_click, false);
						num2 = 0;
					}
					//PlayChannel(channel_click, false);
					g_pLog->Log(0, "%d", i);
					select = i;
					glBindTexture(GL_TEXTURE_2D, g_Texture[i+20]);
				}

				isin = true;
				num = i;
			}
		}
	}
	else if(menu_mode == HELP)
	{
		glBindTexture(GL_TEXTURE_2D, g_Texture[50]);
		if(is_menu_active[5] == true && isInRect(menu_rect[5], mousePos) == true)
		{
			if(isin == false)
				PlayChannel(channel_hover, false);
			j++;
			if(lbutten == false)
			{
				
				if(select == 1) 
				{
					menu_mode = MAIN;
					glBindTexture(GL_TEXTURE_2D, g_Texture[9]);
				}
				select = -1;
				glBindTexture(GL_TEXTURE_2D, g_Texture[51]);
			}
			else
			{
				if(num2 == -1)
				{
					PlayChannel(channel_click, false);
					num2 = 0;
				}
				select = 1;
				glBindTexture(GL_TEXTURE_2D, g_Texture[52]);
			}

			isin = true;
		}
	}
	else if(menu_mode == CREDIT)
	{
		glBindTexture(GL_TEXTURE_2D, g_Texture[60]);
		if(is_menu_active[5] == true && isInRect(menu_rect[5], mousePos) == true)
		{
			if(isin == false)
				PlayChannel(channel_hover, false);
			j++;
			if(lbutten == false)
			{
				
				if(select == 1) 
				{
					menu_mode = MAIN;
					glBindTexture(GL_TEXTURE_2D, g_Texture[9]);
				}
				select = -1;
				glBindTexture(GL_TEXTURE_2D, g_Texture[61]);
			}
			else
			{
				if(num2 == -1)
				{
					PlayChannel(channel_click, false);
					num2 = 0;
				}
				select = 1;
				glBindTexture(GL_TEXTURE_2D, g_Texture[62]);
			}

			isin = true;
		}
	}

	if(lbutten == false && j == 0)
		select = -1;

	if(lbutten == false)
		num2 = -1;

	if(j == 0)
		isin = false;

	// Turn on texture mapping and turn off color
	glEnable(GL_TEXTURE_2D);
	
	int env;
	glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &env);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); // DECAL모드



	
	glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-middleX, -middleY, 0.0);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(middleX, -middleY, 0.0);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(middleX, middleY, 0.0);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-middleX, middleY, 0.0);
    glEnd();

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, env);

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

	// Swap the backbuffers to the foreground
	SwapBuffers(g_hDC);			
}

bool GameMenu::isInRect(RECT rect, POINT pt)
{
	if(rect.left <= pt.x && pt.x <= rect.right)
	{
		if(rect.bottom <= pt.y && pt.y <= rect.top)
		{
			return true;
		}
	}

	return false;
}
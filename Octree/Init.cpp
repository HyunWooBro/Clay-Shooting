#include "main.h"


void CreateTexture(UINT textureArray[], LPSTR strFileName, int textureID)
{
	AUX_RGBImageRec *pBitmap = NULL;
	
	if(!strFileName)							
		return;

	pBitmap = auxDIBImageLoad(strFileName);		
	
	if(pBitmap == NULL)						
		exit(0);

	glGenTextures(1, &textureArray[textureID]);

	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

	glBindTexture(GL_TEXTURE_2D, textureArray[textureID]);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pBitmap->sizeX, pBitmap->sizeY, GL_RGB, GL_UNSIGNED_BYTE, pBitmap->data);
		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);


	if (pBitmap)								
	{
		if (pBitmap->data)							
		{
			free(pBitmap->data);			
		}

		free(pBitmap);							
	}
}



void ChangeToFullScreen()
{
	DEVMODE dmSettings;							

	memset(&dmSettings,0,sizeof(dmSettings));		

	if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dmSettings))
	{
		MessageBox(NULL, "Could Not Enum Display Settings", "Error", MB_OK);
		return;
	}

	dmSettings.dmPelsWidth	= SCREEN_WIDTH;			
	dmSettings.dmPelsHeight	= SCREEN_HEIGHT;			
	dmSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	int result = ChangeDisplaySettings(&dmSettings,CDS_FULLSCREEN);	

	if(result != DISP_CHANGE_SUCCESSFUL)
	{
		MessageBox(NULL, "Display Mode Not Compatible", "Error", MB_OK);
		PostQuitMessage(0);
	}
}


HWND CreateMyWindow(LPSTR szWindowName, int width, int height, DWORD dwStyle, BOOL fullScreen, HINSTANCE hInstance)
{
	HWND hwnd;
	WNDCLASS wndclass;
	
	memset(&wndclass, 0, sizeof(WNDCLASS));			
	wndclass.style = CS_HREDRAW | CS_VREDRAW;		
	wndclass.lpfnWndProc = WinProc;					
	wndclass.hInstance = hInstance;					
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);		
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);	
	wndclass.lpszClassName = szWindowName;			

	RegisterClass(&wndclass);						
	
	if(fullScreen && !dwStyle) 				
	{											
		dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		ChangeToFullScreen();					
		//ShowCursor(FALSE);						
	}
	else if(!dwStyle)				
		dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
												

	RECT rcWindow;
	rcWindow.left	= 0;	
	rcWindow.right	= width;	
	rcWindow.top	= 0;	
	rcWindow.bottom	= height;	

	AdjustWindowRect( &rcWindow, dwStyle, FALSE );	

	hwnd = CreateWindow(szWindowName,szWindowName, dwStyle, 0, 0,
						rcWindow.right  - rcWindow.left, 
						rcWindow.bottom - rcWindow.top, 
						NULL, NULL, hInstance, NULL);

	if(!hwnd) return NULL;						

	ShowWindow(hwnd, SW_SHOWNORMAL);					
	UpdateWindow(hwnd);							

	SetFocus(hwnd);								

	return hwnd;
}

BOOL bSetupPixelFormat(HDC hdc) 
{ 
    PIXELFORMATDESCRIPTOR pfd; 
    int pixelformat; 
 
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);		
    pfd.nVersion = 1;									
													
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
    pfd.dwLayerMask = PFD_MAIN_PLANE;			
    pfd.iPixelType = PFD_TYPE_RGBA;					
    pfd.cColorBits = SCREEN_DEPTH;				
    pfd.cDepthBits = SCREEN_DEPTH;				
    pfd.cAccumBits = 0;								
    pfd.cStencilBits = 0;							
 
    if ( (pixelformat = ChoosePixelFormat(hdc, &pfd)) == FALSE ) 
    { 
        MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
    if (SetPixelFormat(hdc, pixelformat, &pfd) == FALSE) 
    { 
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
    return TRUE;									
}

void SizeOpenGLScreen(int width, int height)		
{
	if (height==0)									
	{
		height=1;										
	}

	glViewport(0,0,width,height);				

	glMatrixMode(GL_PROJECTION);					
	glLoadIdentity();								

	gluPerspective(45.0f,(float)width/(float)height, 1, 500.0f);

	glMatrixMode(GL_MODELVIEW);						
	glLoadIdentity();					
	
}


void InitializeOpenGL(int width, int height) 
{  
    g_hDC = GetDC(g_hWnd);							
													
    if (!bSetupPixelFormat(g_hDC))				
        PostQuitMessage (0);					

	g_hRC = wglCreateContext(g_hDC);			
    wglMakeCurrent(g_hDC, g_hRC);	
		
	glCullFace(GL_BACK);		// 뒤면은 제거 대상
	glEnable(GL_CULL_FACE);		// 컬링 적용
	glEnable(GL_DEPTH_TEST);	// 은면 제거
	glEnable(GL_TEXTURE_2D);	// 2D 텍스쳐 활성화		

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);			

	float fogColor[4] = {0.5f, 0.5f, 0.5f, 1.0f};

	glFogi(GL_FOG_MODE, GL_EXP2);				
	glFogfv(GL_FOG_COLOR, fogColor);				
	glFogf(GL_FOG_DENSITY, 0.07f);					
	glHint(GL_FOG_HINT, GL_DONT_CARE);				
	glFogf(GL_FOG_START, 0);					
	glFogf(GL_FOG_END, 50.0f);					

	//glEnable(GL_FOG);								

	SizeOpenGLScreen(width, height);			
}


void DeInit()
{
	wglMakeCurrent(NULL, NULL);					

	if (g_hRC)									
		wglDeleteContext(g_hRC); 
	if (g_hDC) 
		ReleaseDC(g_hWnd, g_hDC);					
		
	if(g_bFullScreen)								
	{
		ChangeDisplaySettings(NULL,0);			
		ShowCursor(TRUE);							
	}
}



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{	
	HWND hWnd;

	int i;

	if((i=MessageBox(NULL, "풀스크린으로 할까요?", "Options", MB_YESNOCANCEL | MB_ICONQUESTION)) == IDNO)
		g_bFullScreen = false;
	else if(i == IDYES)
		g_bFullScreen = true;
	else
		exit(0);

	
	hWnd = CreateMyWindow("옥트리 테스트", SCREEN_WIDTH, SCREEN_HEIGHT, 0, g_bFullScreen, hInstance);

	if(hWnd == NULL) return TRUE;

	Init(hWnd);											

	//RenderScene();									

	return MainLoop();						
}


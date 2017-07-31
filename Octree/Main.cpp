//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		Octree3	 										 //
//																		 //
//		$Description:	A working octree with a .3ds file format scene   //
//																		 //
//		$Date:			2/16/02											 //
//																		 //
//***********************************************************************//


// This is a compiler directive that includes libraries (For Visual Studio)
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")

#pragma comment(lib, "winmm.lib")


#include "main.h"										
#include "3ds.h"
#include "camera.h"
#include "glfont.h"
#include "collision.h"
#include "ZFLog.h"
#include "Target.h"

#include <vector>
#include "general.h"
#include "bspline.h"
#include "tll.h"
#include "decal.h"
#include "particle.h"
#include "sound.h"
#include "flare.h"

#include "GameMenu.h"

// *Tutorial Note* 
// By reading this tutorial we assume you have looked at the previous octree,
// frustum culling and model loading tutorials.  If not, we strongly recommend it.
//
// This is the third tutorial of the 3 part series on octrees.  The first 2 showed
// how to create an octree, along with incorporating frustum culling to it.  With
// this final part, the octree class will now load in a 3D world from a .3DS file.
// The file format doesn't really matter because all of our loader tutorials
// use the t3DModel structure to store the data, which is passed into the octree
// object to partition.  The octree is completely independent from the model loader.
// 
// In the previous tutorials we stored vertices in the end nodes.  This version takes
// a different approach, since we are dealing with a model structure.  Instead of
// storing vertices, we just store face indices.  There are many benefits to this
// method.  The first is that we can easily plug in this data to vertex arrays, which
// increases our efficiency.  The second is that we don't have to recalculate the
// face information when we break up the data into nodes.  In any model loader, you
// are given all the vertices in the model, then for every polygon to render, indices
// into that vertex array.  This makes it so you don't have to store duplicate vertices
// in memory or the 3D file.  Like the first advantage, it allows us to easily render
// the model through vertex arrays.  The last benefit of this method is that we don't
// have to deal with splitting polygons across planes and then perhaps triangulating
// them.  
//
// For optimal performance, we will be using display lists and vertex arrays for rendering
// each node.  You really wouldn't want it any other way.  Due to the fact that our world
// isn't as big as a full level you would find in a game, the speed increase that the
// octree provides won't be as noticeable, especially on high end graphics cards.  Keep
// this in mind when working with your own octree implementation.  In my experience, the
// biggest problem is loops and being fill limited.  Sometimes, your problem is getting
// geometry to the screen, it's the amount of pixels that are being drawn to the screen.
// If you make your view port size smaller and you get a significant increase, then
// your bottle neck is fill limited.  For instance, On a TNT2 card, I get about 10 to
// 15 frames more on this tutorial when I change the resolution from 800 x 600 to 640 x 480.
// That is why I set the resolution smaller to increase frame rate.  
//
// This application allows you to walk around the world using our camera class.  Intially,
// fog is turned on to give it a cool effect.  You can turn this off to get a better view
// of the world.  
//
// The controls are: 
//
// Mouse               - Rotates the camera's view
// Left Mouse Button   - Changes the Render mode from normal to wire frame.
// Right Mouse Button  - Turns lighting On/Off
// Up    Arrow Key | W - Moves the camera along the view vector
// Down  Arrow Key | S - Moves the camera opposite the view vector
// F				   - Turns fog on/off
// Space Bar		   - Turns on/off the yellow debug lines for the visual node subdivisions
// Escape - Quits


bool  g_bFullScreen = false;			// Set full screen as default
HWND  g_hWnd;						// This is the handle for the window
RECT  g_rRect;						// This holds the window dimensions
HDC   g_hDC;						// General HDC - (handle to device context)
HGLRC g_hRC;						// General OpenGL_DC - Our Rendering Context for OpenGL
							

// Initialize the Camera object
CCamera	g_Camera;

// Here we initialize our single Octree object.  This will hold all of our vertices
COctree g_Octree;

// This will be our global frustum object that keeps track of our frustum
CFrustum g_Frustum;

// We created a debug class to show us the nodes visually in yellow cubes.
CDebug g_Debug;

// The current amount of end nodes in our tree (The nodes with vertices stored in them)
int g_EndNodeCount = 0;

// This stores the amount of nodes that are in the frustum
int g_TotalNodesDrawn = 0;

// The maximum amount of triangles per node.  If a node has equal or less 
// than this, stop subdividing and store the face indices in that node
int g_MaxTriangles = 1000;

// The maximum amount of subdivisions allowed (Levels of subdivision)
int g_MaxSubdivisions = 5;

// Set the current render mode to render triangles initially
bool g_RenderMode = true;

// Turn lighting on initially
bool  g_bLighting     = true;							

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This is the file name that is to be loaded
#define FILE_NAME	"Park.3ds"						

// This is our 3DS loader object that will load the g_World data in
CLoad3DS g_Load3DS;

// This will store our 3ds scene that we will pass into our octree
t3DModel g_World;

// This stores our current frames per second
char g_strFrameRate[255] = {0};

// This holds the texture info, referenced by an ID
UINT g_Texture[MAX_TEXTURES] = {0};	

// This tells us if we want fog on or off - On by default ('F')
bool g_bFog = true;

// This tells us if we want to display the yellow debug lines for our nodes (Space Bar)
bool g_bDisplayNodes = false;				



// Font
	GLFONT myFont;              // the font to use
	ZFLog* g_pLog;

	Target *tar[MAX_TARGET];

	extern float kSpeed;

	bool DEBUG__ = false;
	bool DEBUG2__ = false;

	int stage = 0;
	int step = 8;
	game_mode_type game_mode;
	float animation_end_time = -1;
	float shooting_end_time = -1;

	bool shot_delay = false;
	bool isZoomEnabled = true;
	bool iszooming = false;

	bool isShootingEnabled = true;


	int current_bullets = 6;
	bool isloading = false;
	//int 
	
	struct gun
	{
		float delay;
		float num_bullets;
		float accuracy;
	}shot_gun, auto_gun;

	

	int score = 0;

	float ApplicationStartTime;

	// Splines
	int visible = 0;                // Toggle flag for viewing the splines
	int numSplines = 0;             // Not zero based
	int cameraMode = 2;             // Camera mode (0 = Free, 1 = Look, 2 = Follow)
	int currentSpline = 0;          // Current spline to work with or use for a path
	int lookAtPath = 0;             // Spline to look at when in Follow mode
	char SplineFileName[MAX_PATH];  // filename and path to the spline data
	SPLINE* spline;                 // Global pointer for working with the splines from list
	LinkedList<SPLINE> SplineList;  // Spline list. Note that you can't make this a pointer.


	TEXTURE* texture = new TEXTURE[10];

	DECAL muzzleflashdecal;


	GLuint texture_[1];

	GLfloat lightposition[3];


	// Timing
	float lasttime;
	float multiplier;
	const int average = 10;
	float lastmultiplier[average];
	int fps;

	float angleY = 0.0f;

	float angleZ = 0.0f;	
	float currentRotX = 0.0f;

	bool check = false;

	GameMenu menu;

	bool lbutten;

	int num_targets;
	int num_original_targets;
	int num_hit_targets;

	bool flag = false;

	TargetSetting TS;

	order_type order;

	ParticleSystem* PS;

	
	menu_mode_type menu_mode;

	// Sound
	int device;                             // Selected device
	BOOL lowqual;                           // Low quality option flag
	int SphereSector;                       // Current sector (leaf of bsp tree) of the sphere
	VECTOR SpherePosition;                  // Sphere position
	int numSamples = 0;                     // The number of sound samples in the list
	int numChannels = 0;                    // The number of channels in the list
	LinkedList<SOUND_SAMPLE> SampleList;    // The list of samples
	LinkedList<SOUND_CHANNEL> ChannelList;  // The list of channels


	float gravity = 0.002;

	float SunPosition[3];



///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes the game window.
/////
///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init(HWND hWnd)
{
	g_hWnd = hWnd;										// Assign the window handle to a global window handle
	GetClientRect(g_hWnd, &g_rRect);					// Assign the windows rectangle to a global RECT
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	// Init OpenGL with the global rect

	InitializeBASS();             // Initialize the BASS sound system
    CreateSounds();               // Load the samples and create the channels


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	InitializeLensFlare();

	game_mode = MENU;
	menu_mode = MAIN;
	//game_mode = SHOOTING;
	
	// Initialize the camera position
	if(game_mode == MENU)
		g_Camera.PositionCamera(0, 0, 500,  0, 0, 0,  0, 1, 0);
	else
		g_Camera.PositionCamera(-71, 5, 84,  0, 0, 0,  0, 1, 0);
		

	// Here we load the world from a .3ds file
	g_Load3DS.Import3DS(&g_World, FILE_NAME);

	// Go through all the materials
	for(int i = 0; i < g_World.numOfMaterials; i++)
	{
		// Check to see if there is a file name to load in this material
		if(strlen(g_World.pMaterials[i].strFile) > 0)
		{
			// Use the name of the texture file to load the bitmap, with a texture ID (i).
			// We pass in our global texture array, the name of the texture, and an ID to reference it.	
			CreateTexture(g_Texture, g_World.pMaterials[i].strFile, i);		
			
		}

		// Set the texture ID for this material
		g_World.pMaterials[i].texureId = i;
	}

	// The first thing that needs to happen before creating our octree is to find
	// the total width of the initial root node.  Now we pass in our t3DModel object
	// to GetSceneDimensions(), instead of vertices and a vertex count, as done
	// in the last octree tutorials.  This will store the initial root node cube width.
	g_Octree.GetSceneDimensions(&g_World);

	// Since our model structures stores multiple objects, we can't easily access the
	// total triangle count in the scene with out manually going through and counting the total.
	// This is what we do below.  With the result, we pass this into our CreateNode() function.
	int TotalTriangleCount = g_Octree.GetSceneTriangleCount(&g_World);

	// To create the first node we need the world data, the total triangle count in the scene,
	// along with the initial root node center and width.  This function will then recursively
	// subdivide the rest of the world, according to the global restrictions.
	g_Octree.CreateNode(&g_World, TotalTriangleCount, g_Octree.GetCenter(), g_Octree.GetWidth());

	// The octree should be created by now.  To better increase our efficiency we use display
	// lists for every end node.  This way, we just have to call a display list ID to draw
	// a node, verses the slow loops we normal had.  Vertex arrays are also used to optimize
	// our rendering of the octree.

	// Below we get the display list base ID and store it in the root node.  This should return 1
	// since we don't use display lists anywhere before this.  Notice that we use our global
	// variable that stores our end node count to pass in the total amount of list ID's needed.
	// If you are unfamiliar with displays, basically what you do is section off a certain
	// amount of ID's, and then you are returns a base pointer to the start of those ID's.
	// You can use the ID's from the base pointer to the base pointer ID + the number of
	// ID's that were saved off for that base pointer.  Each of the ID's correspond to a
	// bunch of OpenGL commands.  That means that each end node has it's own ID that
	// corresponds to a bunch of OpenGL commands.  So, for instance, if pass in a bunch
	// of vertices to OpenGL, we can assign this action to a display list.  That way we
	// just call a display list ID to perform that action.  Think of it as a function.
	// You just need to call a function to do a bunch of tasks, which eliminates extra
	// code, and also is saved on the video card for faster processing.  
	g_Octree.SetDisplayListID( glGenLists(g_EndNodeCount) );

	// Now we go through every single end node and create a display list for them all.
	// That way, when we draw the end node, we just use it's display list ID to render
	// the node, instead of looping through all the objects and manually give the verts to opengl.
	// The parameters for this function is the node (starting with the root node), 
	// the world data and current display list base ID.  The base ID is stored in the root
	// node's ID, so we just pass that in.  The reason we do this is because, if you create
	// other display lists before you create the octree, you don't want to assume the octree
	// ID's go from 1 to the end node count.  By passing in the base ID, we then will add
	// this ID to other nodes.  Right now, when they are created they are assigned the
	// end node count at the time upon creating them.  This will make more sense when looking
	// at the octree code.
	g_Octree.CreateDisplayList(&g_Octree, &g_World, g_Octree.GetDisplayListID());

	// Hide our cursor since we are using first person camera mode
	if(game_mode != MENU)
		ShowCursor(FALSE);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	glEnable(GL_LIGHT0);								// Turn on a light with defaults set
	glEnable(GL_LIGHTING);								// Turn on lighting
	glEnable(GL_COLOR_MATERIAL);						// Allow color

	glFontCreate(&myFont, "roman.glf", 2);    // create the font from the glf file


	g_pLog = new ZFLog(ZF_LOG_TARGET_WINDOW, NULL);

	

	SetGLTexture(texture);

	// Decal Initialization
    muzzleflashdecal.Size = 5;            // Set the decal sizes
	muzzleflashdecal.active = false;


	//PlaySound("startsound.wav", NULL, SND_ASYNC);
	

	
	/*glGenTextures(1, texture_);
	
	glBindTexture(GL_TEXTURE_2D, texture_[0]);
	LoadTexture("eogks.bmp");*/
	

	//CreateTexture(texture_, "eogks.bmp", 0);

	glEnable(GL_TEXTURE_2D);


	//glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// light source configuration ##################

	/*GLfloat light_ambient1[] = {0, 0, 0, 1};
	GLfloat light_diffuse1[] = {1, 0, 0, 1};
	GLfloat light_specular1[] = {1, 0.5, 0.5, 1};
	GLfloat light_position1[] = {1, 1, 3, 1};
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular1);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position1);*/

	GLfloat light_ambient2[] = {0.5, 0.5, 0.5, 1};
	//GLfloat light_diffuse2[] = {1, 1, 1, 1};
	//GLfloat light_specular2[] = {0.5, 0.5, 1, 1};
	/*GLfloat light_position2[] = {lightposition[0], lightposition[1], 
		lightposition[2], 1};*/
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient2);
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse2);
	//glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular2);
	//glLightfv(GL_LIGHT1, GL_POSITION, light_position2);

	/*glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);*/

	// #############################################

	// material properties
	GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat mat_diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
	GLfloat mat_specular[] = { 0.4, 0.4, 0.4, 1.0 };
	GLfloat high_shininess[] = { 20.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

	
	CreateTexture(g_Texture, "menu.bmp", 9);
	CreateTexture(g_Texture, "menu1-1.bmp", 10);
	CreateTexture(g_Texture, "menu1-2.bmp", 20);
	//CreateTexture(g_Texture, "menu2-1.bmp", 11);
	//CreateTexture(g_Texture, "menu2-2.bmp", 21);
	CreateTexture(g_Texture, "menu3-1.bmp", 12);
	CreateTexture(g_Texture, "menu3-2.bmp", 22);
	CreateTexture(g_Texture, "menu4-1.bmp", 13);
	CreateTexture(g_Texture, "menu4-2.bmp", 23);
	CreateTexture(g_Texture, "menu5-1.bmp", 14);
	CreateTexture(g_Texture, "menu5-2.bmp", 24);

	CreateTexture(g_Texture, "menu-help-1.bmp", 50);
	CreateTexture(g_Texture, "menu-help-2.bmp", 51);
	CreateTexture(g_Texture, "menu-help-3.bmp", 52);

	CreateTexture(g_Texture, "menu-credit-1.bmp", 60);
	CreateTexture(g_Texture, "menu-credit-2.bmp", 61);
	CreateTexture(g_Texture, "menu-credit-3.bmp", 62);

	TS.step1_position[0] = CVector3(-70.4, 5.3, -62.9);
	TS.step1_vector[0] = CVector3(0.8, 0.3, 0.3);
	TS.step1_position[1] = CVector3(-4.0, 3.7, -40.7);
	TS.step1_vector[1] = CVector3(-0.8, 0.3, 0.2);
	TS.step1_position[2] = CVector3(5.2, 3.6, -16.2);
	TS.step1_vector[2] = CVector3(-0.5, 0.4, -0.6);
	TS.step1_magnitude = 0.4;

	TS.step2_position[0] = CVector3(0.9, 4.2, -72.1);
	TS.step2_vector[0] = CVector3(0.4, 0.3, 0.8);
	TS.step2_position[1] = CVector3(71.6, 4.2, -73.6);
	TS.step2_vector[1] = CVector3(-0.2, 0.3, 0.9);
	TS.step2_position[2] = CVector3(74.6, 3.2, 7.9);
	TS.step2_vector[2] = CVector3(-0.3, 0.3, -0.8);
	TS.step2_position[3] = CVector3(19.8, 4.4, -27.6);
	TS.step2_vector[3] = CVector3(0.9, 0.3, 0.2);
	TS.step2_magnitude = 0.6;

	TS.step3_position[0] = CVector3(-86.8, 6.3, 68.8);
	TS.step3_vector[0] = CVector3(0.8, 0.3, -0.3);
	TS.step3_position[1] = CVector3(63.8, 8.5, 60.4);
	TS.step3_vector[1] = CVector3(-0.8, 0.3, -0.3);
	TS.step3_position[2] = CVector3(-43.2, 3.4, 14.8);
	TS.step3_vector[2] = CVector3(0.9, 0.3, 0.0);
	TS.step3_position[3] = CVector3(-74.8, 4.5, 0.1);
	TS.step3_vector[3] = CVector3(0.8, 0.3, 0.4);
	TS.step3_magnitude = 0.9;

	srand((unsigned)time(NULL));

	if(game_mode == MENU)
		g_Camera.Update();
}


///////////////////////////////// MAIN GAME LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function Handles the main game loop
/////
///////////////////////////////// MAIN GAME LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

WPARAM MainLoop()
{
	MSG msg;

	while(1)											// Do our infinite loop
	{													// Check if there was a message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
        { 
			if(msg.message == WM_QUIT)					// If the message wasnt to quit
				break;
            TranslateMessage(&msg);						// Find out what the message does
            DispatchMessage(&msg);						// Execute the message
        }
		else											// if there wasn't a message
		{
			if(game_mode == MENU)
			{
				//g_Camera.Update();							// Update the camera info
				menu.show();
			}
			else
			{
				g_Camera.Update();							// Update the camera info
				ProcessStage();
				ProcessStep();
				RenderScene();								// Render the scene
			}
        } 
	}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	
	// Go through all the objects in the scene and free them
	for(int i = 0; i < g_World.numOfObjects; i++)
	{
		// Free the faces, normals, vertices, and texture coordinates.
		delete [] g_World.pObject[i].pFaces;
		delete [] g_World.pObject[i].pNormals;
		delete [] g_World.pObject[i].pVerts;
		delete [] g_World.pObject[i].pTexVerts;
		delete [] g_World.pObject[i].pIndices;
	}

	// When using display lists, we need to free them when we are finished using
	// the ID's.  This OpenGL function does just that.  We pass in the base ID and
	// the total ID's we used.
	glDeleteLists(g_Octree.GetDisplayListID(), g_EndNodeCount);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	DeInit();											// Clean up

	return(msg.wParam);									// Return from the program
}

static float i = 0;

///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the entire scene.
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();									// Reset The matrix

	//multiplier = GetTimePassed(lasttime, average, lastmultiplier);
    //g_Camera.Multiplier = multiplier;




    //g_Camera.Update2();

 

   // g_Camera.Apply();

	


	//g_Camera.QuatRotate();

	// Position the camera
	g_Camera.Look();


	UpdateListener();                              // Update the listener's position, etc

    //VECTOR LastSpherePosition = SpherePosition;
    //VECTOR SphereVelocity;
    //spline = SplineList.Get(1);                    // Get the spline from the linked list
    //SpherePosition = bsplinepoint(spline);         // Set the sphere (sound source) position
    //SphereSector = FindCurrentLeaf(SpherePosition, root); // Find the spheres sector for rendering
    //SphereVelocity.x = SpherePosition.x - LastSpherePosition.x; // Find the velocity
    //SphereVelocity.y = SpherePosition.y - LastSpherePosition.y;
    //SphereVelocity.z = SpherePosition.z - LastSpherePosition.z;

    //UpdateChannel(channel_engine, &SpherePosition, NULL, &SphereVelocity);  // Update the engine sound
 //   UpdateChannel(channel_carhorn, &VECTOR(g_Camera.Position().x, g_Camera.Position().y, g_Camera.Position().z), NULL, NULL);  // Update the car horn sound
	//UpdateChannel(channel_loading, &VECTOR(g_Camera.Position().x, g_Camera.Position().y, g_Camera.Position().z), NULL, NULL);  // Update the car horn sound
	//UpdateChannel(channel_click, &VECTOR(g_Camera.Position().x, g_Camera.Position().y, g_Camera.Position().z), NULL, NULL);  // Update the car horn sound
	//UpdateChannel(channel_hover, &VECTOR(g_Camera.Position().x, g_Camera.Position().y, g_Camera.Position().z), NULL, NULL);  // Update the car horn sound


	

	// Each frame we calculate the new frustum.  In reality you only need to
	// calculate the frustum when we move the camera.
	g_Frustum.CalculateFrustum();

	// Initialize the total node count that is being draw per frame
	g_TotalNodesDrawn = 0;


	/*float mat_ambient[] = { 0.8, 0.5, 0.1, 1.0 };
    float mat_diffuse[] = { 0.8, 0.5, 0.1, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

	
	i += 0.05;

    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(-3.0f,10.0f,0.0f+i);
    GLUquadricObj * sphere = gluNewQuadric();
    gluQuadricOrientation(sphere, GLU_OUTSIDE);
    gluSphere(sphere,2.0,50,50);
    glPopMatrix();
    glEnable(GL_TEXTURE_2D);*/




	//int middleX = SCREEN_WIDTH  >> 4;		
	//int middleY = SCREEN_HEIGHT >> 4;

	////glTranslatef(g_Camera.Position().x, g_Camera.Position().y, g_Camera.Position().z);

	//// Turn on texture mapping and turn off color
	//glEnable(GL_TEXTURE_2D);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); // DECAL모드

	//glBindTexture(GL_TEXTURE_2D, g_Texture[10]);
	//glBegin(GL_QUADS);
 //       glTexCoord2f(0.0f, 0.0f);
 //       glVertex3f(-middleX, -middleY, 0.0);
 //       glTexCoord2f(1.0f, 0.0f);
 //       glVertex3f(middleX, -middleY, 0.0);
 //       glTexCoord2f(1.0f, 1.0f);
 //       glVertex3f(middleX, middleY, 0.0);
 //       glTexCoord2f(0.0f, 1.0f);
 //       glVertex3f(-middleX, middleY, 0.0);
 //   glEnd();



	/*CVector3 vVector = g_Camera.View()-g_Camera.Position();
	vVector = Normalize(vVector);

	VECTOR rO(g_Camera.Position().x, g_Camera.Position().y, g_Camera.Position().z);
	VECTOR rV(vVector.x, vVector.y, vVector.z);
	VECTOR sO(-3.0f, 10.0f, 0.0f+i);

	if(tar[0] != NULL && tar[0]->isTargetActive() == true)
		g_pLog->Log(0, "0");
	if(tar[1] != NULL && tar[1]->isTargetActive() == true)
		g_pLog->Log(0, "1");
	if(tar[2] != NULL && tar[2]->isTargetActive() == true)
		g_pLog->Log(0, "2");*/

	if(game_mode == SHOOTING)
	{
		for(int j=0; j<MAX_TARGET; j++)
		{
			if(tar[j] != NULL)
			{
				if(order == SEQ)
				{
					if(j > 0 && tar[j]->isTargetActive() == false)
					{
						bool isnext = true;
						for(int k=0; k<j; k++)
						{
							if(tar[k] != NULL)
								if(tar[k]->isTargetActive() == true)
									isnext = false;
						}

						if(isnext == true)
								tar[j]->EnableTarget();
					}
				}

				float temp = (float)GetTickCount();
				if(tar[j]->isTargetActive() == true && temp >= tar[j]->GetDelay())
				{
					tar[j]->MoveTarget();
					tar[j]->DrawTarget();
					if(tar[j]->Position().y < -10)
					{
						tar[j]->DisableTarget();
						tar[j]->DestroyTarget();
						num_targets--;
						delete tar[j];
						tar[j] = NULL;
					}
				}
			}
		}
	}

	if(game_mode == ANIMATION)
		DrawSplines(SplineList);           // Required to calculate the camera path

	DrawSkybox(g_Camera, texture);
	DrawLensFlare();


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Here we draw the octree, starting with the root node and recursing down each node.
	// This time, we pass in the root node and just the original world model.  You could
	// just store the world in the root node and not have to keep the original data around.
	// This is up to you.  I like this way better because it's easy, though it could be 
	// more error prone.
	g_Octree.DrawOctree(&g_Octree, &g_World);

	// Render the cubed nodes to visualize the octree (in wire frame mode)
	if( g_bDisplayNodes )
		g_Debug.RenderDebugLines();

	// Create a buffer to store the octree information for the title bar
	static char strBuffer[255] = {0};

	// Display in window mode the current subdivision information.  We now display the
	// max triangles per node, the max level of subdivision, total end nodes, current nodes drawn
	// and frames per second we are receiving.  
	sprintf(strBuffer, "초당 폴리곤수: %d   node나눈 수: %d   마지막노드: %d   읽히는노드수: %d   초당프레임수: %s",
		                g_MaxTriangles,		 g_MaxSubdivisions,		 g_EndNodeCount,	   g_TotalNodesDrawn,	  g_strFrameRate);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	
	static int ii = 0;

	if(muzzleflashdecal.active)
    {
		GLfloat light_position2[] = {g_Camera.Position().x, g_Camera.Position().y, g_Camera.Position().z, 1};
		glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
		CVector3 vVector = g_Camera.View()-g_Camera.Position();
		vVector = Normalize(vVector);
		GLfloat light_direction[] = {vVector.x, vVector.y, vVector.z, 1};
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light_direction);
		GLfloat exponent = 50;
		glLightfv(GL_LIGHT1, GL_SPOT_EXPONENT, &exponent);
		GLfloat cutoff = 20;
		glLightfv(GL_LIGHT1, GL_SPOT_CUTOFF, &cutoff);
		//GLfloat att = 1;
		//glLightfv(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, &att);
		



  
		//glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT1);

		ii++;
		if(ii==2)
		{
			muzzleflashdecal.active = false;
			glDisable(GL_LIGHT1);
			ii = 0;
		}
        
		

        muzzleflashdecal.Position = VECTOR(g_Camera.Position().x, g_Camera.Position().y, g_Camera.Position().z);
		//muzzleflashdecal.Position = g_Camera.PositionV;
        muzzleflashdecal.Orientation = g_Camera.Orientation;
        DrawMuzzleFlash();
    }
	
	if(PS != NULL)
	{
		PS->Update();
		PS->Render(3);
	}

	// Set our window title bar to the subdivision information
	SetWindowText(g_hWnd, strBuffer);

	//DrawUI();

	if(isZoomEnabled == true)
		Zooming();

	


	//vVector = g_Camera.View()-g_Camera.Position();
	//vVector = Normalize(vVector);

	//g_pLog->Log(2000, "position vector : %f, %f, %f", g_Camera.Position().x, g_Camera.Position().y, g_Camera.Position().z);
	//g_pLog->Log(0, "view vector : %f, %f, %f", vVector.x, vVector.y, vVector.z);

	// Swap the backbuffers to the foreground
	SwapBuffers(g_hDC);									
}

void DrawUI()
{
	// Draw text
	glEnable(GL_TEXTURE_2D);

 	glBlendFunc(GL_SRC_ALPHA,  GL_ONE_MINUS_SRC_ALPHA) ;
    glDisable(GL_LIGHTING);
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glPushMatrix();
    glLoadIdentity();
    glFontBegin(&myFont);
    char text[256];

	sprintf(text, "SCORE  %d", score);
	glFontTextOut(text, -5, 20, -60);

	if(current_bullets != 0 && isloading == false)
	{
		sprintf(text, "bullet  %d", current_bullets);
		glFontTextOut(text, 25, -25, -70);
	}
	else
	{
		sprintf(text, "loading the bullets....");
		glFontTextOut(text, 20, -25, -70);
	}
	

	//g_pLog->Log(0, "view = %f, %f, %f", g_Camera.View().x, g_Camera.View().y, g_Camera.View().z);

	/*sprintf(text, "position = %f, %f, %f", g_Camera.Position().x, g_Camera.Position().y, g_Camera.Position().z);
    glFontTextOut(text, -52, 34, -100);

    sprintf(text, "view = %f, %f, %f", g_Camera.View().x, g_Camera.View().y, g_Camera.View().z);
    glFontTextOut(text, -52, -30, -100);

    sprintf(text, "up = %f, %f, %f", g_Camera.UpVector().x, g_Camera.UpVector().y, g_Camera.UpVector().z);
    glFontTextOut(text, -52, -32, -100);*/

    glFontEnd();
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);


	// draw bullets


	// draw targetpoint

	glPushMatrix();
    glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
    glDepthMask(0);
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0);
    glDisable(GL_LIGHTING);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, texture[2].TexID);

	glTranslatef(0.0,0.0,-30.0);

	
    glDisable(GL_CULL_FACE);
	//glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-TARGET_SIZE, -TARGET_SIZE, 0.0);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(TARGET_SIZE, -TARGET_SIZE, 0.0);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(TARGET_SIZE, TARGET_SIZE, 0.0);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-TARGET_SIZE, TARGET_SIZE, 0.0);
    glEnd();


	glBindTexture(GL_TEXTURE_2D, texture[3].TexID);

	glTranslatef(25.0,-20.0,-30.0);

	for(int i=0; i<current_bullets; i++)
	{
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-BULLET_WIDTH, -BULLET_HEIGHT+i*2, 0.0);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(BULLET_WIDTH, -BULLET_HEIGHT+i*2, 0.0);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(BULLET_WIDTH, BULLET_HEIGHT+i*2, 0.0);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-BULLET_WIDTH, BULLET_HEIGHT+i*2, 0.0);
		glEnd();
	}



    glEnable(GL_CULL_FACE);
    glDepthMask(1);
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
    glPopMatrix();


}

void ProcessStage()
{
	
}

void ProcessStep()
{
	//g_pLog->Log(0, "step : %d, game_mode : %d", step, game_mode);
	if(step == 0 && game_mode == ANIMATION && flag == false)
	{
		
		sprintf(SplineFileName, "%s", "step1.spn");
		
		ApplicationStartTime = (float)GetTickCount();
		animation_end_time = (float)GetTickCount() + SetSplines(SplineList);

		float CurrentTime = (float)GetTickCount();
		flag = true;
	}

	if(step == 0 && game_mode == ANIMATION && flag == true)
	{
		if((float)GetTickCount() > animation_end_time)
		{
			int i;

			flag = false;
			game_mode = SHOOTING;
			animation_end_time = -1;
			//isShootingEnabled = true;
			num_targets = 3;
			num_original_targets = 3;
			num_hit_targets = 0;

			order = SEQ;

			float mag = TS.step1_magnitude;

			CVector3 temp;

			i = rand()%MAX_STEP1_POSITION;
			temp = TS.step1_vector[i];
			temp.y += STEP1_TARGET_VECTOR_Y_INCREMENT;
			tar[0] = new Target(TS.step1_position[i], DivideVectorByScaler(temp, 1/mag), STEP1_TARGET_RADIUS);
			tar[0]->EnableTarget();

			i = rand()%MAX_STEP1_POSITION;
			temp = TS.step1_vector[i];
			temp.y += STEP1_TARGET_VECTOR_Y_INCREMENT;
			tar[1] = new Target(TS.step1_position[i], DivideVectorByScaler(temp, 1/mag), STEP1_TARGET_RADIUS);
			//tar[1]->EnableTarget();

			i = rand()%MAX_STEP1_POSITION;
			temp = TS.step1_vector[i];
			temp.y += STEP1_TARGET_VECTOR_Y_INCREMENT;
			tar[2] = new Target(TS.step1_position[i], DivideVectorByScaler(temp, 1/mag), STEP1_TARGET_RADIUS);
			//tar[2]->EnableTarget();
			
			shooting_end_time = -1;
		}
	}

	if(step == 0 && game_mode == SHOOTING)
	{
		if(tar[2] != NULL)
			if(tar[2]->isTargetActive() == true && shooting_end_time == -1)
				shooting_end_time = 6000 + (float)GetTickCount();

		if(num_targets == 0 || (shooting_end_time <= (float)GetTickCount() && shooting_end_time != -1))
		{
			for(int j=0; j<MAX_TARGET; j++)
			{
				if(tar[j] != NULL)
					if(tar[j]->isTargetActive())
					{
						tar[j]->DisableTarget();
						tar[j]->DestroyTarget();

						delete tar[j];
						tar[j] = NULL;
					}
			}

			game_mode = ANIMATION;

			//isShootingEnabled = false;
			flag = false;
			step = 1;
		}
	}

	if(step == 1 && game_mode == ANIMATION && flag == false)
	{
		sprintf(SplineFileName, "%s", "step2.spn");

		ApplicationStartTime = (float)GetTickCount();
		animation_end_time = (float)GetTickCount() + SetSplines(SplineList);

		float CurrentTime = (float)GetTickCount();
		flag = true;
	}

	if(step == 1 && game_mode == ANIMATION && flag == true)
	{
		if((float)GetTickCount() > animation_end_time)
		{
			int i;

			flag = false;
			game_mode = SHOOTING;
			animation_end_time = -1;
			//isShootingEnabled = true;
			num_targets = 5;
			num_original_targets = 5;
			num_hit_targets = 0;

			order = SEQ;

			float mag = TS.step2_magnitude;

			CVector3 temp;

			i = rand()%MAX_STEP2_POSITION;
			temp = TS.step2_vector[i];
			temp.y += STEP2_TARGET_VECTOR_Y_INCREMENT;
			tar[0] = new Target(TS.step2_position[i], DivideVectorByScaler(temp, 1/mag), STEP2_TARGET_RADIUS);
			tar[0]->EnableTarget();

			i = rand()%MAX_STEP2_POSITION;
			temp = TS.step2_vector[i];
			temp.y += STEP2_TARGET_VECTOR_Y_INCREMENT;
			tar[1] = new Target(TS.step2_position[i], DivideVectorByScaler(temp, 1/mag), STEP2_TARGET_RADIUS);
			//tar[1]->EnableTarget();

			i = rand()%MAX_STEP2_POSITION;
			temp = TS.step2_vector[i];
			temp.y += STEP2_TARGET_VECTOR_Y_INCREMENT;
			tar[2] = new Target(TS.step2_position[i], DivideVectorByScaler(temp, 1/mag), STEP2_TARGET_RADIUS);
			//tar[2]->EnableTarget();

			i = rand()%MAX_STEP2_POSITION;
			temp = TS.step2_vector[i];
			temp.y += STEP2_TARGET_VECTOR_Y_INCREMENT;
			tar[3] = new Target(TS.step2_position[i], DivideVectorByScaler(temp, 1/mag), STEP2_TARGET_RADIUS);
			//tar[3]->EnableTarget();

			i = rand()%MAX_STEP2_POSITION;
			temp = TS.step2_vector[i];
			temp.y += STEP2_TARGET_VECTOR_Y_INCREMENT;
			tar[4] = new Target(TS.step2_position[i], DivideVectorByScaler(temp, 1/mag), STEP2_TARGET_RADIUS);
			//tar[4]->EnableTarget();

			shooting_end_time = -1;
		}
	}

	if(step == 1 && game_mode == SHOOTING)
	{
		if(tar[4] != NULL)
			if(tar[4]->isTargetActive() == true && shooting_end_time == -1)
				shooting_end_time = 7000 + (float)GetTickCount();

		if(num_targets == 0 || (shooting_end_time <= (float)GetTickCount() && shooting_end_time != -1))
		{
			for(int j=0; j<MAX_TARGET; j++)
			{
				if(tar[j] != NULL)
					if(tar[j]->isTargetActive())
					{
						tar[j]->DisableTarget();
						tar[j]->DestroyTarget();

						delete tar[j];
						tar[j] = NULL;
					}
			}
			game_mode = ANIMATION;

			flag = false;
			step = 2;
		}
	}

	if(step == 2 && game_mode == ANIMATION && flag == false)
	{
		sprintf(SplineFileName, "%s", "step3.spn");

		ApplicationStartTime = (float)GetTickCount();
		animation_end_time = (float)GetTickCount() + SetSplines(SplineList);

		float CurrentTime = (float)GetTickCount();
		flag = true;
	}

	if(step == 2 && game_mode == ANIMATION && flag == true)
	{
		if((float)GetTickCount() > animation_end_time)
		{
			int i;

			flag = false;
			game_mode = SHOOTING;
			animation_end_time = -1;
			//isShootingEnabled = true;
			num_targets = 7;
			num_original_targets = 7;
			num_hit_targets = 0;

			order = RAN;

			float mag = TS.step3_magnitude;

			CVector3 temp;

			i = rand()%MAX_STEP3_POSITION;
			temp = TS.step3_vector[i];
			temp.y += STEP3_TARGET_VECTOR_Y_INCREMENT;
			tar[0] = new Target(TS.step3_position[i], DivideVectorByScaler(temp, 1/mag), STEP3_TARGET_RADIUS, STEP3_TARGET_DELAY+500);
			tar[0]->EnableTarget();

			i = rand()%MAX_STEP3_POSITION;
			temp = TS.step3_vector[i];
			temp.y += STEP3_TARGET_VECTOR_Y_INCREMENT;
			tar[1] = new Target(TS.step3_position[i], DivideVectorByScaler(temp, 1/mag), STEP3_TARGET_RADIUS, STEP3_TARGET_DELAY+500);
			tar[1]->EnableTarget();

			i = rand()%MAX_STEP3_POSITION;
			temp = TS.step3_vector[i];
			temp.y += STEP3_TARGET_VECTOR_Y_INCREMENT;
			tar[2] = new Target(TS.step3_position[i], DivideVectorByScaler(temp, 1/mag), STEP3_TARGET_RADIUS, STEP3_TARGET_DELAY+1000);
			tar[2]->EnableTarget();

			i = rand()%MAX_STEP3_POSITION;
			temp = TS.step3_vector[i];
			temp.y += STEP3_TARGET_VECTOR_Y_INCREMENT;
			tar[3] = new Target(TS.step3_position[i], DivideVectorByScaler(temp, 1/mag), STEP3_TARGET_RADIUS, STEP3_TARGET_DELAY+1500);
			tar[3]->EnableTarget();

			i = rand()%MAX_STEP3_POSITION;
			temp = TS.step3_vector[i];
			temp.y += STEP3_TARGET_VECTOR_Y_INCREMENT;
			tar[4] = new Target(TS.step3_position[i], DivideVectorByScaler(temp, 1/mag), STEP3_TARGET_RADIUS, STEP3_TARGET_DELAY+2000);
			tar[4]->EnableTarget();

			i = rand()%MAX_STEP3_POSITION;
			temp = TS.step3_vector[i];
			temp.y += STEP3_TARGET_VECTOR_Y_INCREMENT;
			tar[5] = new Target(TS.step3_position[i], DivideVectorByScaler(temp, 1/mag), STEP3_TARGET_RADIUS, STEP3_TARGET_DELAY+2000);
			tar[5]->EnableTarget();

			i = rand()%MAX_STEP3_POSITION;
			temp = TS.step3_vector[i];
			temp.y += STEP3_TARGET_VECTOR_Y_INCREMENT;
			tar[6] = new Target(TS.step3_position[i], DivideVectorByScaler(temp, 1/mag), STEP3_TARGET_RADIUS, STEP3_TARGET_DELAY+2500);
			tar[6]->EnableTarget();

			float maxdelay = tar[0]->GetDelay();
			for(int j=0; j<7; j++)
			{
				if(tar[j]->GetDelay() > maxdelay)
					maxdelay = tar[j]->GetDelay();
			}

			shooting_end_time = maxdelay + 10000 + (float)GetTickCount();

		}
	}

	if(step == 2 && game_mode == SHOOTING)
	{
		if(num_targets == 0 || shooting_end_time <= (float)GetTickCount())
		{
			for(int j=0; j<MAX_TARGET; j++)
			{
				if(tar[j] != NULL)
					if(tar[j]->isTargetActive())
					{
						tar[j]->DisableTarget();
						tar[j]->DestroyTarget();

						delete tar[j];
						tar[j] = NULL;
					}
			}
			game_mode = ANIMATION;

			flag = false;
			step = 1;
		}
	}

	if(step == 3 && game_mode == ANIMATION && flag == false)
	{
		sprintf(SplineFileName, "%s", "step4.spn");

		ApplicationStartTime = (float)GetTickCount();
		animation_end_time = (float)GetTickCount() + SetSplines(SplineList);

		float CurrentTime = (float)GetTickCount();
		flag = true;
	}

	if(step == 3 && game_mode == ANIMATION && flag == true)
	{
		if((float)GetTickCount() > animation_end_time)
		{
			//flag = false;
			//game_mode = SHOOTING;
			//animation_end_time = -1;
			//isShootingEnabled = true;
			////num_targets = 2;
			
			game_mode = ANIMATION;

			flag = false;
			step = 4;
		}
	}

	if(step == 3 && game_mode == SHOOTING)
	{
		if(num_targets == 0)
		{
			game_mode = ANIMATION;

			flag = false;
			step = 1;
		}
	}

	if(step == 4 && game_mode == ANIMATION && flag == false)
	{
		sprintf(SplineFileName, "%s", "step5.spn");

		ApplicationStartTime = (float)GetTickCount();
		animation_end_time = (float)GetTickCount() + SetSplines(SplineList);

		float CurrentTime = (float)GetTickCount();
		flag = true;
	}

	if(step == 3 && game_mode == ANIMATION && flag == true)
	{
		if((float)GetTickCount() > animation_end_time)
		{
			//flag = false;
			//game_mode = SHOOTING;
			//animation_end_time = -1;
			//isShootingEnabled = true;
			////num_targets = 2;
			
			//game_mode = ANIMATION;

			//flag = false;
			//step = 4;
		}
	}
}

void Zooming()
{
	static float degree = MAX_FOVY_DEGREE;

	if(iszooming == true)
	{
		degree -= DEGREE_PER_FRAME;
		if(degree < MIN_FOVY_DEGREE)
			degree = MIN_FOVY_DEGREE;
	}
	else
	{
		degree += DEGREE_PER_FRAME;
		if(degree > MAX_FOVY_DEGREE)
			degree = MAX_FOVY_DEGREE;
	}

	glViewport(0,0,g_rRect.right,g_rRect.bottom);				

	glMatrixMode(GL_PROJECTION);					
	glLoadIdentity();		

	gluPerspective(degree,(float)g_rRect.right/(float)g_rRect.bottom, 1, 500.0f);

	glMatrixMode(GL_MODELVIEW);						
	glLoadIdentity();
}





///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the window messages.
/////
///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG    lRet = 0; 
    PAINTSTRUCT    ps;


	CVector3 vVector = g_Camera.View()-g_Camera.Position();
	vVector = Normalize(vVector);

	VECTOR rO;
	VECTOR rV;
	VECTOR sO;

    switch (uMsg)
	{ 
    case WM_SIZE:										// If the window is resized
		if(!g_bFullScreen)								// Do this only if we are NOT in full screen
		{
			SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));// LoWord=Width, HiWord=Height
			GetClientRect(hWnd, &g_rRect);				// Get the window rectangle
			if(g_hDC) RenderScene();					// Redraw the scene if we have a valid hdc
		}
        break; 
 
	case WM_PAINT:										// If we need to repaint the scene
		BeginPaint(hWnd, &ps);							// Init the paint struct		
		if(g_hDC) RenderScene();						// Redraw the scene
		EndPaint(hWnd, &ps);							// EndPaint, Clean up
		break;

	case WM_MOUSEMOVE:
		if (cameraMode == 2)
        {
			//g_Camera.Delta_x = float(HIWORD(lParam) - g_rRect.bottom * 0.5) * 10;
			//g_Camera.Delta_y = float(LOWORD(lParam) - g_rRect.right * 0.5) * 10;
        }
		break;

	case WM_LBUTTONDOWN:								// If we hit the left mouse button
		
		//g_RenderMode = !g_RenderMode;					// Change the rendering mode

		// Change the rendering mode to lines or triangles, depending on it's current status
		//if(g_RenderMode) {				
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// Render the triangles in fill mode		
		//}
		//else {
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// Render the triangles in wire frame mode
		//}

		if(game_mode == MENU)
		{
			lbutten = true;
		}
		else
		{
			if(current_bullets == 0 || isloading == true)
				break;

			if(shot_delay == true)
				break;
			else
			{
				shot_delay = true;
				SetTimer(g_hWnd, 1, 700, NULL);
			}

			if(!isShootingEnabled)
				break;

			current_bullets--;
			if(current_bullets == 0)
			{
				SetTimer(g_hWnd, 2, LOADING_TIME, NULL);
				//PlaySound("loading.wav", NULL, SND_ASYNC);
				PlayChannel(channel_loading, false);
				isloading = true;
			}

			muzzleflashdecal.active = true;

			g_Camera.SetView(g_Camera.View().x, g_Camera.View().y+1, g_Camera.View().z);
			//PlaySound("gunshot.wav", NULL, SND_ASYNC);
			PlayChannel(channel_gunshot, false);

			rO.Set(g_Camera.Position().x, g_Camera.Position().y, g_Camera.Position().z);
			rV.Set(vVector.x, vVector.y, vVector.z);

			//if(DEBUG__ == true)
			{
				for(int j=0; j<MAX_TARGET; j++)
				{
					if(tar[j] != NULL)
					{
						if(tar[j]->isTargetActive() == true)
						{
							sO.Set(tar[j]->Position().x, tar[j]->Position().y, tar[j]->Position().z);
							if(IntersectRaySphere(rO, rV, sO, tar[j]->Radius()) >0 )
							{
								//PlaySound("Objectsound.wav", NULL, SND_ASYNC);
								PlayChannel(channel_explode, false);
								tar[j]->DisableTarget();
								tar[j]->DestroyTarget();
								score += 500;
								num_targets--;
								num_hit_targets++;

								if(num_hit_targets == num_original_targets)
									PlayChannel(channel_applause, false);

								PS = CreateSparks(VECTOR(tar[j]->Position().x, tar[j]->Position().y+10, tar[j]->Position().z), 
									VECTOR(tar[j]->Position().x, tar[j]->Position().y, tar[j]->Position().z), 
									VECTOR(g_Camera.Position().x-tar[j]->Position().x, 
									g_Camera.Position().y-tar[j]->Position().y,
									g_Camera.Position().z-tar[j]->Position().z), tar[j]->Radius());

								PS->SetupParticles();

								delete tar[j];

								tar[j] = NULL;

								//g_pLog->Log(0, "%d", tar[j]);

								/*if(j==1)
								{
								
									PlaySound("applause.wav", NULL, SND_ASYNC);
								
								}*/
							}
						}
					}
				}
			}
		}
		

		
		

		break;

	case WM_LBUTTONUP:
		lbutten = false;
		break;

	case WM_TIMER:
		if(wParam == 1)
		{
			shot_delay = false;
			KillTimer(g_hWnd, 1);
		}

		if(wParam == 2)
		{
			current_bullets = MAX_BULLETS;
			isloading = false;
			KillTimer(g_hWnd, 2);
		}
		break;

	case WM_RBUTTONDOWN:								// If the right mouse button was clicked.
		
		//g_bLighting = !g_bLighting;						// Turn lighting ON/OFF

		//if(g_bLighting) {								// If lighting is ON
		//	glEnable(GL_LIGHTING);						// Enable OpenGL lighting
		//} else {
		//	glDisable(GL_LIGHTING);						// Disable OpenGL lighting
		//}
		if(game_mode != MENU)
		{
			iszooming = true;
		}
		
		break;
	case WM_RBUTTONUP:
		if(game_mode != MENU)
		{
			iszooming = false;
		}
		break;

	case WM_KEYDOWN:									// If we hit a key
		switch (wParam)									// Check which key we hit
		{
			case VK_ESCAPE:								// If we hit ESCAPE
				PostQuitMessage(0);						// Tell windows we want to quit
				break;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

			case VK_SPACE:								// If we hit the space bar

				g_bDisplayNodes = !g_bDisplayNodes;		// Turn the debug lines on/off
				break;

			case 'F':									// Check if hit the 'F' key
				
				g_bFog = !g_bFog;						// Turn fog on/off

				if( g_bFog )							// If fog is turned on
				{
					// Set the background color to grey and enable fog
					glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
					glEnable(GL_FOG);
				}
				else
				{
					// Turn the color back to blue'ish purple and disable fog
					glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
					glDisable(GL_FOG);
				}
				break;
			case 'R':
				if(isloading == false)
				{
					SetTimer(g_hWnd, 2, LOADING_TIME, NULL);
					//PlaySound("loading.wav", NULL, SND_ASYNC);
					PlayChannel(channel_loading, false);
					isloading = true;
				}
				break;
			case VK_F1:
				kSpeed = 5.0f;
				break;
			case VK_F2:
				kSpeed = 10.0f;
				break;
			case VK_F3:
				kSpeed = 50.0f;
				break;
			case VK_F4:
				DEBUG__ = true;
				break;
			case VK_F5:
				if(DEBUG2__ == true)
					DEBUG2__ = false;
				else
				{
					sprintf(SplineFileName, "%s", "step1.spn");
					DEBUG2__ = true;
					ApplicationStartTime = (float)GetTickCount();
					SetSplines(SplineList);
				}
				break;
			case VK_F6:
				if(DEBUG2__ == true)
					DEBUG2__ = false;
				else
				{
					DEBUG2__ = true;

					ApplicationStartTime = (float)GetTickCount();
					SetSplines(SplineList);
				}
				break;
			case VK_F9:
				g_pLog->Log(0, "position vector : %f, %f, %f", g_Camera.Position().x, g_Camera.Position().y, g_Camera.Position().z);
				g_pLog->Log(0, "view vector : %f, %f, %f", vVector.x, vVector.y, vVector.z);
				break;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


		}
		break;
	case WM_CLOSE:
		for (int i = numSplines - 1; i >= 0; i--)
            DeleteSpline(i, SplineList);

    case WM_DESTROY:									// If the window is destroyed
        PostQuitMessage(0);								// Tell windows we want to quit
        break; 
     
    default:											// Return by default
        lRet = DefWindowProc (hWnd, uMsg, wParam, lParam); 
        break; 
    } 
 
    return lRet;										// Return by default
}

/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// If this tutorial was too much of a jump from the previous tutorials, I apologize.
// I tried to think of people who were advanced and wanted all the optimizations, and
// at the same time try not to lose anyone.  Fog was added just for a cool effect.
// This can actually be used to help speed up the octree.  If you make the fog really
// dense, you can then bring in the near plane for our frustum.  This will make it so
// you can't see so far, which means you won't have to display as many end nodes.
// 
// Let's go over the functions that need to be called from the client to make the
// octree work:
// 
//  First we need to get the cube's bounding box of the world:
//
//  // Gets the scene dimensions for our entire model
// 	g_Octree.GetSceneDimensions(&g_World);
//
//  Then we need to find our how many polygons make up the world:
//
//  // Returns the whole scene's polygon count
//	g_Octree.GetSceneTriangleCount(&g_World);
//
//  Once we find the polygon count, we can pass that into:
//
//  // This starts with the root node and recursively creates the octree
//	g_Octree.CreateNode(&g_World, TotalTriangleCount, g_Octree.GetCenter(), g_Octree.GetWidth());
//
//  Since we are using display lists, we want to find the display list base ID:
//
//  // This creates and sets the base list ID for all the end nodes
//	g_Octree.SetDisplayListID( glGenLists(g_EndNodeCount) );
//
//  To finally create the display list, we take the base list ID and pass it into:
//
//  //  This recursively creates a display list for every end node
//	g_Octree.CreateDisplayList(&g_Octree, &g_World, g_Octree.GetDisplayListID());
//
//  Once the octree is finished, we can finally call our DrawOctree() function in RenderScene():
//
//  // Draw the octree with the root world being passed in, along with the root node.
//  g_Octree.DrawOctree(&g_Octree, &g_World);
// 
// I hope this tutorial helps give you ideas for your own octree implementation.  There is
// no one way to do it.  This is just what I came up with.  I will strongly suggest that you
// stick with using vertex arrays if you haven't before.  The impact is huge without them, 
// especially when you get up to rendering thousands of polygons.
//
// I would like to thank Elmar Moelzer <moelzere@aon.at> for the Park.3ds art.  You can visit
// his company's page for more cool art and game oriented stuff.  He is pretty amazing!
// It is located at www.mediastudio-graz.com.
// 
// Good luck!
// 
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// www.GameTutorials.com
//
// ?2001 GameTutorials

#include "general.h"
#include "main.h"
#include "ZFLog.h"
#include <windows.h>
#include "general.h"
#include "Camera.h"
#include "texture.h"
#include "vector.h"


extern int numSplines;
extern int lookAtPath;
extern int currentSpline;
extern SPLINE* spline;
extern char SplineFileName[MAX_PATH];

extern ZFLog* g_pLog;
extern float multiplier;
extern int fps;

void AddSpline(int Number, LinkedList<SPLINE>& SplineList)
{
    SPLINE* spline2 = new SPLINE;
    spline2->Active = 1;
    spline2->Repeat = 1;
    spline2->Degree = 3;
    spline2->NumControl = 7;
    spline2->NumPoints = 100;
    spline2->Control = new VECTOR[100];
    spline2->Output = new VECTOR[1000];
    spline2->StartTime = 5000;
    spline2->EndTime = 25000;
    spline2->CopyOfStartTime = spline2->StartTime;
    spline2->CopyOfEndTime = spline2->EndTime;

    spline2->Red = ((float)(rand()%226) + 30.0) / 255;
    spline2->Green = ((float)(rand()%226) + 30.0) / 255;
    spline2->Blue = ((float)(rand()%266) + 30.0) / 255;

    for (int loop = 0; loop < 100; loop++)
    {
        spline2->Control[loop].x = rand()%60 - 29;  spline2->Control[loop].y = rand()%60 - 29;  spline2->Control[loop].z = rand()%60 - 29;
    }

    spline2->linkPosition = Number;   //Set the link position of the spline
    SplineList.Insert(spline2);       //Insert spline in linked list
}

void DeleteSpline(int Number, LinkedList<SPLINE>& SplineList)
{
    SPLINE* temp = SplineList.Get(Number);
    delete[] temp->Control;
    delete[] temp->Output;
    SplineList.Delete(Number);
    delete temp;
}

float LoadSplines(char* SplineFileName, LinkedList<SPLINE>& SplineList)
{
	float time;

    const int stringLength = 33;
    char tempString[stringLength];
    FILE* SplineFile;
    SplineFile = fopen(SplineFileName, "rt");
    if (!SplineFile)
    {
        MessageBox(NULL, "Spline File didn't open", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }
    fseek(SplineFile, 0, SEEK_SET);
    int InitialNumSplines = numSplines;
	g_pLog->Log(0, "numSplines : %d", numSplines);
    if (!fgets(tempString, stringLength, SplineFile))
    {
        MessageBox(NULL, "Error reading from the spline data file", "Error", MB_OK | MB_ICONERROR);
        numSplines = InitialNumSplines;
        return FALSE;
    }
    numSplines = atoi(tempString);         //2
    if (InitialNumSplines > numSplines)
    {
        for (int loop = InitialNumSplines - 1; loop >= numSplines; loop--)
        {
            DeleteSpline(loop, SplineList);
        }
    }
    if (numSplines > InitialNumSplines)
    {
        for (int loop = InitialNumSplines; loop < numSplines; loop++)
        {
            AddSpline(loop, SplineList);
        }
    }
    for (int loop = 0; loop < numSplines; loop++)
    {
        spline = SplineList.Get(loop);
        // Active flag
        fgets(tempString, stringLength, SplineFile);
        spline->Active = atoi(tempString);
        // Repeat flag
        fgets(tempString, stringLength, SplineFile);
        spline->Repeat = atoi(tempString);
        // Degree
        fgets(tempString, stringLength, SplineFile);
        spline->Degree = atoi(tempString);
        // NumControl
        fgets(tempString, stringLength, SplineFile);
        spline->NumControl = atoi(tempString);
        // NumPoints
        fgets(tempString, stringLength, SplineFile);
        spline->NumPoints = atoi(tempString);
        // StartTime
        fgets(tempString, stringLength, SplineFile);
        spline->StartTime = atof(tempString);
        // EndTime
        fgets(tempString, stringLength, SplineFile);
        spline->EndTime = atof(tempString);
		time = spline->EndTime;
        // Red
        fgets(tempString, stringLength, SplineFile);
        spline->Red = atof(tempString);
        // Green
        fgets(tempString, stringLength, SplineFile);
        spline->Green = atof(tempString);
        // Blue
        fgets(tempString, stringLength, SplineFile);
        spline->Blue = atof(tempString);

        for (int loop2 = 0; loop2 <= spline->NumControl; loop2++)
        {
            fgets(tempString, stringLength, SplineFile);
            spline->Control[loop2].x = atof(tempString);
            fgets(tempString, stringLength, SplineFile);
            spline->Control[loop2].y = atof(tempString);
            fgets(tempString, stringLength, SplineFile);
            spline->Control[loop2].z = atof(tempString);
        }
        spline->CopyOfStartTime = spline->StartTime;
        spline->CopyOfEndTime = spline->EndTime;
    }

    currentSpline = numSplines - 1;
    if (lookAtPath >= numSplines)
        lookAtPath = numSplines - 1;
    if (fclose(SplineFile))
        MessageBox(NULL, "File didn't close", "Error", MB_OK | MB_ICONERROR);

    return time;
}

float SetSplines(LinkedList<SPLINE>& SplineList)
{
    srand((unsigned)time(NULL));
    //Create a new spline
    for (int loop = 0; loop < numSplines; loop++)
    {
        AddSpline(loop, SplineList);
    }
    //Load splines from data file
    return LoadSplines(SplineFileName, SplineList);
}

int SetGLTexture(TEXTURE* texture)
{
    glEnable(GL_TEXTURE_2D);

    /*sprintf(texture[0].TexName, "%s", "muzzleflash.tga");
    if (!texture[0].LoadTGA())
    {
        MessageBox(NULL,"Failed to load floor image","Error",MB_OK|MB_ICONERROR);
        return FALSE;
    }

	sprintf(texture[1].TexName, "%s", "particle1.tga");
    if (!texture[1].LoadTGA(GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR, GL_CLAMP, GL_CLAMP, 1))
    {
        MessageBox(NULL,"Failed to load particle1 image","Error",MB_OK|MB_ICONERROR);
        return FALSE;
    }

	sprintf(texture[2].TexName, "%s", "targetpoint.tga");
    if (!texture[2].LoadTGA())
    {
        MessageBox(NULL,"Failed to load floor image","Error",MB_OK|MB_ICONERROR);
        return FALSE;
    }

	sprintf(texture[3].TexName, "%s", "bullet.tga");
    if (!texture[3].LoadTGA())
    {
        MessageBox(NULL,"Failed to load floor image","Error",MB_OK|MB_ICONERROR);
        return FALSE;
    }*/

	sprintf(texture[1].TexName, "%s", "north2.tga");
    texture[1].LoadTGA();

    sprintf(texture[2].TexName, "%s", "east2.tga");
    texture[2].LoadTGA();

    sprintf(texture[3].TexName, "%s", "south2.tga");
    texture[3].LoadTGA();

    sprintf(texture[4].TexName, "%s", "west2.tga");
    texture[4].LoadTGA();

    sprintf(texture[5].TexName, "%s", "sky.tga");
    texture[5].LoadTGA();

    /*sprintf(texture[6].TexName, "%s", "glare.tga");
    texture[6].LoadTGA();*/
    
    return TRUE;
}

GLuint LoadTexture(char * filename)
{
	AUX_RGBImageRec *pBitmap = NULL;
	FILE *pFile = NULL;									// The File Handle we will use to read the bitmap

	pFile = fopen(filename,"r");						// Check To See If The File Exists

	if(pFile)											// If we have a valid file pointer we found the file
	{
		pBitmap = auxDIBImageLoad(filename);			// Load the bitmap and store the data
	}
	else												// If we can't find the file, quit!
	{													// Prompt the error message
		//MessageBox("Couldn't find a texture!", "Error!", MB_OK);
		exit(0);
	}
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pBitmap->sizeX, pBitmap->sizeY, 0, 
		GL_RGB, GL_UNSIGNED_BYTE, pBitmap->data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	return GLuint();
}

float GetTimePassed(float& lasttime, int average, float* lastmultiplier)
{
    float timeoffset;
    float currenttime;
/*
If the program has just started, set last tickcount to the current tickcount.
This prevents the program from thinking that several hours have passed since the last frame.
*/
    if (lasttime == 0)
        lasttime = (float)GetTickCount();
        // Get the current time
    currenttime = (float)GetTickCount();
        // Calculate the offset
    timeoffset = currenttime - lasttime;
    // If timeoffset less than 1/120 of a second (in milliseconds) then set to minimum offset
    if (timeoffset < 8.333333)
    {
        timeoffset = 8.333333;
        currenttime = lasttime + timeoffset;
    }
    // Put the current time in the lasttime variable
        lasttime = currenttime;
    // return the time offset in seconds per frame
        multiplier = timeoffset / 1000;
    for (int loop = 0; loop < average - 1; loop++)
    {
        lastmultiplier[loop] = lastmultiplier[loop + 1];
    }
    lastmultiplier[average - 1] = multiplier;
    for (int loop = 0; loop < average - 1; loop++)
        multiplier += lastmultiplier[loop];
    multiplier /= (float)average; 
        if (multiplier)
        fps = (int)(1.0 / multiplier);
    return multiplier;
}

void DrawSkybox(CCamera g_Camera, TEXTURE* texture)
{
    VECTOR facenormal;
    VECTOR SkyboxVertex[8];

    // make the skybox relative to the camera position

	//SkyboxVertex[0].x = g_Camera.Position().x - 500;
 //   SkyboxVertex[0].y = g_Camera.Position().y - 100;
 //   SkyboxVertex[0].z = g_Camera.Position().z + 500;

 //   SkyboxVertex[1].x = g_Camera.Position().x - 500;
 //   SkyboxVertex[1].y = g_Camera.Position().y - 100;
 //   SkyboxVertex[1].z = g_Camera.Position().z - 500;

 //   SkyboxVertex[2].x = g_Camera.Position().x + 500;
 //   SkyboxVertex[2].y = g_Camera.Position().y - 100;
 //   SkyboxVertex[2].z = g_Camera.Position().z - 500;

 //   SkyboxVertex[3].x = g_Camera.Position().x + 500;
 //   SkyboxVertex[3].y = g_Camera.Position().y - 100;
 //   SkyboxVertex[3].z = g_Camera.Position().z + 500;

 //   SkyboxVertex[4].x = g_Camera.Position().x - 500;
 //   SkyboxVertex[4].y = g_Camera.Position().y + 200;
 //   SkyboxVertex[4].z = g_Camera.Position().z + 500;

 //   SkyboxVertex[5].x = g_Camera.Position().x - 500;
 //   SkyboxVertex[5].y = g_Camera.Position().y + 200;
 //   SkyboxVertex[5].z = g_Camera.Position().z - 500;

 //   SkyboxVertex[6].x = g_Camera.Position().x + 500;
 //   SkyboxVertex[6].y = g_Camera.Position().y + 200;
 //   SkyboxVertex[6].z = g_Camera.Position().z - 500;

 //   SkyboxVertex[7].x = g_Camera.Position().x + 500;
 //   SkyboxVertex[7].y = g_Camera.Position().y + 200;
 //   SkyboxVertex[7].z = g_Camera.Position().z + 500;

	//SkyboxVertex[0].x = g_Camera.Position().x - 50.0;
 //   SkyboxVertex[0].y = g_Camera.Position().y - 10.0;
 //   SkyboxVertex[0].z = g_Camera.Position().z + 50.0;

 //   SkyboxVertex[1].x = g_Camera.Position().x - 50.0;
 //   SkyboxVertex[1].y = g_Camera.Position().y - 10.0;
 //   SkyboxVertex[1].z = g_Camera.Position().z - 50.0;

 //   SkyboxVertex[2].x = g_Camera.Position().x + 50.0;
 //   SkyboxVertex[2].y = g_Camera.Position().y - 10.0;
 //   SkyboxVertex[2].z = g_Camera.Position().z - 50.0;

 //   SkyboxVertex[3].x = g_Camera.Position().x + 50.0;
 //   SkyboxVertex[3].y = g_Camera.Position().y - 10.0;
 //   SkyboxVertex[3].z = g_Camera.Position().z + 50.0;

 //   SkyboxVertex[4].x = g_Camera.Position().x - 50.0;
 //   SkyboxVertex[4].y = g_Camera.Position().y + 20.0;
 //   SkyboxVertex[4].z = g_Camera.Position().z + 50.0;

 //   SkyboxVertex[5].x = g_Camera.Position().x - 50.0;
 //   SkyboxVertex[5].y = g_Camera.Position().y + 20.0;
 //   SkyboxVertex[5].z = g_Camera.Position().z - 50.0;

 //   SkyboxVertex[6].x = g_Camera.Position().x + 50.0;
 //   SkyboxVertex[6].y = g_Camera.Position().y + 20.0;
 //   SkyboxVertex[6].z = g_Camera.Position().z - 50.0;

 //   SkyboxVertex[7].x = g_Camera.Position().x + 50.0;
 //   SkyboxVertex[7].y = g_Camera.Position().y + 20.0;
 //   SkyboxVertex[7].z = g_Camera.Position().z + 50.0;

	SkyboxVertex[0].x = g_Camera.Position().x - 250;
    SkyboxVertex[0].y = g_Camera.Position().y - 50;
    SkyboxVertex[0].z = g_Camera.Position().z + 250;

    SkyboxVertex[1].x = g_Camera.Position().x - 250;
    SkyboxVertex[1].y = g_Camera.Position().y - 50;
    SkyboxVertex[1].z = g_Camera.Position().z - 250;

    SkyboxVertex[2].x = g_Camera.Position().x + 250;
    SkyboxVertex[2].y = g_Camera.Position().y - 50;
    SkyboxVertex[2].z = g_Camera.Position().z - 250;

    SkyboxVertex[3].x = g_Camera.Position().x + 250;
    SkyboxVertex[3].y = g_Camera.Position().y - 50;
    SkyboxVertex[3].z = g_Camera.Position().z + 250;

    SkyboxVertex[4].x = g_Camera.Position().x - 250;
    SkyboxVertex[4].y = g_Camera.Position().y + 100;
    SkyboxVertex[4].z = g_Camera.Position().z + 250;

    SkyboxVertex[5].x = g_Camera.Position().x - 250;
    SkyboxVertex[5].y = g_Camera.Position().y + 100;
    SkyboxVertex[5].z = g_Camera.Position().z - 250;

    SkyboxVertex[6].x = g_Camera.Position().x + 250;
    SkyboxVertex[6].y = g_Camera.Position().y + 100;
    SkyboxVertex[6].z = g_Camera.Position().z - 250;

    SkyboxVertex[7].x = g_Camera.Position().x + 250;
    SkyboxVertex[7].y = g_Camera.Position().y + 100;
    SkyboxVertex[7].z = g_Camera.Position().z + 250;

	//SkyboxVertex[0].x = - 250;
 //   SkyboxVertex[0].y = - 50;
 //   SkyboxVertex[0].z = + 250;

 //   SkyboxVertex[1].x = - 250;
 //   SkyboxVertex[1].y = - 50;
 //   SkyboxVertex[1].z = - 250;

 //   SkyboxVertex[2].x = + 250;
 //   SkyboxVertex[2].y = - 50;
 //   SkyboxVertex[2].z = - 250;

 //   SkyboxVertex[3].x = + 250;
 //   SkyboxVertex[3].y = - 50;
 //   SkyboxVertex[3].z = + 250;

 //   SkyboxVertex[4].x = - 250;
 //   SkyboxVertex[4].y = + 100;
 //   SkyboxVertex[4].z = + 250;

 //   SkyboxVertex[5].x = - 250;
 //   SkyboxVertex[5].y = + 100;
 //   SkyboxVertex[5].z = - 250;

 //   SkyboxVertex[6].x = + 250;
 //   SkyboxVertex[6].y = + 100;
 //   SkyboxVertex[6].z = - 250;

 //   SkyboxVertex[7].x = + 250;
 //   SkyboxVertex[7].y = + 100;
 //   SkyboxVertex[7].z = + 250;

    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
	
	//glEnable(GL_TEXTURE_2D);

 //   // Left Face
 //   glBindTexture(GL_TEXTURE_2D, texture[1].TexID);
 //   glEnable(GL_TEXTURE_2D);//
	//glBegin(GL_QUADS);
 //       facenormal = GetNormal(SkyboxVertex[6], SkyboxVertex[0], SkyboxVertex[3]);
 //       glNormal3fv(&facenormal.x);
 //       //glTexCoord2f(0.0f, 0.0f); glVertex3fv(&SkyboxVertex[0].x);
 //       //glTexCoord2f(1.0f, 0.0f); glVertex3fv(&SkyboxVertex[1].x);
 //       //glTexCoord2f(1.0f, 1.0f); glVertex3fv(&SkyboxVertex[5].x);
 //       //glTexCoord2f(0.0f, 1.0f); glVertex3fv(&SkyboxVertex[4].x);

	//	glTexCoord2f(0.0f, 0.0f); glVertex3f(SkyboxVertex[0].x,SkyboxVertex[0].y-1,SkyboxVertex[0].z+1);
 //       glTexCoord2f(1.0f, 0.0f); glVertex3f(SkyboxVertex[1].x,SkyboxVertex[1].y-1,SkyboxVertex[1].z-1);
 //       glTexCoord2f(1.0f, 1.0f); glVertex3f(SkyboxVertex[5].x,SkyboxVertex[5].y+1,SkyboxVertex[5].z-1);
 //       glTexCoord2f(0.0f, 1.0f); glVertex3f(SkyboxVertex[4].x,SkyboxVertex[4].y+1,SkyboxVertex[4].z+1);


 //   glEnd();
	//glDisable(GL_TEXTURE_2D);//
	//glPopMatrix();//

	//glPushMatrix();//
 //   // Back Face
 //   glBindTexture(GL_TEXTURE_2D, texture[2].TexID);//2->1·Î ¹Ù²Þ
 //   glEnable(GL_TEXTURE_2D);
	//glBegin(GL_QUADS);

 //       facenormal = GetNormal(SkyboxVertex[4], SkyboxVertex[5], SkyboxVertex[7]);
 //       glNormal3fv(&facenormal.x);
 //       glTexCoord2f(0.0f, 0.0f); glVertex3f(SkyboxVertex[1].x-1,SkyboxVertex[1].y-1,SkyboxVertex[1].z);
 //       glTexCoord2f(1.0f, 0.0f); glVertex3f(SkyboxVertex[2].x+1,SkyboxVertex[2].y-1,SkyboxVertex[2].z);
 //       glTexCoord2f(1.0f, 1.0f); glVertex3f(SkyboxVertex[6].x-1,SkyboxVertex[6].y+1,SkyboxVertex[6].z);
 //       glTexCoord2f(0.0f, 1.0f); glVertex3f(SkyboxVertex[5].x+1,SkyboxVertex[5].y+1,SkyboxVertex[5].z);
 //   glEnd();
	//glDisable(GL_TEXTURE_2D);
 //   glPopMatrix();
	//
	//// Right Face
	//glPushMatrix();
 //   glBindTexture(GL_TEXTURE_2D, texture[3].TexID);//3->4·Î¹Ù²Þ
 //   glEnable(GL_TEXTURE_2D);
	//glBegin(GL_QUADS);
 //       facenormal = GetNormal(SkyboxVertex[1], SkyboxVertex[7], SkyboxVertex[5]);
 //       glNormal3fv(&facenormal.x);
 //       glTexCoord2f(0.0f, 0.0f); glVertex3f(SkyboxVertex[2].x,SkyboxVertex[2].y-1,SkyboxVertex[2].z-1);
 //       glTexCoord2f(1.0f, 0.0f); glVertex3f(SkyboxVertex[3].x,SkyboxVertex[3].y-1,SkyboxVertex[3].z+1);
 //       glTexCoord2f(1.0f, 1.0f); glVertex3f(SkyboxVertex[7].x,SkyboxVertex[7].y+1,SkyboxVertex[7].z+1);
 //       glTexCoord2f(0.0f, 1.0f); glVertex3f(SkyboxVertex[6].x,SkyboxVertex[6].y+1,SkyboxVertex[6].z-1);
 //   glEnd();
	//glDisable(GL_TEXTURE_2D);
 //   glPopMatrix();
	//
	//// Front Face
	//glPushMatrix();//
 //   glBindTexture(GL_TEXTURE_2D, texture[4].TexID);//4->1·Î	¹Ù²Þ
 //   glEnable(GL_TEXTURE_2D);//
	//glBegin(GL_QUADS);
 //       glColor3f(1.0f,1.0f,1.0f);
 //       facenormal = GetNormal(SkyboxVertex[0], SkyboxVertex[1], SkyboxVertex[2]);
 //       glNormal3fv(&facenormal.x);
 //       glTexCoord2f(0.0f, 0.0f); glVertex3f(SkyboxVertex[3].x+1,SkyboxVertex[3].y-1,SkyboxVertex[3].z);
 //       glTexCoord2f(1.0f, 0.0f); glVertex3f(SkyboxVertex[0].x-1,SkyboxVertex[0].y-1,SkyboxVertex[0].z);
 //       glTexCoord2f(1.0f, 1.0f); glVertex3f(SkyboxVertex[4].x-1,SkyboxVertex[4].y+1,SkyboxVertex[4].z);
 //       glTexCoord2f(0.0f, 1.0f); glVertex3f(SkyboxVertex[7].x+1,SkyboxVertex[7].y+1,SkyboxVertex[7].z);
 //   glEnd();
	//glDisable(GL_TEXTURE_2D);
	//glPopMatrix();

    // Top Face
    glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[5].TexID);
    glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
        facenormal = GetNormal(SkyboxVertex[3], SkyboxVertex[2], SkyboxVertex[5]);
        glNormal3fv(&facenormal.x);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(SkyboxVertex[7].x+1,SkyboxVertex[7].y,SkyboxVertex[7].z+1);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(SkyboxVertex[4].x-1,SkyboxVertex[4].y,SkyboxVertex[4].z+1);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(SkyboxVertex[5].x-1,SkyboxVertex[5].y,SkyboxVertex[5].z-1);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(SkyboxVertex[6].x+1,SkyboxVertex[6].y,SkyboxVertex[6].z-1);
    glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

    //glPopMatrix();
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
}

VECTOR GetNormal(VECTOR vertex1, VECTOR vertex2, VECTOR vertex3)
{
        float ux, uy, uz, vx, vy, vz;
          VECTOR temp_vertex;

          ux = vertex1.x - vertex2.x;
          uy = vertex1.y - vertex2.y;
          uz = vertex1.z - vertex2.z;
          vx = vertex3.x - vertex2.x;
          vy = vertex3.y - vertex2.y;
          vz = vertex3.z - vertex2.z;
          temp_vertex.x = (uy*vz)-(vy*uz);
          temp_vertex.y = (uz*vx)-(vz*ux);
          temp_vertex.z = (ux*vy)-(vx*uy);
          return temp_vertex;
}
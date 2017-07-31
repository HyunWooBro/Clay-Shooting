#include <windows.h>
#include "main.h"
#include "bspline.h"
#include "tll.h"

#include "Camera.h"
#include "matrix.h"
//#include "light.h"

#include "ZFLog.h"


//extern LIGHT* light;
//extern int currentLight;
//extern CCamera g_Camera;
//extern int currentCamera;
extern float ApplicationStartTime;              // Start time required for spline timing
extern int numSplines;
extern int visible;
extern int cameraMode;
extern int currentSpline;
extern int lookAtPath;
extern char SplineFileName[MAX_PATH];
extern SPLINE* spline;

extern ZFLog *g_pLog;

extern CCamera	g_Camera;

int SPLINE::Compare(const SPLINE& Spline)
{
  if (linkPosition < Spline.linkPosition)
    return smaller;
  if (linkPosition > Spline.linkPosition)
    return bigger;
  else
    return same;
}

/*********************************************************************

 Simple b-spline curve algorithm

 Copyright 1994 by Keith Vertanen (vertankd@cda.mrs.umn.edu)
 Released to the public domain (your mileage may vary)
    Modified by Alan Baylis 2001 (thegoodalien@inorbit.com)

**********************************************************************/

// puts the number of points specified by NumPoints in the spline's Output array
void bspline(SPLINE* spline)
{
    int t = spline->Degree;
    int n = spline->NumControl;
    int num_output = spline->NumPoints;
    VECTOR* control = spline->Control;

    int* u;

    int output_index;
    float increment,interval;
    VECTOR calcxyz;

    u = new int[n+t+1];
    compute_intervals(u, n, t);

    increment = (float) (n-t+2) / (num_output-1);  // how much parameter goes up each time
    interval = 0;

    for (output_index = 0; output_index < num_output - 1; output_index++)
    {
        compute_point(u, n, t, interval, control, &calcxyz);
        spline->Output[output_index].x = calcxyz.x;
        spline->Output[output_index].y = calcxyz.y;
        spline->Output[output_index].z = calcxyz.z;
        interval = interval + increment;  // increment our parameter
    }
    spline->Output[num_output-1].x = control[n].x;   // put in the last point
    spline->Output[num_output-1].y = control[n].y;
    spline->Output[num_output-1].z = control[n].z;

    delete u;
}

// returns a single point based on the spline start and end times
VECTOR bsplinepoint(SPLINE* spline)
{
    int t = spline->Degree;
    int n = spline->NumControl;
    VECTOR *control = spline->Control;
//    VECTOR *output = spline->Output;

    int* u;
    float increment;
    VECTOR calcxyz;

    u = new int[n+t+1];
    compute_intervals(u, n, t);

    float CurrentTime = (float)GetTickCount() - ApplicationStartTime;

    float TotalTime = spline->EndTime - spline->StartTime;
    float Position;
    float ActualTime;

    if (CurrentTime <= spline->CopyOfStartTime)
    {
        calcxyz.x = control[0].x;
        calcxyz.y = control[0].y;
        calcxyz.z = control[0].z;
        delete u;
        return calcxyz;
    }
    else if (CurrentTime >= spline->CopyOfEndTime)
    {
        if (spline->Repeat)
        {
            spline->CopyOfStartTime = CurrentTime;
            spline->CopyOfEndTime = CurrentTime + TotalTime;
        }
        calcxyz.x = control[n].x;
        calcxyz.y = control[n].y;
        calcxyz.z = control[n].z;
        delete u;
        return calcxyz;
    }
    else
    {
        ActualTime = CurrentTime - spline->CopyOfStartTime;
        Position = 1 / (TotalTime / ActualTime);
    }
    increment = (n - t + 2) * Position;  // how much parameter goes up each time

    compute_point(u, n, t, increment, control, &calcxyz);

    delete u;

    return calcxyz;
}

float blend(int k, int t, int *u, float v)  // calculate the blending value
{
    float value;

    if (t == 1)            // base case for the recursion
    {
        if ((u[k] <= v) && (v < u[k+1]))
            value = 1;
        else
            value = 0;
    }
    else
    {
        if ((u[k+t-1] == u[k]) && (u[k+t] == u[k+1]))         // check for divide by zero
            value = 0;
        else
            if (u[k+t-1] == u[k])           // if a term's denominator is zero,use just the other
                value = (u[k+t] - v) / (u[k+t] - u[k+1]) * blend(k+1, t-1, u, v);
            else
                if (u[k+t]==u[k+1])
                    value = (v - u[k]) / (u[k+t-1] - u[k]) * blend(k, t-1, u, v);
                else
                    value = (v - u[k]) / (u[k+t-1] - u[k]) * blend(k, t-1, u, v) + (u[k+t] - v) / (u[k+t] - u[k+1]) * blend(k+1, t-1, u, v);
    }
    return value;
}

void compute_intervals(int *u, int n, int t)   // figure out the knots
{
    int j;

    for (j = 0; j <= n+t; j++)
    {
        if (j<t)
            u[j]=0;
        else
            if ((t <= j) && (j <= n))
                u[j] = j-t+1;
            else
                if (j > n)
                    u[j] = n-t+2;  // if n-t=-2 then we're screwed, everything goes to 0
    }
}

void compute_point(int *u, int n, int t, float v, VECTOR *control, VECTOR *output)
{
    int k;
    float temp;

    // initialize the variables that will hold our outputted point
    output->x=0;
    output->y=0;
    output->z=0;

    for (k = 0; k <= n; k++)
    {
        temp = blend(k,t,u,v);  // same blend is used for each dimension coordinate
        output->x = output->x + (control[k]).x * temp;
        output->y = output->y + (control[k]).y * temp;
        output->z = output->z + (control[k]).z * temp;
    }
}

void DrawSplines(LinkedList<SPLINE>& SplineList)
{
    GLUquadricObj * sphere = gluNewQuadric();      // sphere for control points

	//g_pLog->Log(0, "view ");

    for (int loop = 0; loop < numSplines; loop++)     // loop through all splines
    {

		//g_pLog->Log(0, "view2 ");
        spline = SplineList.Get(loop);               // get the spline from the linked list

        if (visible)                                 // if the curves are visible
        {
            // draw the control points
            glDisable(GL_TEXTURE_2D);
            float mat_ambient[] = {spline->Red, spline->Green, spline->Blue, 1.0 };
            float mat_diffuse[] = {spline->Red, spline->Green, spline->Blue, 1.0 };
            float mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
            glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
            for (int i=0; i <= spline->NumControl; i++)
            {
                glPushMatrix();
                glTranslatef(spline->Control[i].x, spline->Control[i].y, spline->Control[i].z);
                gluSphere(sphere,0.4,10,10);
                glPopMatrix();
            }
            glEnable(GL_TEXTURE_2D);

            // get the output point of this spline
            VECTOR Output = bsplinepoint(spline);

            // draw the point
            glPointSize(5.0);
            glColor3f(0.0, 1.0 ,0.0);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_LIGHTING);
            glPushMatrix();
            glBegin(GL_POINTS);
            glVertex3f(Output.x, Output.y, Output.z);
            glEnd();
            glPopMatrix();
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_LIGHTING);

            if (cameraMode > 0 && currentSpline == loop) // if the camera mode is not free and this is the current spline
            {
                VECTOR LookAt;
                if (cameraMode == 2 && (currentSpline != lookAtPath)) // if the camera mode is follow
                {
                    SPLINE* spline2 = SplineList.Get(lookAtPath);
                    LookAt = bsplinepoint(spline2);
                }

					float dx = Output.x - g_Camera.Position().x;
					float dy = Output.y - g_Camera.Position().y;
					float dz = Output.z - g_Camera.Position().z;

                g_Camera.SetPosition(Output.x, Output.y, Output.z);

					g_Camera.SetView(g_Camera.View().x + dx, g_Camera.View().y + dy, g_Camera.View().z + dz);
					//.x = Output.x;  // set the camera position to the current spline output
     //           g_Camera.Position().y = Output.y;
     //           g_Camera.Position().z = Output.z;
                MATRIX mat;
                glPushMatrix();
                glLoadIdentity();
                //if (cameraMode == 2 && (currentSpline != lookAtPath)) // if the camera mode is follow
                //    gluLookAt(Output.x, Output.y, Output.z, LookAt.x, LookAt.y, LookAt.z, 0, 1, 0);
                //else
                                                                      // else make the camera look at another position
                    //gluLookAt(Output.x, Output.y, Output.z, light[currentLight].Position.x, light[currentLight].Position.y, light[currentLight].Position.z, 0, 1, 0);
                glGetFloatv(GL_MODELVIEW_MATRIX, mat.Element);       // get the gluLookAt matrix
                glPopMatrix();

                float mat2[4][4];
                mat2[0][0] = mat.Element[0];
                mat2[0][1] = mat.Element[1];
                mat2[0][2] = mat.Element[2];
                mat2[0][3] = mat.Element[3];
                mat2[1][0] = mat.Element[4];
                mat2[1][1] = mat.Element[5];
                mat2[1][2] = mat.Element[6];
                mat2[1][3] = mat.Element[7];
                mat2[2][0] = mat.Element[8];
                mat2[2][1] = mat.Element[9];
                mat2[2][2] = mat.Element[10];
                mat2[2][3] = mat.Element[11];
                mat2[3][0] = mat.Element[12];
                mat2[3][1] = mat.Element[13];
                mat2[3][2] = mat.Element[14];
                mat2[3][3] = mat.Element[15];

                //g_Camera.Orientation.MatrixToQuat(mat2);  // set the camera orientation to the gluLookAt matrix
            }

            bspline(spline);             // get the output points of this spline

            // draw the curve along to the output points
            glColor3f(spline->Red, spline->Green, spline->Blue);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_LIGHTING);
            glPushMatrix();
            glBegin(GL_LINE_STRIP);
            for (int i=0; i < spline->NumPoints; i++)
            {
                glVertex3fv(&spline->Output[i].x);
            }
            glEnd();
            glPopMatrix();
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_LIGHTING);
        }

        else if (cameraMode > 0) // if the curves are not visible and the camera mode is not free
        {
            VECTOR Output = bsplinepoint(spline);

            VECTOR LookAt;
            if (cameraMode == 2 && currentSpline != lookAtPath)
            {
                SPLINE* spline2 = SplineList.Get(lookAtPath);
                LookAt = bsplinepoint(spline2);
            }

				float dx = Output.x - g_Camera.Position().x;
				float dy = Output.y - g_Camera.Position().y;
				float dz = Output.z - g_Camera.Position().z;

            g_Camera.SetPosition(Output.x, Output.y, Output.z);

				g_Camera.SetView(g_Camera.View().x + dx, g_Camera.View().y + dy, g_Camera.View().z + dz);

            /*camera[currentCamera].Position.x = Output.x;
            camera[currentCamera].Position.y = Output.y;
            camera[currentCamera].Position.z = Output.z;*/
            MATRIX mat;
            glPushMatrix();
            glLoadIdentity();
            //if (cameraMode == 2 && currentSpline != lookAtPath)
            //    gluLookAt(Output.x, Output.y, Output.z, LookAt.x, LookAt.y, LookAt.z, 0, 1, 0);
           // else
            //    gluLookAt(Output.x, Output.y, Output.z, light[currentLight].Position.x, light[currentLight].Position.y, light[currentLight].Position.z, 0, 1, 0);
            glGetFloatv(GL_MODELVIEW_MATRIX, mat.Element);
            glPopMatrix();

            float mat2[4][4];
            mat2[0][0] = mat.Element[0];
            mat2[0][1] = mat.Element[1];
            mat2[0][2] = mat.Element[2];
            mat2[0][3] = mat.Element[3];
            mat2[1][0] = mat.Element[4];
            mat2[1][1] = mat.Element[5];
            mat2[1][2] = mat.Element[6];
            mat2[1][3] = mat.Element[7];
            mat2[2][0] = mat.Element[8];
            mat2[2][1] = mat.Element[9];
            mat2[2][2] = mat.Element[10];
            mat2[2][3] = mat.Element[11];
            mat2[3][0] = mat.Element[12];
            mat2[3][1] = mat.Element[13];
            mat2[3][2] = mat.Element[14];
            mat2[3][3] = mat.Element[15];

            //g_Camera.Orientation.MatrixToQuat(mat2);
        }
    }
}

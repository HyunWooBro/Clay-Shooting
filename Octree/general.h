#pragma once

#include "bspline.h"
#include "tll.h"
#include "Texture.h"
#include "Camera.h"

void AddSpline(int Number, LinkedList<SPLINE>& SplineList);
void DeleteSpline(int Number, LinkedList<SPLINE>& SplineList);
float LoadSplines(char* SplineFileName, LinkedList<SPLINE>& SplineList);
float SetSplines(LinkedList<SPLINE> &);

int SetGLTexture(TEXTURE* texture);
GLuint LoadTexture(char * filename);
float GetTimePassed(float& lasttime, int average, float* lastmultiplier);
void DrawSkybox(CCamera g_Camera, TEXTURE* texture);
VECTOR GetNormal(VECTOR vertex1, VECTOR vertex2, VECTOR vertex3);
#include "Target.h"
#include "main.h"
#include <mmsystem.h>

//extern ZFLog* g_pLog;

extern float gravity;

void Target::DrawTarget()
{
	glDisable(GL_TEXTURE_2D);
    
	glPushMatrix();

	o_vPosition.x += velocity.x;		
	o_vPosition.y += velocity.y;	
	o_vPosition.z += velocity.z;

	velocity.y -= gravity;
	if(velocity.y < -0.1)
		velocity.y = -0.1;

	glTranslatef(o_vPosition.x,o_vPosition.y,o_vPosition.z);
	//Sphere = gluNewQuadric();
	//gluQuadricOrientation(Sphere, GLU_OUTSIDE);
	gluSphere(Sphere,radius,50,50);
	glPopMatrix(); 


	glEnable(GL_TEXTURE_2D);

	//g_pLog->Log(0, "position = %f, %f, %f", o_vPosition.x, o_vPosition.y, o_vPosition.z);
}

void Target::DestroyTarget()
{
	// 불꽃 텍스쳐 보여주자.
}
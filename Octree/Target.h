#ifndef _Target_H
#define _Target_H

#include "main.h"
#include <mmsystem.h>

class Target {

public:

	Target(CVector3 o_vPosition1, CVector3 velocity1, float radius1, float delay = 0){

		o_vPosition = o_vPosition1;
		velocity = velocity1;
		radius = radius1;
		
		//glPushMatrix();
		//glTranslatef(o_vPosition.x,o_vPosition.y,o_vPosition.z);
		Sphere = gluNewQuadric();
		gluQuadricOrientation(Sphere, GLU_OUTSIDE);
		gluSphere(Sphere,radius,50,50);

		isActive = false;

		this->delay = delay + (float)GetTickCount();
		//glPopMatrix(); 
	}

	CVector3 Position() {return o_vPosition;}
	float Radius() {return radius;}
	float GetDelay() {return delay;}

	void MoveTarget() {}

	void DrawTarget();

	int isTargetActive() {return isActive;}

	void EnableTarget() {isActive = true;}

	void DisableTarget() {isActive = false;}

	void DestroyTarget();

private:

	CVector3 o_vPosition;													
	
	float radius;

	float delay;

	CVector3 velocity;

	GLUquadricObj *Sphere;

	bool isActive;
};

#endif

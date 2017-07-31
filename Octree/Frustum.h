#ifndef _FRUSTUM_H
#define _FRUSTUM_H

#include "main.h"

class CFrustum {

public:
	void CalculateFrustum();

	bool PointInFrustum(float x, float y, float z);

	bool SphereInFrustum(float x, float y, float z, float radius);

	bool CubeInFrustum( float x, float y, float z, float size );

private:
	float m_Frustum[6][4];
};


#endif


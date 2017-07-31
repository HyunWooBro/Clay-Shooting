// Collision detection routines and helper functions 
// (by Alan Baylis 2001, Adapted from the work of Kasper Fauerby - aka Telemachos)

#include <windows.h>
#include <math.h>
#include "main.h"
#include "collision.h"
#include "vector.h"

GLfloat MagnitudeVector(VECTOR vec1)
{
  return(sqrt(vec1.x*vec1.x+vec1.y*vec1.y+vec1.z*vec1.z));
}

float DotProduct(VECTOR vec1, VECTOR vec2)
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


float IntersectRaySphere(VECTOR rO, VECTOR rV, VECTOR sO, float sR) 
{
   VECTOR TempVect;
   TempVect.x = sO.x - rO.x;
   TempVect.y = sO.y - rO.y;
   TempVect.z = sO.z - rO.z;
   VECTOR Q = TempVect;
   
   float c = MagnitudeVector(Q);
   float v = DotProduct(Q,rV);
   float d = sR*sR - (c*c - v*v);

   // If there was no intersection, return -1
   if (d < 0.0) return (-1.0f);

   // Return the distance to the [first] intersecting point
   return (v - sqrt(d));
}

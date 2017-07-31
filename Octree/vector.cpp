#include <windows.h>
#include "vector.h"


VECTOR::VECTOR(float sx, float sy, float sz)
:
    x(sx),
    y(sy),
    z(sz)
{
}

VECTOR::~VECTOR()
{
}

void VECTOR::Reset()
{
    x = 0.0;
    y = 0.0;
    z = 0.0;
}

float VECTOR::DotProduct(VECTOR vect)
{
      float dot;
      dot = vect.x * x + vect.y * y + vect.z * z;
      return dot;
}

void VECTOR::CrossVector(VECTOR vect)
{
      VECTOR temp = *this;
      x = vect.y * temp.z - vect.z * temp.y;
      y = vect.z * temp.x - vect.x * temp.z;
      z = vect.x * temp.y - vect.y * temp.x;
}

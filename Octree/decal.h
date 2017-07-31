// Decal Class    by Alan Baylis 2001

#ifndef DecalH
#define DecalH

#include "vector.h"
#include "vertex.h"
#include "quat.h"

class DECAL
{
    public:
        DECAL();
          ~DECAL();

    int Compare(const DECAL& Decal);
    int GetMyPosition() const {return linkPosition;}
    void SetMyPosition(int newPosition) {linkPosition = newPosition;}
    int linkPosition;
    
    int type;
    int flag;
    int counter;
    int active;
    VECTOR CollisionVector;
    VERTEX Vertex[4];
    float Size;
    VECTOR Position;
    QUAT Orientation;
};

void DrawMuzzleFlash();

#endif

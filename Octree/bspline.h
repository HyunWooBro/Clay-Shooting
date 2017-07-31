// Spline Class    by Alan Baylis 2001

#ifndef SplineH
#define SplineH

#include "vector.h"
#include "tll.h"

class SPLINE
{
public:
      SPLINE(){};
      ~SPLINE(){};

    int Compare(const SPLINE& Spline);
    int GetMyPosition() const {return linkPosition;}
    void SetMyPosition(int newPosition) {linkPosition = newPosition;}
    int linkPosition;
    int Degree;                   // the degree of the polynomial plus 1
    int NumControl;               // number of control points minus 1
    int NumPoints;                // number of points to output      *not used in bsplinepoint()
    VECTOR* Control;              // control point array
    VECTOR* Output;               // output array                    *not used in bsplinepoint()
    float StartTime;            // start of spline movement        *used in bsplinepoint()
    float EndTime;              // end of spline movement          *used in bsplinepoint()
    float CopyOfStartTime;      // work variable
    float CopyOfEndTime;        // work variable
    bool Active;                  // active flag
    bool Repeat;                  // flag for looped output          *used in bsplinepoint()
    float Red;                  // color
    float Green;
    float Blue;
};

void bspline(SPLINE* spline);
VECTOR bsplinepoint(SPLINE* spline);
float blend(int k, int t, int *u, float v); // calculate the blending value
void compute_intervals(int *u, int n, int t); // figure out the knots
void compute_point(int *u, int n, int t, float v, VECTOR *control, VECTOR *output);
void DrawSplines(LinkedList<SPLINE>&);

#endif


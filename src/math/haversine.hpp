#ifndef HAVERSINE_H
#define HAVERSINE_H
#include <math.h>

/* ========================================================================
   LISTING 65
   ======================================================================== */

typedef double f64;

f64 Square(f64 A);

f64 RadiansFromDegrees(f64 Degrees);

// NOTE(casey): EarthRadius is generally expected to be 6372.8
f64 ReferenceHaversine(f64 X0, f64 Y0, f64 X1, f64 Y1);

#endif // HAVERSINE_H

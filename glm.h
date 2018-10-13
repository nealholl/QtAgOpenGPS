#ifndef GLM_H
#define GLM_H

#include <math.h>
#include <limits>

#include "vec2.h"
#include "vec3.h"

#ifndef M_PI
#  ifndef PI
#     define M_PI 3.14159265358979323846
#  else
#     define M_PI PI
#  endif
#endif

namespace glm
{
    const double DOUBLE_EPSILON=std::numeric_limits<double>::epsilon();

    //inches to meters
    const double in2m = 0.0254;

    //meters to inches
    const double m2in = 39.3701;

    //meters to feet
     const double m2ft = 3.28084;

    // liters per hectare to us gal per acre
     const double galAc2Lha = 9.35396;

    //us gal per acre to liters per hectare
     const double LHa2galAc = 0.106907;

    //Liters to Gallons
     const double L2Gal = 0.264172;

    //Gallons to Liters
     const double Gal2L = 3.785412534258;

    //the pi's
     const double twoPI = 6.28318530717958647692;

     const double PIBy2 = 1.57079632679489661923;

    /*
     inline float acos(float x)
    {
        return acosf(x);
    }

     inline float acosh(float x)
    {
        return acoshf(x);
    }

     inline float asin(float x)
    {
        return asinf(x);
    }

     inline float asinh(float x)
    {
        return asinhf(x);
    }

     inline float atan(float y, float x)
    {
        return atan2f(y,x);
    }

     inline float atan(float y_over_x)
    {
        return atanf(y_over_x);
    }

     inline float atanh(float x)
    {
        return atanhf(x);
    }

     inline float cos(float angle)
    {
        return cosf(angle);
    }

     inline float cosh(float angle)
    {
        return coshf(angle);
    }
    */
     //Degrees Radians Conversions
       inline float toDegrees(float radians)
      {
          return radians * 180.0 / M_PI;
      }

       inline double toDegrees(double radians)
      {
          return radians * 180.0 / M_PI;
      }

       inline float toRadians(float degrees)
      {
          return degrees * M_PI / 180.0;
      }

       inline double toRadians(double degrees)
      {
          return degrees * M_PI / 180.0;
      }

     //Distance calcs of all kinds
      inline double Distance(double east1, double north1, double east2, double north2)
     {
         return sqrt(pow(east1 - east2, 2) + pow(north1 - north2, 2));
     }

      inline double Distance(Vec2 first, Vec2 second)
     {
         return sqrt(pow(first.easting - second.easting, 2) + pow(first.northing - second.northing, 2));
     }

      inline double Distance(Vec2 first, Vec3 second)
     {
         return sqrt(pow(first.easting - second.easting, 2) + pow(first.northing - second.northing, 2));
     }

      /*
      inline double Distance(CBndPt first, vec3 second)
     {
         return sqrt(pow(first.easting - second.easting, 2) + pow(first.northing - second.northing, 2));
     }

      inline double Distance(CBndPt first, CBndPt second)
     {
         return sqrt(pow(first.easting - second.easting, 2) + pow(first.northing - second.northing, 2));
     }
     */

      inline double Distance(Vec3 first, Vec3 second)
     {
         return sqrt(pow(first.easting - second.easting, 2) + pow(first.northing - second.northing, 2));
     }

      inline double Distance(Vec2 first, double east, double north)
     {
         return sqrt(
             pow(first.easting - east, 2) + pow(first.northing - north, 2));
     }

      inline double Distance(Vec3 first, double east, double north)
     {
         return sqrt(pow(first.easting - east, 2) + pow(first.northing - north, 2));
     }

     //not normalized distance, no square root
      inline double DistanceSquared(double northing1, double easting1, double northing2, double easting2)
     {
         return pow(easting1 - easting2, 2) + pow(northing1 - northing2, 2);
     }

     inline double roundAwayFromZero(double number)
     {
        return (number < 0) ? floor(number) : ceil(number);
     }
}
#endif // GLM_H

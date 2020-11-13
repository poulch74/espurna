/* BEGIN fs_math1.c */

#include "libs/fs_math.h"
#include "libs/fs_math1.h"

#include <float.h>

#define _PI180 (0.01745329f) // pi/180
#define _180PI (57.2957795f) // 180/pi
#define _PI2   (1.5707963f) // half pi
#define _PI18000 (0.00017453f) // pi/18000

#define fs_abs(x) ((x)<0 ? (-x):(x))

#define fs_min(a,b) ((a)<(b) ? (a):(b))
#define fs_max(a,b) ((a)>(b) ? (a):(b))

#define fs_sin(x) (float(fs_cos(x-_PI2)-fs_cos(x+_PI2))/2.0f)

float fs_acos(float x)
{
    float negate = (float)(x<0);
    x=fs_abs(x);
    float ret = -0.0187293f;
    ret *= x;
    ret += 0.0742610f;
    ret *=  x;
    ret -= 0.2121144f;
    ret *= x;
    ret += 1.5707288f;
    ret *= (float)fs_sqrt(1.0-x);
    ret = ret - 2 * negate * ret;
    return negate * 3.141592654f + ret;
}

float fs_atan2(float y, float x)
{
    float t0, t1, t2, t3, t4;

    t3 = fs_abs(x);
    t1 = fs_abs(y);
    t2 = (t1>t3);
    t0 = fs_max(t3, t1);
    t1 = fs_min(t3, t1);
    t3= t1/t0;

    t4 = t3 * t3;
    t0 =         - 0.013480470f;
    t0 = t0 * t4 + 0.057477314f;
    t0 = t0 * t4 - 0.121239071f;
    t0 = t0 * t4 + 0.195635925f;
    t0 = t0 * t4 - 0.332994597f;
    t0 = t0 * t4 + 0.999995630f;
    t3 = t0 * t3;

    t3 = t2 ? 1.570796327f - t3 : t3;
    t3 = (x < 0) ?  3.141592654f - t3 : t3;
    t3 = (y < 0) ? -t3 : t3;

    return t3;
}

#ifndef H_FS_MATH1_H
#define H_FS_MATH1_H

#define _PI180 (0.01745329f) // pi/180
#define _180PI (57.2957795f) // 180/pi
#define _PI2   (1.5707963f) // half pi
#define _PI18000 (0.00017453f) // pi/18000

#define fs_abs(x) ((x)<0 ? (-x):(x))

#define fs_min(a,b) ((a)<(b) ? (a):(b))
#define fs_max(a,b) ((a)>(b) ? (a):(b))

#define fs_sin(x) (float(fs_cos(x-_PI2)-fs_cos(x+_PI2))/2.0f)

float fs_acos(float x);

float fs_atan2(float y, float x);

#endif
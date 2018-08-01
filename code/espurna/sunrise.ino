#include "libs/Sunrise.h"
extern "C" {
    #include "libs/fs_math.h"
}

#define _PI180 (0.01745329f) // pi/180
#define _180PI (57.2957795f) // 180/pi
#define _PI2   (1.5707963f) // half pi

#define fs_abs(x) ((x)<0 ? (-x):(x))

#define fs_min(a,b) ((a)<(b) ? (a):(b))
#define fs_max(a,b) ((a)>(b) ? (a):(b))

#define fs_sin(x) (float(fs_cos(x-_PI2)-fs_cos(x+_PI2))/2.0f)


float fs_acos(float x)
{
    float negate = float(x<0);
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

static float zcos[] = {-0.01454,-0.10453,-0.20791,-0.30901};

Sunrise::Sunrise(float latitude, float longitude, float timezone)
{
   //lat=latitude;
   sinlat = fs_sin(latitude*_PI180);
   coslat = fs_cos(latitude*_PI180);
   lngHour = longitude/15.0f;
   lngHour24 = lngHour/24.0f;
   tz=timezone;

   hr=255;
   min=0;
}

int Sunrise::calc(int year, unsigned char  month, unsigned char  day, Zenith zenith, bool rs)
{
   int doy;
   float minutes;

   coszenith = zcos[zenith];

   // approximate day of year
   int N1,N2,N3;
   N1 = 275*month/9;
   N2 = (month+9)/12;
   N3 = 1+((year-4*(year/4)+2)/3);
   doy = N1-(N2*N3)+day-30;
   
   float t = doy-lngHour24;
   if(rs) t+=0.25f;
   else t+=0.75f;

   //calculate the Sun's mean anomaly
   float M = (t * 0.9856f) - 3.289f;
   float Mr = M*_PI180;

   float L = M + (1.916f * fs_sin(Mr)) + (0.020f * fs_sin(2.0f * Mr)) + 282.634f;
   L = adjust(L,360);
   
   float RA = fs_atan2(0.91764f * fs_sin(L*_PI180),fs_cos(L*_PI180))*_180PI;
   RA = adjust(RA,360);

   int Lquadrant  = L/90;
   int RAquadrant = RA/90;

   RA = (RA + 90.0f*(Lquadrant - RAquadrant))/15.0f;

   float sinDec = (0.39782f) * fs_sin(L*_PI180);
   float cosDec = fs_sqrt(float(1.0-sinDec*sinDec));

   // calculate the Sun's local hour angle
   float cosH = (coszenith - (sinDec * sinlat)) / (cosDec * coslat);

   // we're in the (ant)arctic and there is no rise(or set) today!
   if(cosH>1.0f){ hr=255; return -1; } // polar night
   if(cosH<(-1.0f)){ hr=255; return -2; } // polar day
    
   float ha=fs_acos(cosH)*_180PI; 

   if(rs==1) ha=360-ha;

   float UT = ha/15.0f + RA - (0.06571f * t) - 6.622f - lngHour;
   UT+=tz;
   UT = adjust(UT,24);

   minutes = UT*(60.0f);
   
   hr=minutes/60;
   min=(int)(minutes-hr*60);

// for test

    if(rs) { //rise
    minutes=(int)(50+19*60);
    } else { // set
    minutes=(int)(53+19*60);
    }
  
   return minutes;
}

Sunrise sun(55.7,37.6,3);
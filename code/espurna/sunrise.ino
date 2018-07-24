#include "libs/Sunrise.h"
extern "C" {
    #include "libs/fs_math.h"
}

#define _PI180 float(0.01745329) // pi/180
#define _180PI float(57.2957795) // 180/pi
#define _PI2 float(1.5707963) // half PI

float fs_floor(float x) {
    int xi = (int)x;
    return (float)(x < xi ? xi - 1 : xi);
}

#define fs_abs(x) ((x)<0 ? (-x):(x))

#define fs_min(a,b) ((a)<(b) ? (a):(b))
#define fs_max(a,b) ((a)>(b) ? (a):(b))

#define fs_sin(x) (float(fs_cos(x-_PI2)-fs_cos(x+_PI2))/float(2.0))
#define fs_tan(x) (fs_sin(x)/fs_cos(x))


float fs_acos(float x)
{
    float negate = float(x<0);
    x=fs_abs(x);
    float ret = float(-0.0187293);
    ret *= x;
    ret += float(0.0742610);
    ret *=  x;
    ret -= float(0.2121144);
    ret *= x;
    ret += float(1.5707288);
    ret *= (float)fs_sqrt(1.0-x);
    ret = ret - 2 * negate * ret;
    return negate * float(3.141592654) + ret;
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
  t0 =         - float(0.013480470);
  t0 = t0 * t4 + float(0.057477314);
  t0 = t0 * t4 - float(0.121239071);
  t0 = t0 * t4 + float(0.195635925);
  t0 = t0 * t4 - float(0.332994597);
  t0 = t0 * t4 + float(0.999995630);
  t3 = t0 * t3;

  t3 = t2 ? float(1.570796327) - t3 : t3;
  t3 = (x < 0) ?  float(3.141592654) - t3 : t3;
  t3 = (y < 0) ? -t3 : t3;

  return t3;
}



Sunrise::Sunrise(float latitude, float longitude, float timezone)
{
   //lat=latitude;
   sinlat = fs_sin(latitude*_PI180);
   coslat = fs_cos(latitude*_PI180);
   lngHour = longitude/float(15.0);
   tz=timezone;
   
   Actual(); // default to actual times
   theHour=255;
   theMinute=0;
}

int Sunrise::Calc(int year, unsigned char  month, unsigned char  day, int rs)
{
   int doy, minutes;
  
   // approximate day of year
   int N1,N2,N3;
   N1 = 275*month/9;
   N2 = (month+9)/12;
   N3 = 1+((year-4*(year/4)+2)/3);
   doy = N1-(N2*N3)+day-30;

   float t = doy+((float(18.)-lngHour)/float(24.));
   if(rs) t = doy+((float(6.)-lngHour)/float(24.));

   //calculate the Sun's mean anomaly
   float M = (float(0.9856) * t) - float(3.289);
   float Mr = M*_PI180;

   float L = M + (float(1.916) * fs_sin(Mr)) + (float(0.020) * fs_sin(float(2.) * Mr)) + float(282.634);
   L = adjust(L,360);
   
   float RA = fs_atan2(float(0.91764) * fs_tan(L*_PI180),1.0)*_180PI;
   RA = adjust(RA,360);

   float Lquadrant  = fs_floor(L/float(90.)) * float(90.);
   float RAquadrant = fs_floor(RA/float(90.)) * float(90.);

   RA = (RA + (Lquadrant - RAquadrant))/float(15.0);

   float sinDec = float(0.39782) * fs_sin(L*_PI180);
   float cosDec = fs_sqrt(float(1.0-sinDec*sinDec));

   // calculate the Sun's local hour angle
   float cosH = (coszenith - (sinDec * sinlat)) / (cosDec * coslat);

   // we're in the (ant)arctic and there is no rise(or set) today!
   if(fs_abs(cosH)>1){ theHour=255; return -1; }
    
   float ha=fs_acos(cosH)*_180PI; 

   if(rs==1) ha=360-ha;

   float UT = ha/float(15.0) + RA - (float(0.06571) * t) - float(6.622) - lngHour;

   minutes = (int)((UT+tz)*float(60.0));
   if(minutes<0) minutes+=1440;
   minutes%=1440;
  
   theHour=minutes/60;
   theMinute=minutes-theHour*60;

// for test
/*
    if(rs) { //rise
        theHour=17;
        theMinute=01;
    } else { // set
        theHour=17;
        theMinute=03;
    }
*/  
   return minutes;
}

Sunrise sun(55.7,37.6,3);
#include "libs/Sunrise.h"
#include <math.h>

#define _PI180 (3.14159/180.0)
#define _180PI (180.0/3.14159)

Sunrise::Sunrise(double latitude, double longitude, double timezone)
{
   //lat=latitude;
   sinlat = sin(latitude*_PI180);
   coslat = cos(latitude*_PI180);
   lngHour = longitude/15.0;
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

   double t = doy+((18.-lngHour)/24.);
   if(rs) t = doy+((6.-lngHour)/24.);

   //calculate the Sun's mean anomaly
   double M = (0.9856 * t) - 3.289;
   double Mr = M*_PI180;

   double L = M + (1.916 * sin(Mr)) + (0.020 * sin(2 * Mr)) + 282.634;
   L = adjust(L,360);
   
   double RA = atan(0.91764 * tan(L*_PI180))*_180PI;
   RA = adjust(RA,360);

   double Lquadrant  = floor(L/90) * 90;
   double RAquadrant = floor(RA/90) * 90;

   RA = (RA + (Lquadrant - RAquadrant))/15.0;

   double sinDec = 0.39782 * sin(L*_PI180);
   double cosDec = cos(asin(sinDec));

   // calculate the Sun's local hour angle
   double cosH = (coszenith - (sinDec * sinlat)) / (cosDec * coslat);

   // we're in the (ant)arctic and there is no rise(or set) today!
   if(fabs(cosH)>1){ theHour=255; return -1; }
    
   double ha=acos(cosH)*_180PI; 

   if(rs==1) ha=360-ha;

   double UT = ha/15.0 + RA - (0.06571 * t) - 6.622 - lngHour;

   minutes = (int)((UT+tz)*60.0);
   if(minutes<0) minutes+=1440;
   minutes%=1440;
  
   theHour=minutes/60;
   theMinute=minutes-theHour*60;
  
   return minutes;
}

Sunrise sun(55.7,37.6,3);
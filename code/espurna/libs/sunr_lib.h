#ifndef sunrise_h
#define sunrise_h

#define sRISE 1
#define sSET  0


extern "C" {
    #include "../libs/fs_math.h"
    #include "../libs/fs_math1.h"
}


static float zcos[] = {-0.01454,-0.10453,-0.20791,-0.30901};


enum Zenith {official, civil, nautical, astronomical };

class Sunrise {
    public:
      Sunrise(int latitude, int longitude, int timezone)
		{   
		    begin(latitude, longitude, timezone);
		}

		void begin(int latitude, int longitude, int timezone)
		{
		    lat = latitude;
			lon = longitude;
			tzm = timezone;
			sinlat = fs_sin(latitude*_PI18000);
			coslat = fs_cos(latitude*_PI18000);
			lngHour = longitude/1500.0f;
			lngHour24 = lngHour/2400.0f;
			tz=tzm/60.0f;

         passive = false;

			//hr=255;
			//min=0;
		}

        //unsigned char get_hr() { return (unsigned char)hr; }
        //unsigned char get_min(){ return (unsigned char)min; }


        int calc(int year, unsigned char  month, unsigned char  day, Zenith zenith, unsigned char rs)
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

            float t = doy - lngHour24;
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
            if(cosH>1.0f){ /*hr=255;*/ return -1; } // polar night
            if(cosH<(-1.0f)){ /*hr=255;*/ return -2; } // polar day
            //if(fs_abs(cosH)>1.0f){ hr=255; return -1; } // polar night
            
            float ha=fs_acos(cosH)*_180PI; 

            if(rs==1) ha=360-ha;

            float UT = ha/15.0f + RA - (0.06571f * t) - 6.622f - lngHour;
            UT+=tz;
            UT = adjust(UT,24);

            minutes = UT*(60.0f);
        
            //hr=minutes/60;
            //min=(int)(minutes-hr*60);

        // for test
        /*
            static int next = 0;

            if(rs) { //rise
            minutes=(int)(10+14*60)+next;
            next+=10;
            } else { // set
            minutes=(int)(11+14*60)+next;
            next+=10;
            }
        */  
            return minutes;
        }


        int get_lat() {return lat;}
        int get_lon() {return lon;}
        int get_tzm() {return tzm;}

        void setPassive(bool p) { passive = p;}
        bool getPassive() { return passive;}
    private:
        float adjust(float val, float bounds)
        {
            float v = val;
            while(v>bounds) v-=bounds;
            while(v<0) v+=bounds;
            return v;
        }

        int lat,lon,tzm;
        float tz;
        float sinlat,coslat,lngHour,lngHour24, coszenith;
        bool passive; 
        //int  hr,min;
};


//extern Sunrise sun;

#endif
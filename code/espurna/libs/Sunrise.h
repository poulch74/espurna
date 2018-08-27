#ifndef sunrise_h
#define sunrise_h

#define sRISE 1
#define sSET  0

enum Zenith {official, civil, nautical, astronomical };

class Sunrise {
    public:
        Sunrise(int, int, int);
        void begin(int, int, int);
        //unsigned char get_hr() { return (unsigned char)hr; }
        //unsigned char get_min(){ return (unsigned char)min; }

        int calc(int,unsigned char ,unsigned char, Zenith, unsigned char);
        int get_lat() {return lat;}
        int get_lon() {return lon;}
        int get_tzm() {return tzm;}

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
        //int  hr,min;
};


extern Sunrise sun;

#endif
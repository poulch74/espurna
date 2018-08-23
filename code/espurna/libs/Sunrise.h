#ifndef sunrise_h
#define sunrise_h

#define sRISE 1
#define sSET  0

enum Zenith {official, civil, nautical, astronomical };

class Sunrise {
    public:
        Sunrise(float, float, float);
        void begin(float , float , float);
        //unsigned char get_hr() { return (unsigned char)hr; }
        //unsigned char get_min(){ return (unsigned char)min; }

        int calc(int,unsigned char ,unsigned char, Zenith, unsigned char);

    private:
        float adjust(float val, float bounds)
        {
            float v = val;
            while(v>bounds) v-=bounds;
            while(v<0) v+=bounds;
            return v;
        }

        float tz;
        float sinlat,coslat,lngHour,lngHour24, coszenith;
        //int  hr,min;
};


extern Sunrise sun;

#endif
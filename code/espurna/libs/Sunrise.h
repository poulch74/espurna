#ifndef sunrise_h
#define sunrise_h

#define srRISE true
#define srSET  false

enum Zenith {official, civil, nautical, astronomical };

class Sunrise {
    public:
        Sunrise(float, float, float);
        void begin(float , float , float);
        unsigned char get_hr() { return (unsigned char)hr; }
        unsigned char get_min(){ return (unsigned char)min; }

        int calc(int,unsigned char ,unsigned char, Zenith, bool);

    private:
        float adjust(float val, float bounds)
        {
            float v = val;
            while(v>bounds) v-=bounds;
            while(v<0) v+=bounds;
            return v;
        }

        float sinlat,coslat,lngHour,lngHour24, coszenith, tz;
        int  hr,min;
};


extern Sunrise sun;

#endif
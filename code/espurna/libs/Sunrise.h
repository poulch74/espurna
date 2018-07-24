#ifndef sunrise_h
#define sunrise_h

#define srRISE 1
#define srSET  0

class Sunrise{
  public:
	Sunrise(float, float, float);
	void Actual() {coszenith=float(-0.01454); }
	void Civil() { coszenith=float(-0.10453); }
	void Nautical(){ coszenith=float(-0.20791); }
	void Astronomical(){ coszenith=float(-0.30901); }
	unsigned char getHr() { return theHour; }
	unsigned char getMin(){ return theMinute; }
	
   int Calc(int,unsigned char ,unsigned char, int);

  private:
	float adjust(float val, float bounds)
	{
		float v = val;
		while(v>bounds) v-=bounds;
		while(v<0) v+=bounds;
		return v;
	}

	float sinlat,coslat,lngHour, coszenith, tz;
	unsigned char  theHour,theMinute;
};

extern Sunrise sun;

#endif
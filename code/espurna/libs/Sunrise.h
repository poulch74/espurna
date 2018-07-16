#ifndef sunrise_h
#define sunrise_h

#define srRISE 1
#define srSET  0

class Sunrise{
  public:
	Sunrise(double, double, double);
	void Actual() {coszenith=-0.01454; }
	void Civil() { coszenith=-0.10453; }
	void Nautical(){ coszenith=-0.20791; }
	void Astronomical(){ coszenith=-0.30901; }
	unsigned char getHr() { return theHour; }
	unsigned char getMin(){ return theMinute; }
	
   int Calc(int,unsigned char ,unsigned char, int);

  private:
	double adjust(double val, double bounds)
	{
		double v = val;
		while(v>bounds) v-=bounds;
		while(v<0) v+=bounds;
		return v;
	}

	double sinlat,coslat,lngHour, coszenith, tz;
	unsigned char  theHour,theMinute;
};

extern Sunrise sun;

#endif
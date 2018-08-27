#if SENSOR_SUPPORT && SUNRISE_SUPPORT && NTP_SUPPORT

#include "libs/sunrise.h"

extern "C" {
    #include "libs/fs_math.h"
}

#define _PI180 (0.01745329f) // pi/180
#define _180PI (57.2957795f) // 180/pi
#define _PI2   (1.5707963f) // half pi
#define _PI18000 (0.0001745329f) // pi/18000

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


Sunrise::Sunrise(int latitude, int longitude, int timezone)
{   
    begin(latitude, longitude, timezone);
}

void Sunrise::begin(int latitude, int longitude, int timezone)
{
    lat = latitude;
    lon = longitude;
    tzm = timezone;
    sinlat = fs_sin(latitude*_PI18000);
    coslat = fs_cos(latitude*_PI18000);
    lngHour = longitude/1500.0f;
    lngHour24 = lngHour/2400.0f;
    tz=tzm/60.0f;

    //hr=255;
    //min=0;
}

int Sunrise::calc(int year, unsigned char  month, unsigned char  day, Zenith zenith, unsigned char rs)
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
    static int next = 0;

    if(rs) { //rise
    minutes=(int)(10+14*60)+next;
    next+=10;
    } else { // set
    minutes=(int)(11+14*60)+next;
    next+=10;
    }
  
    return minutes;
}

Sunrise sun(5570,3760,180); // Moscow default

extern std::vector<BaseSensor *> _sensors;

bool _sunrise_update = false;
bool _sunrise_configure = false;

void _sunriseConfigure()
{
    _sunrise_configure = false;
    int offset = getSetting("ntpOffset", NTP_TIME_OFFSET).toInt();
    int lat = (int)(100.0*getSetting("ntpLatitude", NTP_LATITUDE).toFloat());
    int lon = (int)(100.0*getSetting("ntpLongitude", NTP_LONGITUDE).toFloat());

    DEBUG_MSG_P(PSTR("[SUNRISE] Time zone : %d\n"), offset);
    DEBUG_MSG_P(PSTR("[SUNRISE] Latitude : %d.%d\n"), lat/100,lat%100);
    DEBUG_MSG_P(PSTR("[SUNRISE] Longitude : %d.%d\n"), lon/100,lon%100);
    if((lat!=sun.get_lat()) || (lon!=sun.get_lon()) || (offset!=sun.get_tzm())) {
        DEBUG_MSG_P(PSTR("[SUNRISE] Recalc forced\n"));
        sun.begin(lat,lon,offset);
    }        

    _sunrise_update = true;
}

void _sunriseUpdate()
{
    if(ntpSyncedEx())
    {
        _sunrise_update = false;

        for (unsigned char i=0; i<_sensors.size(); i++) // and reset sunrise sensors
            if(_sensors[i]->getID() == SENSOR_SUNRISE_ID) _sensors[i]->begin();
    }            
}

void _sunriseLoop() {

    if(_sunrise_configure) _sunriseConfigure();
    if(_sunrise_update) _sunriseUpdate();
}

void sunriseSetup() {

    _sunrise_configure = true;

    #if WEB_SUPPORT
        //wsOnSendRegister(_ntpWebSocketOnSend);
        //wsOnReceiveRegister(_ntpWebSocketOnReceive);
        wsOnAfterParseRegister([]() { _sunrise_configure = true; });
    #endif

    // Register loop
    espurnaRegisterLoop(_sunriseLoop);
}

#endif
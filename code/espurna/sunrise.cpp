#include "sunrise.h"

#if SUNRISE_SUPPORT && NTP_SUPPORT && SENSOR_SUPPORT

#include "libs/sunr_lib.h"

#include "ntp.h"
#include "debug.h"
#include "broker.h"
#include "ws.h"

#include "sensors/BaseSensor.h"

Sunrise sun(5570,3760,180); // Moscow default

extern std::vector<BaseSensor *> _sensors;

bool _sunrise_update = false;
bool _sunrise_configure = false;
bool _sunrise_passive = false;

void _sunriseConfigure()
{
    _sunrise_configure = false;
    int offset = getSetting("ntpOffset", NTP_TIME_OFFSET);
    int lat = /*(int)5570;*/(getSetting("ntpLatitude", NTP_LATITUDE)*100.0f);
    int lon = /*(int)3760;*/(getSetting("ntpLongitude", NTP_LONGITUDE)*100.0f);
    _sunrise_passive = (getSetting("ntpSunPassive", 0) == 1);

    DEBUG_MSG_P(PSTR("[SUNRISE] Time zone : %d\n"), offset);
    DEBUG_MSG_P(PSTR("[SUNRISE] Latitude : %d\n"), lat);
    DEBUG_MSG_P(PSTR("[SUNRISE] Longitude : %d\n"), lon);
    DEBUG_MSG_P(PSTR("[SUNRISE] SunPassive : %d\n"), _sunrise_passive);

    sun.setPassive(_sunrise_passive);

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

        for (unsigned char i=0; i<_sensors.size(); i++)
            if(_sensors[i]->getID() == SENSOR_SUNRISE_ID) _sensors[i]->begin();
    }            
}

void _sunriseLoop()
{

    if(_sunrise_configure) _sunriseConfigure();
    if(_sunrise_update) _sunriseUpdate();
}

void sunriseSetup()
{
    _sunrise_configure = true;
    // Register loop
    espurnaRegisterLoop(_sunriseLoop);
    espurnaRegisterReload([]() { _sunrise_configure = true; });
}

#endif
// -----------------------------------------------------------------------------
// Sunrise Sensor
// Copyright (C) 2018 by Pavel Chauzov <poulch at mail dot ru>
// -----------------------------------------------------------------------------

#if SENSOR_SUPPORT && SUNRISE_SUPPORT

#pragma once


#include "Arduino.h"
#include "BaseSensor.h"
#include <TimeLib.h>
#include <NtpClientLib.h>
#include "../libs/sunrise.h"

extern "C" {
    #include "../libs/fs_math.h"
}

// forward decl
bool ntpSynced();
String ntpDateTime(time_t t);
//String ntpDateTime();
//time_t ntpLocal2UTC(time_t local);
bool relayStatus(unsigned char id, bool status);

class SunriseSensor : public BaseSensor {

    public:

        // ---------------------------------------------------------------------
        // Public
        // ---------------------------------------------------------------------

        SunriseSensor(): BaseSensor() {
            _count = 3;
            _sensor_id = SENSOR_SUNRISE_ID;
            _relayID = 0;            
        }


        // ---------------------------------------------------------------------
        // Sensor API
        // ---------------------------------------------------------------------

        // Initialization method, must be idempotent
        void begin() {
            _ready = true;
            _first = true;
        }

        // Descriptive name of the sensor
        String description() {
            return String("SUNRISE @ DEV");
        }

        // Descriptive name of the slot # index
        String slot(unsigned char index) {
            //return String(names[index]);
            
            if (index == 0) return String("Day(1)/Night(0) ");
            if (index == 1) return String("Sunrise time (BCD) ");
            if (index == 2) return String("Sunset  time (BCD)" );
            return description();
        };

        // Address of the sensor (it could be the GPIO or I2C address)
        String address(unsigned char index) {
            return String("0");
        }

        // Type for slot # index
        unsigned char type(unsigned char index) {
            return MAGNITUDE_ANALOG;
        }

        void setRelayBinding(unsigned char id) {
            _relayID = id;
        }

        void pre()
        {
            _error = SENSOR_ERROR_OUT_OF_RANGE;
            if(ntpSynced()) {
                
                _error = SENSOR_ERROR_OK;
                time_t curt = now();

                if(_first) {
                    _first = false;
                    today = previousMidnight((curt));
                    _calculate_sunrise();
                    if(curt>=rise_time && curt<set_time) state =1;
                    else state = 0;

                    if(_relayID > 0) {
                        int status = _relayMode ? (!state):state;
                        DEBUG_MSG(("[SENSOR] First set relay  : %d\n"), status);
                        relayStatus(_relayID - 1, status);
                    }

                } else {
                
                    int curstate;

                    if((curt>=rise_time) && (curt<set_time)) curstate =1; //day
                    else curstate = 0; //night

                    if(state==1 && curstate==0) { // set occure so night to next day.switch to next today and recalc next rise/set times
                        today = nextMidnight(today);
                        _calculate_sunrise();
                    }

                    if(curstate!=state) {
                        state = curstate;
                        if(_relayID > 0) {
                            int status = _relayMode ? (!state):state;
                            DEBUG_MSG(("[SENSOR] Switch relay  : %d\n"), status);
                            relayStatus(_relayID - 1, status);
                        }
                    }

                }
            }
        }

        // Current value for slot # index
        double value(unsigned char index) {

            _error = SENSOR_ERROR_OUT_OF_RANGE;
            time_t tt = set_time;
            if (ntpSynced() && (index<3)) {
                _error = SENSOR_ERROR_OK;
                if(index == 0) return state;
                if(index == 1) tt = rise_time;
            }
            tmElements_t tm;
            breakTime(tt, tm);
            return tm.Hour*100+tm.Minute; // BCD
            
        }

    protected:
        time_t rise_time;
        time_t set_time;
        time_t today;
        bool _first;
        int state;
        unsigned char _relayID;
        int _relayMode; // 0 off on  night 1  on on night

        void _calculate_sunrise() { // today is midnight today date with cleared time so no checks and clearing unused elements
            tmElements_t tm;
            breakTime(today, tm);
            sun.calc(tm.Year,tm.Month,tm.Day,official,srRISE); // month,date - january=1 ;  t= minutes past midnight of sunrise (6 am would be 360)
            tm.Hour = sun.get_hr();
            tm.Minute=sun.get_min();
            rise_time = makeTime(tm);
            sun.calc(tm.Year,tm.Month,tm.Day,official,srSET); // month,date - january=1 ;  t= minutes past midnight of sunrise (6 am would be 360)
            tm.Hour = sun.get_hr();
            tm.Minute=sun.get_min();
            set_time = makeTime(tm);

            DEBUG_MSG(("[SENSOR] Today Time  : %s\n"), (char *) ntpDateTime(today).c_str());
            DEBUG_MSG(("[SENSOR] Sunrise Time  : %s\n"), (char *) ntpDateTime(rise_time).c_str());
            DEBUG_MSG(("[SENSOR] Sunset Time  : %s\n"), (char *) ntpDateTime(set_time).c_str());
        }

};

#endif // SENSOR_SUPPORT && SUNRISE_SUPPORT

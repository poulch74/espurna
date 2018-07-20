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

bool ntpSynced();
String ntpDateTime(time_t t);
String ntpDateTime();
time_t ntpLocal2UTC(time_t local);
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
            if (index == 0) return String("Day(1)/Night(0) ");
            if (index == 1) return String("Sunrise time (BCD) ");
            if (index == 2) return String("Sunset time (BCD)" );
            return description();
        };

        // Address of the sensor (it could be the GPIO or I2C address)
        String address(unsigned char index) {
            return String("0");
        }

        // Type for slot # index
        unsigned char type(unsigned char index) {
            if (index == 0) return MAGNITUDE_DIGITAL;
            if (index == 1) return MAGNITUDE_ANALOG;
            if (index == 2) return MAGNITUDE_ANALOG;
            return MAGNITUDE_NONE;
        }

        void setRelayBinding(unsigned char id) {
            _relayID = id;
        }

        void pre()
        {
            if(!ntpSynced()) {
                _error = SENSOR_ERROR_OUT_OF_RANGE;
                return;
            }

            time_t curt = now();

            if(_first)
            {
                DEBUG_MSG(("[SENSOR] Sunrise PRE success!! "));

                _first = false;
                today = previousMidnight(curt);
                _calculate_sunrise();
                if(curt>=rise_time && curt<set_time) state =1;
                else state = 0;
                if (_relayID > 0) {
                    int status = _relayMode ? (!state):state;
                    DEBUG_MSG(("[SENSOR] First set relay  : %ds\n"), status);
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
                    if (_relayID > 0) {
                        int status = _relayMode ? (!state):state;
                        DEBUG_MSG(("[SENSOR] Switch relay  : %ds\n"), status);
                        relayStatus(_relayID - 1, status);
                    }
                }
                DEBUG_MSG(("[SENSOR] Sunrise tick check!! "));
            }
            
        }

        // Current value for slot # index
        double value(unsigned char index) {

            if(!ntpSynced()) {
                _error = SENSOR_ERROR_OUT_OF_RANGE;
                return 0;
            }

            DEBUG_MSG(("[SENSOR] Sunrise value check!! "));

            if( index==0 ) {
                return state;
            }

            if(index == 1) {
                tmElements_t tm;
                breakTime(rise_time, tm);
                return tm.Hour*100+tm.Minute;
            }

            if(index == 2) {
                tmElements_t tm;
                breakTime(set_time, tm);
                return tm.Hour*100+tm.Minute;
            }
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
            sun.Calc(tm.Year,tm.Month,tm.Day,srRISE); // month,date - january=1 ;  t= minutes past midnight of sunrise (6 am would be 360)
            tm.Hour = sun.getHr();
            tm.Minute=sun.getMin();
            rise_time = makeTime(tm);
            sun.Calc(tm.Year,tm.Month,tm.Day,srSET); // month,date - january=1 ;  t= minutes past midnight of sunrise (6 am would be 360)
            tm.Hour = sun.getHr();
            tm.Minute=sun.getMin();
            set_time = makeTime(tm);

            DEBUG_MSG(("[SENSOR] Today Time  : %s\n"), (char *) ntpDateTime(today).c_str());
            DEBUG_MSG(("[SENSOR] Sunrise Time  : %s\n"), (char *) ntpDateTime(rise_time).c_str());
            DEBUG_MSG(("[SENSOR] Sunset Time  : %s\n"), (char *) ntpDateTime(set_time).c_str());
        }

};

#endif // SENSOR_SUPPORT && SUNRISE_SUPPORT

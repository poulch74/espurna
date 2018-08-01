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
            _update = true;
            _state = -1;
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

        // Current value for slot # index
        double value(unsigned char index) {

            _error = SENSOR_ERROR_OUT_OF_RANGE;
            time_t tt = _set_time;
            if (ntpSynced() && (index<3)) {
                _error = SENSOR_ERROR_OK;
                if(index == 0) return _state;
                if(index == 1) tt = _rise_time;
            }
            tmElements_t tm;
            breakTime(tt, tm);
            return tm.Hour*100+tm.Minute; // BCD
            
        }

        void pre()
        {
            _error = SENSOR_ERROR_OUT_OF_RANGE;
            while(ntpSynced()) {
                
                _error = SENSOR_ERROR_OK;
                time_t curt = now();

                if(_update) {
                    _update = false;
                    _today = previousMidnight((curt));
                    _calculate_sunrise();
                    DEBUG_MSG(("[SENSOR] update sunrise time \n"));
                }

                if(curt>=nextMidnight(_today)) {_update = true; continue;}

                _check_state(curt);
                break;
            }                
        }



    protected:
        time_t _rise_time;
        time_t _set_time;
        time_t _today;
        int _state;
        int _polar;
        bool _update;
        unsigned char _relayID;
        int _relayMode; // 0 off on  night 1  on on night

        void _calculate_sunrise() { // today is midnight today date with cleared time so no checks and clearing unused elements
            tmElements_t tm;
            breakTime(_today, tm);
            int min = sun.calc(tm.Year,tm.Month,tm.Day,official,srRISE); // month,date - january=1 ;  t= minutes past midnight of sunrise (6 am would be 360)
            if(min>=0) _rise_time = _today+min*60;
            min = sun.calc(tm.Year,tm.Month,tm.Day,official,srSET); // month,date - january=1 ;  t= minutes past midnight of sunrise (6 am would be 360)
            if(min>=0) _set_time = _today+min*60;
            _polar = min;

            DEBUG_MSG(("[SENSOR] Today Time  : %s\n"), (char *) ntpDateTime(_today).c_str());
            DEBUG_MSG(("[SENSOR] Sunrise Time  : %s\n"), (char *) ntpDateTime(_rise_time).c_str());
            DEBUG_MSG(("[SENSOR] Sunset Time  : %s\n"), (char *) ntpDateTime(_set_time).c_str());
        }

        void _check_state(time_t curt) 
        {
            int curstate;
            // -1 night -2 day polar
            if(_polar<0) {
                if(_polar==(-1)) curstate = 0; // night
                else curstate = 1;  //day
            } else {
               if((curt>=_rise_time) && (curt<_set_time)) curstate =1; //day
               else curstate = 0; //night
            }                    

            if(curstate!=_state) { // on first call state=-1, so always init relay if binded
                _state = curstate;
                if(_relayID > 0) {
                    int status = _relayMode ? (!_state):_state;
                    DEBUG_MSG(("[SENSOR] Switch relay  : %d\n"), status);
                    relayStatus(_relayID - 1, status);
                }
            }
        }


};

#endif // SENSOR_SUPPORT && SUNRISE_SUPPORT

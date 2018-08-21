/*

NTP MODULE

Copyright (C) 2016-2018 by Xose Pérez <xose dot perez at gmail dot com>

*/

#if NTP_SUPPORT

#include <TimeLib.h>
#include <NtpClientLib.h>
#include <WiFiClient.h>
#include <Ticker.h>

unsigned long _ntp_start = 0;
bool _ntp_update = false;
bool _ntp_configure = false;

#if RTC_SUPPORT && RTC_NTP_SYNC_ENA
bool _rtc_update = false;
#endif


// -----------------------------------------------------------------------------
// NTP
// -----------------------------------------------------------------------------

#if WEB_SUPPORT

bool _ntpWebSocketOnReceive(const char * key, JsonVariant& value) {
    return (strncmp(key, "ntp", 3) == 0);
}

void _ntpWebSocketOnSend(JsonObject& root) {
    root["ntpVisible"] = 1;
    root["ntpStatus"] = (timeStatus() == timeSet);
    root["ntpServer"] = getSetting("ntpServer", NTP_SERVER);
    root["ntpOffset"] = getSetting("ntpOffset", NTP_TIME_OFFSET).toInt();
    root["ntpDST"] = getSetting("ntpDST", NTP_DAY_LIGHT).toInt() == 1;
    root["ntpRegion"] = getSetting("ntpRegion", NTP_DST_REGION).toInt();

    root["ntpLatitude"] = getSetting("ntpLatitude", NTP_LATITUDE).toFloat();
    root["ntpLongitude"] = getSetting("ntpLongitude", NTP_LONGITUDE).toFloat();

    time_t t = ntpSyncedEx();
    if (t) { root["now"] = t;  }
}

#endif

void _ntpStart() {

    _ntp_start = 0;

    #if RTC_SUPPORT
        NTP.begin(getSetting("ntpServer", NTP_SERVER),DEFAULT_NTP_TIMEZONE,false,0,NULL,true);
        // set alter sync provider. two attempts for NTP synchro at start occure...
        setSyncProvider(ntp_getTime);
    #else
        NTP.begin(getSetting("ntpServer", NTP_SERVER));
    #endif

    NTP.setInterval(NTP_SYNC_INTERVAL, NTP_UPDATE_INTERVAL);
    NTP.setNTPTimeout(NTP_TIMEOUT);
    //_ntpConfigure();
    _ntp_configure = true;
}

void _ntpConfigure() {

    _ntp_configure = false;

    int offset = getSetting("ntpOffset", NTP_TIME_OFFSET).toInt();
    int sign = offset > 0 ? 1 : -1;
    offset = abs(offset);
    int tz_hours = sign * (offset / 60);
    int tz_minutes = sign * (offset % 60);
    if (NTP.getTimeZone() != tz_hours || NTP.getTimeZoneMinutes() != tz_minutes) {
        NTP.setTimeZone(tz_hours, tz_minutes);
        _ntp_update = true;
    }

    bool daylight = getSetting("ntpDST", NTP_DAY_LIGHT).toInt() == 1;
    if (NTP.getDayLight() != daylight) {
        NTP.setDayLight(daylight);
        _ntp_update = true;
    }

    String server = getSetting("ntpServer", NTP_SERVER);
    if (!NTP.getNtpServerName().equals(server)) {
        NTP.setNtpServerName(server);
    }

    uint8_t dst_region = getSetting("ntpRegion", NTP_DST_REGION).toInt();
    NTP.setDSTZone(dst_region);

    // update sunrise provider
    #if SENSOR_SUPPORT && SUNRISE_SUPPORT
        if(ntpSyncedEx()) {
             SunProviderInit();
        }
    #endif        
}

void _ntpUpdate(time_t t) {

    _ntp_update = false;

    #if WEB_SUPPORT
        wsSend(_ntpWebSocketOnSend);
    #endif

    if (t) {
        #if RTC_SUPPORT && RTC_NTP_SYNC_ENA
            // sync/update rtc here!!!!!!!!!!!!
            if(_rtc_update) setTime_rtc(t);
        #endif

        DEBUG_MSG_P(PSTR("[NTP] UTC Time  : %s\n"), (char *) ntpDateTime(ntpLocal2UTC(t)).c_str());
        DEBUG_MSG_P(PSTR("[NTP] Local Time: %s\n"), (char *) ntpDateTime(t).c_str());
    }

}

void _ntpLoop() {

    if (0 < _ntp_start && _ntp_start < millis()) _ntpStart();
    if (_ntp_configure) _ntpConfigure();

    time_t t = ntpSyncedEx();
    if (_ntp_update) _ntpUpdate(t);

    #if BROKER_SUPPORT            
        static unsigned char last_minute = 60;
        if(t)
        {
            tmElements_t tm;
            breakTime(t,tm);
            if (tm.Minute != last_minute) {
                last_minute = tm.Minute;
                brokerPublish(MQTT_TOPIC_DATETIME, ntpDateTime(t).c_str());
            }
        }
    #endif                    
}

void _ntpBackwards() {
    moveSetting("ntpServer1", "ntpServer");
    delSetting("ntpServer2");
    delSetting("ntpServer3");
    int offset = getSetting("ntpOffset", NTP_TIME_OFFSET).toInt();
    if (-30 < offset && offset < 30) {
        offset *= 60;
        setSetting("ntpOffset", offset);
    }
}

// -----------------------------------------------------------------------------

time_t ntpSyncedEx() {
    time_t t = now();
    return (t>1483228800) ? t : 0; // 01.01.2017
}

bool ntpSynced() {
    return (year() > 2017);
}

String ntpDateTime(time_t t) {
    char buffer[20];
    tmElements_t tm;
    breakTime(t,tm);
    snprintf_P(buffer, sizeof(buffer),
        PSTR("%04d-%02d-%02d %02d:%02d:%02d"),
        tmYearToCalendar(tm.Year),tm.Month,tm.Day,tm.Hour,tm.Minute,tm.Second
    );
    return String(buffer);
}

String ntpDateTime() {
    time_t t = ntpSyncedEx();
    if (t) return ntpDateTime(t);
    return String();
}

time_t ntpLocal2UTC(time_t local) {
    int offset = getSetting("ntpOffset", NTP_TIME_OFFSET).toInt();
    if (NTP.isSummerTime()) offset += 60;
    return local - offset * 60;
}

// -----------------------------------------------------------------------------

void ntpSetup() {

    _ntpBackwards();

    NTP.onNTPSyncEvent([](NTPSyncEvent_t error) {
        if (error) {
            #if WEB_SUPPORT
                wsSend_P(PSTR("{\"ntpStatus\": false}"));
            #endif
            if (error == noResponse) {
                DEBUG_MSG_P(PSTR("[NTP] Error: NTP server not reachable\n"));
            } else if (error == invalidAddress) {
                DEBUG_MSG_P(PSTR("[NTP] Error: Invalid NTP server address\n"));
            }
            _ntp_update = false;
        } else {
            _ntp_update = true;
            #if RTC_SUPPORT && RTC_NTP_SYNC_ENA
                _rtc_update = true;
            #endif

        }
    });

    wifiRegister([](justwifi_messages_t code, char * parameter) {
        if (code == MESSAGE_CONNECTED) _ntp_start = millis() + NTP_START_DELAY;
        #if RTC_SUPPORT
          // system time from local RTC, but still try recovery if enabled (without success)
        else 
            if(code == MESSAGE_ACCESSPOINT_CREATED) _ntp_start = millis() + NTP_START_DELAY;
        #endif                

    });

    #if WEB_SUPPORT
        wsOnSendRegister(_ntpWebSocketOnSend);
        wsOnReceiveRegister(_ntpWebSocketOnReceive);
        wsOnAfterParseRegister([]() { _ntp_configure = true; });
    #endif

    // Register loop
    espurnaRegisterLoop(_ntpLoop);
    
    #if RTC_SUPPORT && TERMINAL_SUPPORT
        _rtcInitCommands();
    #endif        

}

#endif // NTP_SUPPORT

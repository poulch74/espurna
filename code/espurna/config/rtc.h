/*

RTC support module

Copyright (C) 2018 by Pavel Chauzov <poulch at mail dot ru>

*/

//#define RTC_SUPPORT             1               // enable battery backed RTC for ntp/
//#define RTC_PROVIDER            RTC_PROVIDER_DS3231 //RTC_DUMMY
//#define RTC_NTP_SYNC_ENA	0               // enable sync RTC on NTP sync success

// -----------------------------------------------------------------------------
// RTC3231_SUPPORT
// -----------------------------------------------------------------------------


#ifndef RTC_SUPPORT
#define RTC_SUPPORT              1               // enable battery backed RTC for ntp
#define RTC_PROVIDER             RTC_PROVIDER_DS3231 //RTC_DUMMY
#endif

#ifndef RTC_NTP_SYNC_ENA
#define RTC_NTP_SYNC_ENA         1               // enable sync RTC on NTP sync success
#endif


#if RTC_SUPPORT

#if RTC_PROVIDER == RTC_PROVIDER_DS3231
    #include "../rtc/ds3231.h"
#elif RTC_PROVIDER == RTC_PROVIDER_DS1307
    #include "../rtc/ds1307.h"
#else
    #include "../rtc/dummy.h"
#endif

#endif

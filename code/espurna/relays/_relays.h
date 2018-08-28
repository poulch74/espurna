/* to add new relay or provider
    0 - add your.h file with callback function
    1 - add your.h in #include below,
    2 - add below in InitRelay(N) proper callback setting
        or in InitProviderCb() if new provider added
    3 - add needed definitions in defaults.h, types.h etc...
*/    

#include "dummy.h"
#include "hbridge.h"
#include "latched.h"
#include "light.h"
#include "normal.h"
#include "rfbridge.h"
#include "software.h"
#include "sonoffdual.h"
#include "stmrelay.h"
#include "btnrst.h"


#define InitProviderCb() \
    do \
    {\
        if(RELAY_PROVIDER == RELAY_PROVIDER_RFBRIDGE) \
            _relaycb=_relayRFBridgeCb; \
        else if(RELAY_PROVIDER == RELAY_PROVIDER_STM) \
            _relaycb=_relay_STM_Cb; \
        else if(RELAY_PROVIDER == RELAY_PROVIDER_LIGHT) \
            _relaycb=_relayLightCb; \
        else if(RELAY_PROVIFER == RELAY_PROVIDER_DUAL) \
            _relaycb=_relayDualCb; \
    } while(0)

#define InitRelay(N) \
    do \
    { \
        if((RELAY##N##_TYPE == RELAY_TYPE_NORMAL) || (RELAY##N##_TYPE == RELAY_TYPE_INVERSE)) \
            _relaycb = _relayNormalCb; \
        else if((RELAY##N##_TYPE == RELAY_TYPE_LATCHED) || (RELAY##N##_TYPE == RELAY_TYPE_LATCHED_INVERSE)) \
            _relaycb = _relayLatchedCb; \
        else if(RELAY##N##_TYPE == RELAY_TYPE_HBRIDGE) \
            _relaycb = _relayHBridgeCb; \
        else if(RELAY##N##_TYPE == RELAY_TYPE_SOFTWARE) \
            _relaycb = _relaySoftwareCb; \
        else if(RELAY##N##_TYPE == RELAY_TYPE_BTNRESET) \
            _relaycb = _relayButtonRstCb; \
        \
        _relays.push_back((relay_t) { _relaycb, RELAY##N##_PIN, RELAY##N##_PIN2, RELAY##N##_PINP, RELAY##N##_BUTTON, \
                                                    RELAY##N##_TYPE, RELAY##N##_RESET_PIN, RELAY##N##_DELAY_ON, \
                                                    RELAY##N##_DELAY_OFF, RELAY##N##_HBPULSE }); \
    } while(0)

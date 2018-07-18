#pragma once

// -----------------------------------------------------------------------------
// Debug
// -----------------------------------------------------------------------------

#define DEBUG_SUPPORT           DEBUG_SERIAL_SUPPORT || DEBUG_UDP_SUPPORT || DEBUG_TELNET_SUPPORT || DEBUG_WEB_SUPPORT

#if DEBUG_SUPPORT
    #define DEBUG_MSG(...) debugSend(__VA_ARGS__)
    #define DEBUG_MSG_P(...) debugSend_P(__VA_ARGS__)
    #define DEBUG_MSG_APPEND_ALLOC(name,cnt,len) int name##_l = len; char *ptr_##name; char name[cnt*len];
    #define DEBUG_MSG_APPEND_INIT_P(name) ptr_##name = name; debugAppend_P(&ptr_##name,name##_l,true,"");
    #define DEBUG_MSG_APPEND_INIT(name) ptr_##name = name; debugAppend(&ptr_##name,name##_l,true,"");
    #define DEBUG_MSG_APPEND_P(name,...) debugAppend_P(&ptr_##name,name##_l,false,__VA_ARGS__)
    #define DEBUG_MSG_APPEND(name,...) debugAppend(&ptr_##name,name##_l,false,__VA_ARGS__)

#endif

#ifndef DEBUG_MSG
    #define DEBUG_MSG(...)
    #define DEBUG_MSG_P(...)
    #define DEBUG_MSG_APPEND_ALLOC(name,cnt,len)
    #define DEBUG_MSG_APPEND_INIT_P(name)
    #define DEBUG_MSG_APPEND_INIT(name)
    #define DEBUG_MSG_APPEND_P(name,...)
    #define DEBUG_MSG_APPEND(name,...)
#endif

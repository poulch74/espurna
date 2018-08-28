#if RELAY_PROVIDER == RELAY_PROVIDER_RFBRIDGE
void _relayRFBridgeCb(unsigned char id, bool status) {
    _relays[id].current_status = status;    
    rfbStatus(id, status);
}
#endif

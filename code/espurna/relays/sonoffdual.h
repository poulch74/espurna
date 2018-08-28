#if RELAY_PROVIDER == RELAY_PROVIDER_DUAL
void _relayDualCb(unsigned char id, bool status) {
    // Calculate mask
    _relays[id].current_status = status;    
    unsigned char mask=0;
    for (unsigned char i=0; i<_relays.size(); i++) {
        if (_relays[i].current_status) mask = mask + (1 << i);
    }

    // Send it to F330
    Serial.flush();
    Serial.write(0xA0);
    Serial.write(0x04);
    Serial.write(mask);
    Serial.write(0xA1);
    Serial.flush();
}
#endif

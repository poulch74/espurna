void _relayLatchedCb(unsigned char id, bool status) {
    _relays[id].current_status = status;    
    bool pulse = (_relays[id].type == RELAY_TYPE_LATCHED) ? HIGH : LOW;
    digitalWrite(_relays[id].pin, !pulse);
    digitalWrite(_relays[id].reset_pin, !pulse);
    if (status) {
        digitalWrite(_relays[id].pin, pulse);
    } else {
        digitalWrite(_relays[id].reset_pin, pulse);
    }
    nice_delay(RELAY_LATCHING_PULSE);
    digitalWrite(_relays[id].pin, !pulse);
    digitalWrite(_relays[id].reset_pin, !pulse);
}

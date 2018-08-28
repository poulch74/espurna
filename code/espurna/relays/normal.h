// callbacks
void _relayNormalCb(unsigned char id, bool status) {
    _relays[id].current_status = status;    
    bool _status = (_relays[id].type == RELAY_TYPE_NORMAL) ? status : !status;
    digitalWrite(_relays[id].pin, _status);
}

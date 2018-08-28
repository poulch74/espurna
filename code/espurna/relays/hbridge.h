// callbacks for H-Brige (TB6612) controlled relay/valve
// with pulse_ms set 0 acts like simple relay
// with pulse_ms set for example 5s - drive motor for 5s
// with autostop function enabled real time for valve is about 3s,
// so 5s acts like timeout

void _valveStop(unsigned char id) {
    if (id >= _relays.size()) return;
    digitalWrite(_relays[id].pin, LOW);
    digitalWrite(_relays[id].pin2, LOW);
}

void _relayHBridgeCb(unsigned char id, bool status) {
    _relays[id].current_status = status;    
    if(_relays[id].pulse_ms == 0) { // simple relay

        digitalWrite(_relays[id].pin, status);
        digitalWrite(_relays[id].pin2, LOW);

    } else { // valve control

        if(status) {
            digitalWrite(_relays[id].pin,  HIGH);
            digitalWrite(_relays[id].pin2, LOW);
        } else {
            digitalWrite(_relays[id].pin,  LOW);
            digitalWrite(_relays[id].pin2, HIGH);
        }

        if(_relays[id].pinP != GPIO_NONE) {
            digitalWrite(_relays[id].pinP, HIGH);
            nice_delay(_relays[id].pulse_hb); // to start autostop
            digitalWrite(_relays[id].pinP, LOW);
        }
        
        _relays[id].postTicker.once_ms(_relays[id].pulse_ms,_valveStop,id);
    }
}

// reset binded button  with once mode set

void _relayButtonRstCb(unsigned char id, bool status) {
    _relays[id].current_status = status;
    unsigned char bid = _relays[id].btnID;
    if((bid>0) && (bid <= _buttons.size())) {
        _buttons[bid-1].button->reset_once();
    }
}


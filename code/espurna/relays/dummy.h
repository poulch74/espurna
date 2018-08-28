void _relayDummyCb(unsigned char id, bool status) {
    _relays[id].current_status = status;
    // nothing to do
}

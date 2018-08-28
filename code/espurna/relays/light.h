#if RELAY_PROVIDER == RELAY_PROVIDER_LIGHT
void _relayLightCb(unsigned char id, bool status) {
    // If the number of relays matches the number of light channels
    // assume each relay controls one channel.
    // If the number of relays is the number of channels plus 1
    // assume the first one controls all the channels and
    // the rest one channel each.
    // Otherwise every relay controls all channels.
    // TODO: this won't work with a mixed of dummy and real relays
    // but this option is not allowed atm (YANGNI)

    _relays[id].current_status = status;
    if (_relays.size() == lightChannels()) {
        lightState(id, status);
        lightState(true);
    } else if (_relays.size() == lightChannels() + 1) {
        if (id == 0) {
            lightState(status);
        } else {
            lightState(id-1, status);
        }
    } else {
        lightState(status);
    }

    lightUpdate(true, true);
}
#endif

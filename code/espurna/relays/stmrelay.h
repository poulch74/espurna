#if RELAY_PROVIDER == RELAY_PROVIDER_STM
void _relay_STM_Cb(unsigned char id, bool status) {
    _relays[id].current_status = status;    
    Serial.flush();
    Serial.write(0xA0);
    Serial.write(id + 1);
    Serial.write(status);
    Serial.write(0xA1 + status + id);
    Serial.flush();
}
#endif

// for user defined software or else pseudo relay
void _userFunction1(unsigned char id, bool status)
{
}

void _relaySoftwareCb(unsigned char id, bool status) {
    _relays[id].current_status = status;
    switch(id) {
      case 0: _userFunction1(id,status); break;
      case 1: break;
      case 2: break;
      case 3: break;
      case 4: break;
      case 5: break;
      case 6: break;
      case 7: break;
   }
    // some code
}


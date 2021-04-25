#ifndef Commands_h
#define Commands_h

#include "arduino.h"

void handleCommand(String c);

namespace cmd{
  void help();
  void readstr(String c);
  //void writestr();
}

#endif

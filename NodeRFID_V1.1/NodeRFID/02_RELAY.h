#ifndef _RELAY_H
#define _RELAY_H

#include <Arduino.h>
#define K1 17

class RELAY {
public:
  void khoitao();
  void dieukhienK1(bool status);
};
#endif
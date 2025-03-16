#include "02_RELAY.h"

void RELAY::khoitao() {
  pinMode(K1, OUTPUT);
  digitalWrite(K1, 0);
  pinMode(25, OUTPUT);
  digitalWrite(25, 1); 
}
void RELAY::dieukhienK1(bool status) {
  digitalWrite(K1, status);
}


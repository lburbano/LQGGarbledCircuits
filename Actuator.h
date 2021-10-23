#ifndef ACTUATOR_H
#define ACTUATOR_H
#include "fixedPoint.h"

class Actuator {
public:
  fixedPoint **uk;
  int sizeuk[2];

  Actuator() {}

  void getValue(fixedPoint **uk, int *sizeuk) {
    this->sizeuk[0] = sizeuk[0];
    this->sizeuk[1] = sizeuk[1];
    this->uk = new fixedPoint *[this->sizeuk[0]];
    for (int i = 0; i < this->sizeuk[0]; i++) {
      this->uk[i] = new fixedPoint[this->sizeuk[1]];
    }

    for (int i = 0; i < this->sizeuk[0]; i++) {
      for (int j = 0; j < this->sizeuk[1]; j++) {
        this->uk[i][j] = uk[i][j];
      }
    }
  }
};

#endif

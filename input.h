#ifndef INPUT_H
#define INPUT_H

#include "raylib.h"

struct InputState
{
  Vector2 mouseDelta;
  bool shoot;
  bool modeSwitch;

  void gatherInput();
};

#endif

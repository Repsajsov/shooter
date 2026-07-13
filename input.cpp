#include "input.h"

void InputState::gatherInput()
{
  mouseDelta = GetMouseDelta();
  shoot = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

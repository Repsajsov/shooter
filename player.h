#ifndef PLAYER_H
#define PLAYER_H

#include "input.h"
#include "raylib.h"

class Player
{
private:
  Camera camera;
  float yaw = 0.0f;
  float pitch = 0.0f;

public:
  Player();
  void update(const InputState& input);
  void draw() const;
  Ray getShootRay() const;
  const Camera& getCamera() const;
};

#endif

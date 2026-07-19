#ifndef ROUTINE_H
#define ROUTINE_H

#include <initializer_list>
#include <vector>

#include "raylib.h"
#include "raymath.h"


enum class BehaviourType
{
  STATIC,
  LINEAR,
  RANDOM_LINEAR
};

struct Behaviour
{
  BehaviourType type;
  float duration;
  Vector3 direction;
  float speed;

  static Behaviour STATIC(float duration);
  static Behaviour LINEAR(float duration, Vector3 direction, float speed);
};

class Routine
{

private:
  std::vector<Behaviour> steps;
  int currentIndex;
  float timeInStep;

public:
  Routine();
  Routine(std::initializer_list<Behaviour> steps);
  void update(float dt);
  Vector3 computePosition(Vector3 position, float dt) const;
};

#endif

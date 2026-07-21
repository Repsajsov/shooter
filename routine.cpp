#include "routine.h"


Behaviour Behaviour::STATIC(float duration)
{
  return Behaviour{BehaviourType::STATIC, duration, Vector3{0.0f, 0.0f, 0.0f},
                   0.0f};
}

Behaviour Behaviour::LINEAR(float duration, Vector3 direction, float speed)
{
  return Behaviour{BehaviourType::LINEAR, duration, direction, speed};
}

Routine::Routine() : steps(), currentIndex(0), timeInStep(0.0f)
{
}

Routine::Routine(std::vector<Behaviour> steps)
    : steps(std::move(steps)), currentIndex(0), timeInStep(0.0f)
{
}

void Routine::update(float dt)
{
  if (steps.empty()) return;
  timeInStep += dt;
  if (timeInStep >= steps[currentIndex].duration)
  {
    timeInStep = 0.0f;
    currentIndex = (currentIndex + 1) % steps.size();
  }
}

Vector3 Routine::computePosition(Vector3 position, float dt) const
{
  if (steps.empty()) return position;
  const Behaviour& step = steps[currentIndex];
  switch (step.type)
  {
  case BehaviourType::STATIC:
    return position;
  case BehaviourType::LINEAR:
    return Vector3Add(position, Vector3Scale(step.direction, step.speed * dt));
  case BehaviourType::RANDOM_LINEAR:
    return position;
  }
  return position;
}

void Routine::reflectCurrentDirection(Vector3 normal)
{
  if (steps.empty()) return;
  Behaviour& step = steps[currentIndex];
  step.direction = reflect(step.direction, normal);
}

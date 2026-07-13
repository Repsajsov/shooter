#include "target.h"


Routine::Routine() : steps(), currentIndex(0), timeInStep(0.0f)

{
}

Routine::Routine(std::initializer_list<Behaviour> steps)
    : steps(steps), currentIndex(0), timeInStep(0.0f)
{
}

void Routine::update(float dt)
{
  if (steps.empty()) return;
  timeInStep += dt;
  if (timeInStep >= steps[currentIndex].duration)
  {
    timeInStep = 0;
    currentIndex = (currentIndex + 1) % steps.size();
  }
}

Vector3 Routine::computePosition(Vector3 basePosition) const
{
  if (steps.empty()) return basePosition;
  const Behaviour& step = steps[currentIndex];
  switch (step.type)
  {
  case BehaviourType::STATIC:
    return basePosition;
  case BehaviourType::LINEAR:
    return Vector3Add(basePosition,
                      Vector3Scale(step.direction, timeInStep * step.speed));
  }
  return basePosition;
}

Target::Target(Vector3 position, Vector3 radii, Color color, int health)
    : basePosition(position), position(position), radii(radii), color(color),
      health(health), maxHealth(health)
{
}

Target::Target(Vector3 position, float radii, Color color, int health)
    : basePosition(position), position(position),
      radii((Vector3){radii, radii, radii}), color(color), health(health),
      maxHealth(health)
{
}

RayCollision Target::getCollision(const Ray& ray) const
{
  Ray localRay;
  localRay.position = {(ray.position.x - position.x) / radii.x,
                       (ray.position.y - position.y) / radii.y,
                       (ray.position.z - position.z) / radii.z};

  localRay.direction = Vector3Normalize((Vector3){ray.direction.x / radii.x,
                                                  ray.direction.y / radii.y,
                                                  ray.direction.z / radii.z});
  RayCollision collision =
      GetRayCollisionSphere(localRay, Vector3{0, 0, 0}, 1.0f);

  if (collision.hit)
  {
    collision.point = {collision.point.x * radii.x + position.x,
                       collision.point.y * radii.y + position.y,
                       collision.point.z * radii.z + position.z};
  }
  return collision;
}

void Target::draw() const
{
  Color displayColor = ColorLerp(GRAY, color, (float)health / (float)maxHealth);
  rlPushMatrix();
  rlTranslatef(position.x, position.y, position.z);
  rlScalef(radii.x, radii.y, radii.z);
  DrawSphere(Vector3{0, 0, 0}, 1.0f, displayColor);
  rlPopMatrix();
}

void Target::takeDamage(int amount)
{
  health -= amount;
}

bool Target::isDead() const
{
  return health <= 0;
}

void Target::update(float dt)
{
  routine.update(dt);
  position = routine.computePosition(basePosition);
}

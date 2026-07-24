#include "target.h"

Target::Target(Vector3 position, float radius, Color color, int health,
               Routine routine, bool respawns, PositionMode positionMode)
    : basePosition(position), position(position), radius(radius), color(color),
      baseHealth(health), health(health), maxHealth(health), routine(routine),
      respawns(respawns), positionMode(positionMode)
{
}

RayCollision Target::getCollision(const Ray& ray) const
{
  return GetRayCollisionSphere(ray, position, radius);
}

void Target::draw() const
{
  Color displayColor = ColorLerp(GRAY, color, (float)health / (float)maxHealth);
  DrawSphere(position, radius, displayColor);
}

void Target::takeDamage(int amount)
{
  health -= amount;
}

bool Target::isDead() const
{
  return health <= 0;
}

bool Target::shouldRespawn() const
{
  return respawns;
}

void Target::update(float dt, const std::vector<Plane>& bounds)
{
  routine.update(dt);
  position = routine.computePosition(position, dt);
  for (const Plane& plane : bounds)
  {
    if (isOutsideBounds(position, radius, plane))
      routine.reflectCurrentDirection(plane.normal);
  }
}

Vector3 Target::getPosition() const
{
  return position;
}


void Target::respawn(Vector3 newPosition)
{
  health = baseHealth;
  position = newPosition;
  basePosition = newPosition;
  routine.reset();
}

PositionMode Target::getPositionMode() const
{
  return positionMode;
}

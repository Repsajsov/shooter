#ifndef TARGET_H
#define TARGET_H

#include "raylib.h"
#include "raymath.h"
#include "routine.h"

enum class PositionMode
{
  FIXED,
  RANDOM
};

class Target
{
private:
  Vector3 basePosition;
  Vector3 position;
  float radius;
  Color color;
  int baseHealth;
  int health;
  int maxHealth;
  Routine routine;
  bool respawns;
  PositionMode positionMode;

public:
  Target(Vector3 position, float radius, Color color, int health,
         Routine routine, bool respawns, PositionMode positionMode);
  RayCollision getCollision(const Ray& ray) const;
  void draw() const;
  void takeDamage(int amount);
  bool isDead() const;
  void update(float dt, const std::vector<Plane>& bounds);
  Vector3 getPosition() const;
  bool shouldRespawn() const;
  void reset(Vector3 newPosition);
  PositionMode getPositionMode() const;
};

#endif

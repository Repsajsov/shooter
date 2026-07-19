#ifndef TARGET_H
#define TARGET_H


#include "raylib.h"
#include "raymath.h"
#include "routine.h"

class Target
{
private:
  Vector3 basePosition;
  Vector3 position;
  float radius;
  Color color;
  int health;
  int maxHealth;
  Routine routine;

public:
  Target(Vector3 position, float radius, Color color, int health,
         Routine routine);
  RayCollision getCollision(const Ray& ray) const;
  void draw() const;
  void takeDamage(int amount);
  bool isDead() const;
  void update(float dt, const std::vector<Plane>& bounds);
};

#endif

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"


class Target
{
private:
  Vector3 position;
  Vector3 radii;
  Color color;
  int health;
  int maxHealth;

public:
  Target(Vector3 position, Vector3 radii, Color color, int health);
  Target(Vector3 position, float radius, Color color, int health);
  RayCollision getCollision(const Ray& ray) const;
  void draw() const;
  void takeDamage(int amount);
  bool isDead() const;
};

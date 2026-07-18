#ifndef TARGET_H
#define TARGET_H

#include <initializer_list>
#include <vector>

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

enum class BehaviourType
{
  STATIC,
  LINEAR
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
  Vector3 computePosition(Vector3 basePosition) const;
};


class Target
{
private:
  Vector3 basePosition;
  Vector3 position;
  Vector3 radii;
  Color color;
  int health;
  int maxHealth;
  Routine routine;

public:
  Target(Vector3 position, Vector3 radii, Color color, int health,
         Routine routine);
  Target(Vector3 position, float radius, Color color, int health,
         Routine routine);
  RayCollision getCollision(const Ray& ray) const;
  void draw() const;
  void takeDamage(int amount);
  bool isDead() const;
  void update(float dt);
};

#endif

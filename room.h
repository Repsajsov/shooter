#ifndef ROOM_H
#define ROOM_H

#include <vector>

#include "bounds.h"
#include "raylib.h"
#include "target.h"

struct ShotResult
{
  bool anyTargetInScene;
  float distanceFromCenter;
  float angleFromCenter;
  bool hit;
};

class Room
{
private:
  Vector3 min;
  Vector3 max;
  std::vector<Plane> bounds;
  std::vector<Target> targets;

public:
  Room(Vector3 min, Vector3 max, std::vector<Plane> bounds,
       std::vector<Target> targets);

  void update(float dt);
  void draw() const;
  ShotResult processShot(const Ray& ray, int damage);
};

#endif

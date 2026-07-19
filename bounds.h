#ifndef BOUNDS_H
#define BOUNDS_H

#include "raylib.h"
#include "raymath.h"

struct Plane
{
  Vector3 point;
  Vector3 normal;
};

Vector3 reflect(Vector3 direction, Vector3 normal);
bool isOutsideBounds(Vector3 position, float radius, const Plane& plane);

#endif

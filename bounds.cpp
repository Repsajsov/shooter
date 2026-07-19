#include "bounds.h"


Vector3 reflect(Vector3 direction, Vector3 normal)
{
  float d = Vector3DotProduct(direction, normal);
  return Vector3Subtract(direction, Vector3Scale(normal, 2.0f * d));
}

bool isOutsideBounds(Vector3 position, float radius, const Plane& plane)
{
  float signedDistance =
      Vector3DotProduct(Vector3Subtract(position, plane.point), plane.normal);
  return signedDistance < radius;
}

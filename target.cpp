#include "target.h"


Target::Target(Vector3 position, Vector3 radii, Color color, int health)
    : position(position), radii(radii), color(color), health(health),
      maxHealth(health)
{
}

Target::Target(Vector3 position, float radii, Color color, int health)
    : position(position), radii((Vector3){radii, radii, radii}), color(color),
      health(health), maxHealth(health)
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

#include "room.h"

#include <algorithm>
#include <cmath>

#include "randomizer.h"
#include "raymath.h"

Room::Room(Vector3 min, Vector3 max, std::vector<Plane> bounds,
           std::vector<Target> targets)
    : min(min), max(max), bounds(std::move(bounds)), targets(std::move(targets))
{
}

namespace
{
float distancePointToRay(Vector3 point, const Ray& ray)
{
  Vector3 toPoint = Vector3Subtract(point, ray.position);
  float projectionLength = Vector3DotProduct(toPoint, ray.direction);
  Vector3 closestPointOnRay =
      Vector3Add(ray.position, Vector3Scale(ray.direction, projectionLength));
  return Vector3Distance(point, closestPointOnRay);
}

float getAngleFromCenter(Vector3 targetPosition, const Ray& ray)
{
  Vector3 toTarget =
      Vector3Normalize(Vector3Subtract(targetPosition, ray.position));
  float dot = Vector3DotProduct(ray.direction, toTarget);
  dot = std::clamp(dot, -1.0f, 1.0f);
  return acosf(dot) * RAD2DEG;
}
} // namespace

void Room::update(float dt)
{
  for (auto& t : targets) t.update(dt, bounds);
}

void Room::draw() const
{
  Vector3 center = Vector3Scale(Vector3Add(min, max), 0.5f);
  Vector3 size = Vector3Subtract(max, min);
  DrawCubeWiresV(center, size, LIGHTGRAY);

  for (const auto& t : targets) t.draw();
}

ShotResult Room::processShot(const Ray& ray, int damage)
{
  if (targets.empty()) return ShotResult{false, 0.0f, 0.0f, false};

  int closestIndex = 0;
  float closestDistance = distancePointToRay(targets[0].getPosition(), ray);
  for (int i = 1; i < (int)targets.size(); i++)
  {
    float d = distancePointToRay(targets[i].getPosition(), ray);
    if (d < closestDistance)
    {
      closestDistance = d;
      closestIndex = i;
    }
  }

  float angle = getAngleFromCenter(targets[closestIndex].getPosition(), ray);
  bool hit = targets[closestIndex].getCollision(ray).hit;

  ShotResult result{true, closestDistance, angle, hit};

  if (!hit) return result;

  targets[closestIndex].takeDamage(damage);
  if (targets[closestIndex].isDead())
  {
    if (targets[closestIndex].shouldRespawn())
    {
      Vector3 newPosition =
          (targets[closestIndex].getPositionMode() == PositionMode::RANDOM)
              ? GetRandomPosition(min, max)
              : targets[closestIndex].getPosition();
      targets[closestIndex].reset(newPosition);
    }
    else
    {
      targets[closestIndex] = targets.back();
      targets.pop_back();
    }
  }

  return result;
}

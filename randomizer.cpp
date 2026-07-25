#include "randomizer.h"

#include "config.h"
#include "raymath.h"

Vector3 GetRandomPosition(Vector3 min, Vector3 max)
{
  float x =
      min.x + ((float)GetRandomValue(0, SAMPLE_SIZE) / (float)SAMPLE_SIZE) *
                  (max.x - min.x);
  float y =
      min.y + ((float)GetRandomValue(0, SAMPLE_SIZE) / (float)SAMPLE_SIZE) *
                  (max.y - min.y);
  float z =
      min.z + ((float)GetRandomValue(0, SAMPLE_SIZE) / (float)SAMPLE_SIZE) *
                  (max.z - min.z);
  return Vector3{x, y, z};
}

Vector3 GetRandomDirection()
{
  float x = (float)GetRandomValue(-1 * SAMPLE_SIZE, SAMPLE_SIZE);
  float y = (float)GetRandomValue(-1 * SAMPLE_SIZE, SAMPLE_SIZE);
  float z = (float)GetRandomValue(-1 * SAMPLE_SIZE, SAMPLE_SIZE);
  return Vector3Normalize(Vector3{x, y, z});
}

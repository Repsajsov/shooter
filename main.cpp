#include <algorithm>
#include <iostream>
#include <vector>

#include "config.h"
#include "input.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "target.h"


void updateLook(Camera& camera, InputState& input, float& yaw, float& pitch)
{
  float mouseSensitivity = 0.003f;

  yaw -= input.mouseDelta.x * mouseSensitivity;
  pitch -= input.mouseDelta.y * mouseSensitivity;

  if (pitch > 1.5f) pitch = 1.5f;
  if (pitch < -1.5f) pitch = -1.5f;

  Vector3 forward;
  forward.x = cosf(pitch) * sinf(yaw);
  forward.y = sinf(pitch);
  forward.z = cosf(pitch) * cosf(yaw);

  camera.target.x = camera.position.x + forward.x;
  camera.target.y = camera.position.y + forward.y;
  camera.target.z = camera.position.z + forward.z;
}


struct HitResult
{
  int index;
  float distance;
};

std::vector<HitResult> getSortedHits(const Ray& ray,
                                     std::vector<Target>& targets)
{
  std::vector<HitResult> hits;
  for (int i = 0; i < (int)targets.size(); i++)
  {
    RayCollision collision = targets[i].getCollision(ray);
    if (collision.hit)
    {
      float worldDistance = Vector3Distance(ray.position, collision.point);
      hits.push_back({i, worldDistance});
    }
  }
  std::sort(hits.begin(), hits.end(), [](const HitResult& a, const HitResult& b)
            { return a.distance < b.distance; });
  return hits;
}

void updateShooting(Camera& camera, InputState& input,
                    std::vector<Target>& targets)
{
  if (!input.shoot) return;

  Ray ray;
  ray.position = camera.position;
  ray.direction =
      Vector3Normalize(Vector3Subtract(camera.target, camera.position));

  auto hits = getSortedHits(ray, targets);
  if (hits.empty()) return;

  int closestIndex = hits.front().index;
  targets[closestIndex].takeDamage(DAMAGE_PER_HIT);

  if (targets[closestIndex].isDead())
  {
    targets[closestIndex] = targets.back();
    targets.pop_back();
  }

  std::cout << "HIT\n";
}

void drawCrosshair()
{
  int const size = 10;

  DrawLine((SCREEN_WIDTH / 2) - size, (SCREEN_HEIGHT / 2),
           (SCREEN_WIDTH / 2) + size, (SCREEN_HEIGHT / 2), BLACK);
  DrawLine((SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2) - size, (SCREEN_WIDTH / 2),
           (SCREEN_HEIGHT / 2) + size, BLACK);
}

int main()
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "shooter");
  SetTargetFPS(FPS);
  DisableCursor();

  InputState input;

  Camera camera = {};
  camera.position = (Vector3){0.0f, 1.0f, 10.0f};
  camera.target = (Vector3){0.0f, 2.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = FOV;
  camera.projection = CAMERA_PERSPECTIVE;

  float yaw = 0.0f;
  float pitch = 0.0f;

  std::vector<Target> targets;
  targets.push_back(Target(Vector3{0.0f, 1.0f, 0.0f}, 1.0f, RED, 100));
  targets.push_back(Target(Vector3{3.0f, 1.0f, 0.0f}, 1.0f, RED, 100));
  targets.push_back(Target(Vector3{6.0f, 1.0f, 0.0f}, 1.0f, RED, 100));
  targets.push_back(Target(Vector3{9.0f, 1.0f, 0.0f}, 1.0f, RED, 100));

  while (!WindowShouldClose())
  {
    input.gatherInput();

    updateLook(camera, input, yaw, pitch);
    updateShooting(camera, input, targets);

    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode3D(camera);

    DrawPlane((Vector3){0.0f, 0.0f, 0.0f}, (Vector2){20.0f, 20.0f}, LIGHTGRAY);
    DrawGrid(20, 1.0f);
    for (Target& target : targets) target.draw();

    EndMode3D();
    drawCrosshair();

    DrawFPS(10, 10);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}

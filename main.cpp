#include <algorithm>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>

#include "config.h"
#include "input.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "target.h"

enum class Mode
{
  EDIT,
  PLAY
};

nlohmann::json data = nlohmann::json::parse(std::ifstream("scenario.json"));

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

void updateMode(Mode& mode, InputState& input)
{
  if (input.modeSwitch)
  {
    mode = (mode == Mode::PLAY) ? Mode::EDIT : Mode::PLAY;
    if (mode == Mode::PLAY) DisableCursor();
    else EnableCursor();
  }
}

void update(Mode& mode, Camera& camera, InputState& input, float& yaw,
            float& pitch, std::vector<Target>& targets, float dt,
            const std::vector<Plane>& bounds)
{
  updateMode(mode, input);
  if (mode == Mode::PLAY)
  {
    updateLook(camera, input, yaw, pitch);
    updateShooting(camera, input, targets);
    for (auto& t : targets) t.update(dt, bounds);
  }
}

float horizontalToVerticalFOV(float hFOVDegrees, float aspectRatio)
{
  float hFOVRadians = hFOVDegrees * DEG2RAD;
  float vFOVRadians = 2.0f * atanf(tanf(hFOVRadians / 2.0f) / aspectRatio);
  return vFOVRadians * RAD2DEG;
}

Vector3 loadVector3(const nlohmann::json& arr)
{
  return Vector3{arr[0], arr[1], arr[2]};
}

Color stringToColor(const std::string& s)
{
  if (s == "RED") return RED;
  if (s == "BLUE") return BLUE;
  if (s == "PURPLE") return PURPLE;
  return GRAY;
}


BehaviourType stringToBehaviourType(const std::string& s)
{
  if (s == "STATIC") return BehaviourType::STATIC;
  if (s == "LINEAR") return BehaviourType::LINEAR;
  return BehaviourType::STATIC;
}

std::vector<Plane> loadBounds(const nlohmann::json& data)
{
  std::vector<Plane> bounds;
  for (auto& b : data["bounds"])
  {
    Plane plane{
        loadVector3(b["point"]),
        Vector3Normalize(loadVector3(b["normal"])),
    };
    bounds.push_back(plane);
  }
  return bounds;
}

std::vector<Target> loadTargets(const nlohmann::json& data)
{
  std::vector<Target> targets;
  for (auto& t : data["targets"])
  {
    std::vector<Behaviour> routine;
    for (auto& b : t["routine"])
    {
      Behaviour behaviour{stringToBehaviourType(b["type"]), b["duration"],
                          loadVector3(b["direction"]), b["speed"]};
      routine.push_back(behaviour);
    }
    targets.push_back(Target(loadVector3(t["position"]), t["radius"],
                             stringToColor(t["color"]), t["health"],
                             Routine(routine)));
  }
  return targets;
}

void drawPlaneGrid(const Plane& plane, float size, int divisions, Color color)
{
  Vector3 up =
      fabsf(plane.normal.y) < 0.99f ? Vector3{0, 1, 0} : Vector3{1, 0, 0};
  Vector3 tangent = Vector3Normalize(Vector3CrossProduct(up, plane.normal));
  Vector3 bitangent = Vector3CrossProduct(plane.normal, tangent);
  float step = (2.0f * size) / divisions;
  for (int i = 0; i <= divisions; i++)
  {
    float offset = -size + i * step;
    Vector3 a1 =
        Vector3Add(plane.point, Vector3Add(Vector3Scale(tangent, offset),
                                           Vector3Scale(bitangent, -size)));
    Vector3 a2 =
        Vector3Add(plane.point, Vector3Add(Vector3Scale(tangent, offset),
                                           Vector3Scale(bitangent, size)));
    DrawLine3D(a1, a2, color);
    Vector3 b1 =
        Vector3Add(plane.point, Vector3Add(Vector3Scale(bitangent, offset),
                                           Vector3Scale(tangent, -size)));
    Vector3 b2 =
        Vector3Add(plane.point, Vector3Add(Vector3Scale(bitangent, offset),
                                           Vector3Scale(tangent, size)));
    DrawLine3D(b1, b2, color);
  }
}

void draw(const Camera& camera, const std::vector<Target>& targets,
          const std::vector<Plane>& bounds)
{
  BeginDrawing();
  ClearBackground(RAYWHITE);
  BeginMode3D(camera);
  for (const auto& b : bounds) drawPlaneGrid(b, 10.0f, 20, LIGHTGRAY);
  for (const auto& t : targets) t.draw();
  EndMode3D();
  drawCrosshair();
  DrawFPS(10, 10);
  EndDrawing();
}

int main()
{
  // stuff before gameloop
  Mode mode = Mode::PLAY;
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "shooter");
  SetTargetFPS(FPS);
  DisableCursor();

  InputState input;

  Camera camera = {};
  camera.position = (Vector3){0.0f, 1.0f, 10.0f};
  camera.target = (Vector3){0.0f, 2.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = horizontalToVerticalFOV(FOV, ASPECT_RATIO);
  camera.projection = CAMERA_PERSPECTIVE;

  float yaw = 0.0f;
  float pitch = 0.0f;


  std::vector<Plane> bounds = loadBounds(data);
  std::vector<Target> targets = loadTargets(data);


  while (!WindowShouldClose())
  {
    float dt = GetFrameTime();

    // inputs
    input.gatherInput();

    // update
    update(mode, camera, input, yaw, pitch, targets, dt, bounds);

    // draw
    draw(camera, targets, bounds);
  }

  CloseWindow();

  return 0;
}

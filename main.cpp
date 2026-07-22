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

float distancePointToRay(Vector3 point, const Ray& ray)
{
  Vector3 toPoint = Vector3Subtract(point, ray.position);
  float projectionLength = Vector3DotProduct(toPoint, ray.direction);
  Vector3 closestPointOnRay =
      Vector3Add(ray.position, Vector3Scale(ray.direction, projectionLength));
  return Vector3Distance(point, closestPointOnRay);
}

struct TargetProbe
{
  int index;
  float distanceFromCenter;
  bool hit;
  Vector3 point;
};

std::vector<TargetProbe> probeTargets(const Ray& ray,
                                      std::vector<Target>& targets)
{
  std::vector<TargetProbe> probes;
  for (int i = 0; i < (int)targets.size(); i++)
  {
    float d = distancePointToRay(targets[i].getPosition(), ray);
    RayCollision collision = targets[i].getCollision(ray);
    probes.push_back({i, d, collision.hit, collision.point});
  }
  return probes;
}

struct FrameRecord
{

  int frameNumber;
  Vector2 mouseDelta;
  float jaggedness;
  bool wasShot;
  bool hit;
  float distanceFromCenter;
};

class SessionStats
{
private:
  int shotsFired = 0;
  int shotsHit = 0;
  int score = 0;
  int frameCounter = 0;
  std::vector<FrameRecord> frames;


public:
  void recordFrame(Vector2 mouseDelta, float jaggedness)
  {
    frameCounter++;
    frames.push_back(
        {frameCounter, mouseDelta, jaggedness, false, false, 0.0f});
  }
  void recordShot(float distanceFromCenter, bool hit)
  {
    shotsFired++;
    if (hit) shotsHit++;
    if (!frames.empty())
    {
      frames.back().wasShot = true;
      frames.back().hit = hit;
      frames.back().distanceFromCenter = distanceFromCenter;
    }
  }

  float accuracy() const
  {
    return shotsFired == 0 ? 0.0f : (float)shotsHit / (float)shotsFired;
  }

  const std::vector<FrameRecord>& getFrames() const
  {
    return frames;
  }
};

void updateShooting(Camera& camera, InputState& input,
                    std::vector<Target>& targets, SessionStats& stats)
{
  if (!input.shoot) return;

  Ray ray;
  ray.position = camera.position;
  ray.direction =
      Vector3Normalize(Vector3Subtract(camera.target, camera.position));

  auto probes = probeTargets(ray, targets);
  if (probes.empty()) return;

  auto closestOverall =
      std::min_element(probes.begin(), probes.end(),
                       [](const TargetProbe& a, const TargetProbe& b)
                       { return a.distanceFromCenter < b.distanceFromCenter; });
  stats.recordShot(closestOverall->distanceFromCenter, closestOverall->hit);

  auto hitTarget = std::find_if(probes.begin(), probes.end(),
                                [](const TargetProbe& p) { return p.hit; });
  if (hitTarget == probes.end()) return;

  targets[hitTarget->index].takeDamage(DAMAGE_PER_HIT);
  if (targets[hitTarget->index].isDead())
  {
    targets[hitTarget->index] = targets.back();
    targets.pop_back();
  }
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
            const std::vector<Plane>& bounds, SessionStats& stats)
{
  updateMode(mode, input);
  if (mode == Mode::PLAY)
  {
    float jaggedness = 0.0f;
    stats.recordFrame(input.mouseDelta, jaggedness);
    updateLook(camera, input, yaw, pitch);
    updateShooting(camera, input, targets, stats);
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
          const std::vector<Plane>& bounds, const SessionStats& stats)
{
  BeginDrawing();
  ClearBackground(RAYWHITE);
  BeginMode3D(camera);
  for (const auto& b : bounds) drawPlaneGrid(b, 10.0f, 20, LIGHTGRAY);
  for (const auto& t : targets) t.draw();
  EndMode3D();
  drawCrosshair();
  DrawFPS(10, 10);
  DrawText(TextFormat("Accuracy: %.1f%%", stats.accuracy() * 100.0f), 10, 40,
           20, BLACK);
  EndDrawing();
}

void writeFramesToCSV(const SessionStats& stats)
{
  std::ofstream out("frames.csv");
  out << "frame,mouseDeltaX,mouseDeltaY,jaggedness,wasShot,hit,"
         "distanceFromCenter\n";
  for (const FrameRecord& f : stats.getFrames())
  {
    out << f.frameNumber << "," << f.mouseDelta.x << "," << f.mouseDelta.y
        << "," << f.jaggedness << "," << (f.wasShot ? "true" : "false") << ",";
    if (f.wasShot)
      out << (f.hit ? "true" : "false") << "," << f.distanceFromCenter;
    else out << ",";
    out << "\n";
  }
}

int main()
{
  Mode mode = Mode::PLAY;
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "shooter");
  SetTargetFPS(FPS);
  DisableCursor();

  InputState input;
  SessionStats stats;

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
    input.gatherInput();
    update(mode, camera, input, yaw, pitch, targets, dt, bounds, stats);
    draw(camera, targets, bounds, stats);
  }

  writeFramesToCSV(stats);
  CloseWindow();
  return 0;
}

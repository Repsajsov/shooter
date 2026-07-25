#include "loader.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "randomizer.h"
#include "raymath.h"

namespace
{
Vector3 loadVector3(const nlohmann::json& arr)
{
  return Vector3{arr[0], arr[1], arr[2]};
}

Color stringToColor(const std::string& s)
{
  if (s == "LIGHTGRAY") return LIGHTGRAY;
  if (s == "GRAY") return GRAY;
  if (s == "DARKGRAY") return DARKGRAY;
  if (s == "YELLOW") return YELLOW;
  if (s == "GOLD") return GOLD;
  if (s == "ORANGE") return ORANGE;
  if (s == "PINK") return PINK;
  if (s == "RED") return RED;
  if (s == "MAROON") return MAROON;
  if (s == "GREEN") return GREEN;
  if (s == "LIME") return LIME;
  if (s == "DARKGREEN") return DARKGREEN;
  if (s == "SKYBLUE") return SKYBLUE;
  if (s == "BLUE") return BLUE;
  if (s == "DARKBLUE") return DARKBLUE;
  if (s == "PURPLE") return PURPLE;
  if (s == "VIOLET") return VIOLET;
  if (s == "DARKPURPLE") return DARKPURPLE;
  if (s == "BEIGE") return BEIGE;
  if (s == "BROWN") return BROWN;
  if (s == "DARKBROWN") return DARKBROWN;
  if (s == "WHITE") return WHITE;
  if (s == "BLACK") return BLACK;
  if (s == "BLANK") return BLANK;
  if (s == "MAGENTA") return MAGENTA;
  if (s == "RAYWHITE") return RAYWHITE;
  return GRAY;
}

BehaviourType stringToBehaviourType(const std::string& s)
{
  if (s == "STATIC") return BehaviourType::STATIC;
  if (s == "LINEAR") return BehaviourType::LINEAR;
  return BehaviourType::STATIC;
}

PositionMode stringToPositionMode(const std::string& s)
{
  if (s == "RANDOM") return PositionMode::RANDOM;
  if (s == "FIXED") return PositionMode::FIXED;
  return PositionMode::RANDOM;
}

std::vector<Plane> loadBounds(const nlohmann::json& data)
{
  std::vector<Plane> bounds;
  for (auto& b : data["bounds"])
    bounds.push_back(Plane{loadVector3(b["point"]),
                           Vector3Normalize(loadVector3(b["normal"]))});
  return bounds;
}

Vector3 resolveDirection(const nlohmann::json& b)
{
  return (b["direction"].is_string() && b["direction"] == "random")
             ? GetRandomDirection()
             : Vector3Normalize(loadVector3(b["direction"]));
}

Behaviour loadBehaviour(const nlohmann::json& b)
{
  return Behaviour{stringToBehaviourType(b["type"]), b["duration"],
                   resolveDirection(b), b["speed"]};
}

std::vector<Behaviour> loadRoutineSteps(const nlohmann::json& t)
{
  std::vector<Behaviour> routine;
  for (auto& b : t["routine"]) routine.push_back(loadBehaviour(b));
  return routine;
}

Vector3 resolvePosition(const nlohmann::json& t, PositionMode positionMode,
                        Vector3 roomMin, Vector3 roomMax)
{
  return (positionMode == PositionMode::FIXED)
             ? loadVector3(t["position"])
             : GetRandomPosition(roomMin, roomMax);
}

Target loadSingleTarget(const nlohmann::json& t, PositionMode positionMode,
                        Vector3 roomMin, Vector3 roomMax)
{
  Vector3 position = resolvePosition(t, positionMode, roomMin, roomMax);
  Routine routine(loadRoutineSteps(t));
  return Target(position, t["radius"], stringToColor(t["color"]), t["health"],
                routine, t["respawns"], positionMode);
}

std::vector<Target> loadTargets(const nlohmann::json& data, Vector3 roomMin,
                                Vector3 roomMax)
{
  std::vector<Target> targets;
  for (auto& t : data["targets"])
  {
    PositionMode positionMode = stringToPositionMode(t["positionMode"]);
    int count = t["count"];
    for (int n = 0; n < count; n++)
      targets.push_back(loadSingleTarget(t, positionMode, roomMin, roomMax));
  }
  return targets;
}
} // namespace

Room jsonLoader::loadRoomFromFile(const std::string& path)
{
  nlohmann::json data = nlohmann::json::parse(std::ifstream(path));
  Vector3 min = loadVector3(data["roomBounds"]["min"]);
  Vector3 max = loadVector3(data["roomBounds"]["max"]);
  std::vector<Plane> bounds = loadBounds(data);
  std::vector<Target> targets = loadTargets(data, min, max);
  return Room(min, max, std::move(bounds), std::move(targets));
}

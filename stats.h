#ifndef STATS_H
#define STATS_H

#include <string>
#include <vector>

#include "raylib.h"

struct FrameRecord
{
  int frameNumber;
  Vector2 mouseDelta;
  float jaggedness;
  bool wasShot;
  bool hit;
  float distanceFromCenter;
  float angleFromCenter;
};

class Stats
{
private:
  int shotsFired = 0;
  int shotsHit = 0;
  int score = 0;
  int frameCounter = 0;
  std::vector<FrameRecord> frames;

public:
  void recordFrame(Vector2 mouseDelta, float jaggedness);
  void recordShot(float distanceFromCenter, float angleFromCenter, bool hit);
  float accuracy() const;
  const std::vector<FrameRecord>& getFrames() const;
  int getScore() const;
  void draw() const;
  void writeToCSV(const std::string& path) const;
};

#endif

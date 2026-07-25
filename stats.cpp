#include "stats.h"

#include <fstream>

void Stats::recordFrame(Vector2 mouseDelta, float jaggedness)
{
  frameCounter++;
  frames.push_back(
      {frameCounter, mouseDelta, jaggedness, false, false, 0.0f, 0.0f});
}

void Stats::recordShot(float distanceFromCenter, float angleFromCenter,
                       bool hit)
{
  shotsFired++;
  if (hit)
  {
    shotsHit++;
    score++;
  }
  if (!frames.empty())
  {
    frames.back().wasShot = true;
    frames.back().hit = hit;
    frames.back().distanceFromCenter = distanceFromCenter;
    frames.back().angleFromCenter = angleFromCenter;
  }
}

float Stats::accuracy() const
{
  return shotsFired == 0 ? 0.0f : (float)shotsHit / (float)shotsFired;
}

const std::vector<FrameRecord>& Stats::getFrames() const
{
  return frames;
}

int Stats::getScore() const
{
  return score;
}

void Stats::draw() const
{
  DrawText(TextFormat("Accuracy: %.1f%%", accuracy() * 100.0f), 10, 40, 20,
           BLACK);
  DrawText(TextFormat("Score: %i", getScore()), 200, 40, 20, BLACK);
}

void Stats::writeToCSV(const std::string& path) const
{
  std::ofstream out(path);
  out << "frame,mouseDeltaX,mouseDeltaY,jaggedness,wasShot,hit,"
         "distanceFromCenter,angleFromCenter\n";
  for (const FrameRecord& f : frames)
  {
    out << f.frameNumber << "," << f.mouseDelta.x << "," << f.mouseDelta.y
        << "," << f.jaggedness << "," << (f.wasShot ? "true" : "false") << ",";
    if (f.wasShot)
      out << (f.hit ? "true" : "false") << "," << f.distanceFromCenter << ","
          << f.angleFromCenter;
    else out << ",,";
    out << "\n";
  }
}

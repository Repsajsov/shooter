#ifndef CONFIG_H
#define CONFIG_H

#include <string>

int constexpr SAMPLE_SIZE = 100000;
int constexpr SCREEN_WIDTH = 1600;
int constexpr SCREEN_HEIGHT = 900;
int constexpr DAMAGE_PER_HIT = 20;
int constexpr FPS = 360;
int constexpr FOV = 120;

float constexpr ASPECT_RATIO = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;

const std::string SCENARIO_PATH = "scenario.json";
const std::string FRAMES_CSV_PATH = "frames.csv";

#endif

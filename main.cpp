#include "config.h"
#include "game.h"
#include "loader.h"
#include "raylib.h"

int main()
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "shooter");
  SetTargetFPS(FPS);

  Room room = jsonLoader::loadRoomFromFile(SCENARIO_PATH);
  Game game(std::move(room));
  game.run();

  CloseWindow();
  return 0;
}

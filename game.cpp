#include "game.h"

#include "config.h"
#include "raylib.h"

Game::Game(Room room) : room(std::move(room))
{
  DisableCursor();
}

void Game::updateMode()
{
  if (input.modeSwitch)
  {
    mode = (mode == Mode::PLAY) ? Mode::EDIT : Mode::PLAY;
    if (mode == Mode::PLAY) DisableCursor();
    else EnableCursor();
  }
}

void Game::update(float dt)
{
  updateMode();
  if (mode != Mode::PLAY) return;

  stats.recordFrame(input.mouseDelta, 0.0f);
  player.update(input);

  if (input.shoot)
  {
    ShotResult result = room.processShot(player.getShootRay(), DAMAGE_PER_HIT);
    if (result.anyTargetInScene)
      stats.recordShot(result.distanceFromCenter, result.angleFromCenter,
                       result.hit);
  }

  room.update(dt);
}

void Game::draw() const
{
  BeginDrawing();
  ClearBackground(RAYWHITE);

  BeginMode3D(player.getCamera());
  room.draw();
  EndMode3D();

  player.draw();
  DrawFPS(10, 10);
  stats.draw();

  EndDrawing();
}

void Game::run()
{
  while (!WindowShouldClose())
  {
    float dt = GetFrameTime();
    input.gatherInput();
    update(dt);
    draw();
  }
  stats.writeToCSV(FRAMES_CSV_PATH);
}

#include "raylib.h"

int main()
{
  InitWindow(800, 450, "shooter");
  SetTargetFPS(144);
  DisableCursor();

  Camera3D camera = {
      .position = {0.0f, 2.0f, 4.0f},
      .target = {0.0f, 0.0f, 0.0f},
      .up = {0.0f, 1.0f, 0.0f},
      .fovy = 60.0f,
      .projection = CAMERA_PERSPECTIVE,
  };


  while (!WindowShouldClose())
  {
    UpdateCamera(&camera, CAMERA_FIRST_PERSON);

    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode3D(camera);

    DrawPlane((Vector3){0.0f, 0.0f, 0.0f}, (Vector2){20.0f, 20.0f}, LIGHTGRAY);
    DrawCube((Vector3){0.0f, 1.0f, 0.0f}, 2.0f, 2.0f, 2.0f, RED);
    DrawCubeWires((Vector3){0.0f, 1.0f, 0.0f}, 2.0f, 2.0f, 2.0f, MAROON);
    DrawGrid(20, 1.0f);

    EndMode3D();

    DrawFPS(10, 10);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}

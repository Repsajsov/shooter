#include <iostream>

#include "raylib.h"
#include "raymath.h"

int const SCREEN_WIDTH = 800;
int const SCREEN_HEIGHT = 450;

struct InputState
{
  bool moveForward;
  bool moveBack;
  bool moveLeft;
  bool moveRight;
  Vector2 mouseDelta;
  bool shoot;

  void gatherInput();
};

void InputState::gatherInput()
{
  moveForward = IsKeyDown(KEY_W);
  moveBack = IsKeyDown(KEY_S);
  moveLeft = IsKeyDown(KEY_A);
  moveRight = IsKeyDown(KEY_Q);
  mouseDelta = GetMouseDelta();
  shoot = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

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

void updatePosition(Camera& camera, InputState& input, float yaw, float dt)
{
  float moveSpeed = 5.0f;

  Vector3 moveForward = {sinf(yaw), 0.0f, cosf(yaw)};
  Vector3 up = {0.0f, 1.0f, 0.0f};
  Vector3 moveRight = Vector3Normalize(Vector3CrossProduct(moveForward, up));

  Vector3 movement = {};
  if (input.moveForward)
  {
    movement.x += moveForward.x;
    movement.z += moveForward.z;
  }
  if (input.moveBack)
  {
    movement.x -= moveForward.x;
    movement.z -= moveForward.z;
  }
  if (input.moveLeft)
  {
    movement.x -= moveRight.x;
    movement.z -= moveRight.z;
  }
  if (input.moveRight)
  {
    movement.x += moveRight.x;
    movement.z += moveRight.z;
  }
  movement = Vector3Normalize(movement);

  camera.position.x += movement.x * moveSpeed * dt;
  camera.position.z += movement.z * moveSpeed * dt;
}

void updateShooting(Camera& camera, InputState& input, BoundingBox& targetBox)
{
  if (!input.shoot) return;

  Ray ray;
  ray.position = camera.position;
  ray.direction =
      Vector3Normalize(Vector3Subtract(camera.target, camera.position));

  RayCollision collision = GetRayCollisionBox(ray, targetBox);
  if (collision.hit) std::cout << "HIT!" << std::endl;
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
  SetTargetFPS(144);
  DisableCursor();

  InputState input;

  Camera camera = {};
  camera.position = (Vector3){0.0f, 1.0f, 10.0f};
  camera.target = (Vector3){0.0f, 2.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 75.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  float yaw = 0.0f;
  float pitch = 0.0f;
  float dt = 0.0f;


  while (!WindowShouldClose())
  {
    dt = GetFrameTime();
    input.gatherInput();

    Vector3 redBox;
    redBox.x = 0.0f;
    redBox.y = 1.0f;
    redBox.z = 0.0f;
    float redBoxSize = 2.0f;

    BoundingBox box;
    box.min = (Vector3){redBox.x - redBoxSize / 2, redBox.y - redBoxSize / 2,
                        redBox.z - redBoxSize / 2};
    box.max =
        Vector3Add(box.min, (Vector3){redBoxSize, redBoxSize, redBoxSize});

    updateLook(camera, input, yaw, pitch);
    updatePosition(camera, input, yaw, dt);
    updateShooting(camera, input, box);

    BeginDrawing();
    ClearBackground(RAYWHITE);


    BeginMode3D(camera);

    DrawPlane((Vector3){0.0f, 0.0f, 0.0f}, (Vector2){20.0f, 20.0f}, LIGHTGRAY);
    DrawCube(redBox, redBoxSize, redBoxSize, redBoxSize, RED);
    DrawCubeWires((Vector3){0.0f, 1.0f, 0.0f}, 2.0f, 2.0f, 2.0f, MAROON);
    DrawGrid(20, 1.0f);
    EndMode3D();
    drawCrosshair();

    DrawFPS(10, 10);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}

#include <iostream>
#include <vector>

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

void updateShooting(Camera& camera, InputState& input, std::vector<Box>& boxes)
{
  if (!input.shoot) return;

  Ray ray;
  ray.position = camera.position;
  ray.direction =
      Vector3Normalize(Vector3Subtract(camera.target, camera.position));

  for (auto it = boxes.begin(); it != boxes.end(); ++it)
  {

    RayCollision collision = GetRayCollisionBox(ray, it->getBoundingBox());

    if (collision.hit)
    {
      boxes.erase(it);
      break;
    }
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


class Box
{
private:
  Vector3 position;
  Vector3 size;
  BoundingBox bbox;
  Color color;

public:
  Box(Vector3 position, Vector3 size, Color color);
  BoundingBox getBoundingBox();
  void draw();
  ~Box();
};

Box::Box(Vector3 position, Vector3 size, Color color)
    : position(position), size(size), color(color)
{
  Vector3 halfSize = Vector3Scale(size, 0.5f);
  bbox.min = Vector3Subtract(position, halfSize);
  bbox.max = Vector3Add(position, halfSize);
}

BoundingBox Box::getBoundingBox()
{
  return bbox;
}

void Box::draw()
{
  DrawCube(position, size.x, size.y, size.z, color);
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


  Box redBox({0.0f, 1.0f, 0.0f}, {2.0f, 2.0f, 2.0f}, RED);
  Box blueBox({4.0f, 1.5f, -1.0f}, {2.0f, 2.0f, 2.0f}, BLUE);

  std::vector<Box> boxes;
  boxes.push_back(redBox);
  boxes.push_back(blueBox);


  while (!WindowShouldClose())
  {
    dt = GetFrameTime();
    input.gatherInput();


    updateLook(camera, input, yaw, pitch);
    updatePosition(camera, input, yaw, dt);
    updateShooting(camera, input, boxes);

    BeginDrawing();
    ClearBackground(RAYWHITE);


    BeginMode3D(camera);

    DrawPlane((Vector3){0.0f, 0.0f, 0.0f}, (Vector2){20.0f, 20.0f}, LIGHTGRAY);
    redBox.draw();
    blueBox.draw();
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

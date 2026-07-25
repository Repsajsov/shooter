#include "player.h"

#include "config.h"
#include "raymath.h"

namespace
{
float horizontalToVerticalFOV(float hFOVDegrees, float aspectRatio)
{
  float hFOVRadians = hFOVDegrees * DEG2RAD;
  float vFOVRadians = 2.0f * atanf(tanf(hFOVRadians / 2.0f) / aspectRatio);
  return vFOVRadians * RAD2DEG;
}
} // namespace

Player::Player()
{
  camera.position = Vector3{0.0f, 5.0f, 10.0f};
  camera.target = Vector3{0.0f, 5.0f, 0.0f};
  camera.up = Vector3{0.0f, 1.0f, 0.0f};
  camera.fovy = horizontalToVerticalFOV(FOV, ASPECT_RATIO);
  camera.projection = CAMERA_PERSPECTIVE;
}

void Player::update(const InputState& input)
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

void Player::draw() const
{
  int const size = 10;
  DrawLine((SCREEN_WIDTH / 2) - size, (SCREEN_HEIGHT / 2),
           (SCREEN_WIDTH / 2) + size, (SCREEN_HEIGHT / 2), BLACK);
  DrawLine((SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2) - size, (SCREEN_WIDTH / 2),
           (SCREEN_HEIGHT / 2) + size, BLACK);
}

Ray Player::getShootRay() const
{
  Ray ray;
  ray.position = camera.position;
  ray.direction =
      Vector3Normalize(Vector3Subtract(camera.target, camera.position));
  return ray;
}

const Camera& Player::getCamera() const
{
  return camera;
}

#ifndef GAME_H
#define GAME_H

#include "input.h"
#include "player.h"
#include "room.h"
#include "stats.h"

enum class Mode
{
  EDIT,
  PLAY
};

class Game
{
private:
  Mode mode = Mode::PLAY;
  Player player;
  InputState input;
  Room room;
  Stats stats;

  void updateMode();
  void update(float dt);
  void draw() const;

public:
  Game(Room room);
  void run();
};

#endif

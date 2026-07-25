#ifndef LOADER_H
#define LOADER_H

#include <string>

#include "room.h"

namespace jsonLoader
{
Room loadRoomFromFile(const std::string& path);
}

#endif

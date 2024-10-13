#pragma once
#include <string>
#include <vector>
#include <memory>

#include "World.h"

#ifndef ROOM_H__
#define ROOM_H__



struct RoomDef {
	id_t id;
	std::string name;
	std::string description;
	std::vector<int> connectedRooms;
	std::vector<std::unique_ptr<Trigger>> triggers;
};

std::istream& operator>>(std::istream& is, RoomDef& def);

typedef Registry<RoomDef>  RoomRegistry;

extern RoomRegistry g_roomRegistry;

#endif // !ROOM_H__

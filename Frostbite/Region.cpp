#include "Region.h"
#include "Engine.h"

#include<cassert>
#include <iostream>
#include <climits>

RoomRegistry g_roomRegistry;

std::istream& operator>>(std::istream& is, RoomDef& def)
{

	is >> def.id;
	is.ignore(INT_MAX, '\n');
	getline(is, def.name);
	getline(is, def.description);

	int countConnectedRooms;
	is >> countConnectedRooms;
	for (int i = 0; i < countConnectedRooms; ++i) {
		int roomId;
		is >> roomId;
		def.connectedRooms.push_back(roomId);
	}

	int countTriggers;
	is >> countTriggers;
	for (int i = 0; i < countTriggers; ++i) {
		std::string triggerName;
		is >> triggerName;
		Trigger* trigger = createTriggerByName(triggerName);
		trigger->loadFromStream(is);
		def.triggers.push_back(std::unique_ptr<Trigger>(trigger));
	}

	return is;

}
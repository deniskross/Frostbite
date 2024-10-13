#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <functional>
#include <ctime>

#include "Engine.h"
#include "Region.h"
#include "Trap.h"
#include "World.h"
#include "Character.h"
#include "Battle.h"


int main(int argc, char* argv[]) {

	srand((unsigned)time(nullptr));

	std::ifstream input("world.txt");

	g_monsterRegistry.loadFromStream(input);
	g_itemRegistry.loadFromStream(input);
	g_trapRegistry.loadFromStream(input);
	g_roomRegistry.loadFromStream(input);
	int startRoomId;
	input >> startRoomId;

	input.close();

	WorldState worldState;
	worldState.initNewGame(startRoomId);

	id_t itemId = worldState.generateItem(g_itemRegistry.getDef(1));
	worldState.addToInventory(itemId);
	worldState.equipItem(0);

	bool isEnded = false;
	for (;;) {

		const RoomDef* currentRoom = g_roomRegistry.getDef(worldState.getCurrentRoomId());
		writef("{} \n\n", currentRoom->description);

		int lastIndex = worldState.getRoomTriggerIndex(currentRoom->id);

		for (index_t i = lastIndex; i < isize(currentRoom->triggers); ++i) {
			bool success = currentRoom->triggers[i]->execute(worldState);
			if (success) {
				worldState.incRoomTriggerIndex(currentRoom->id);
			}
			else {
				break;
			}
			if (worldState.isGameEnded()) {
				isEnded = true;
				break;
			}
		}

		if (isEnded) {
			break;
		}

		std::vector <std::string> choices;
		std::transform(
			currentRoom->connectedRooms.begin(),
			currentRoom->connectedRooms.end(),
			std::back_inserter(choices),
			[](int roomId) {return g_roomRegistry.getDef(roomId)->name; });

		int choice = dialog(worldState, "", "Where to go?", choices);
		worldState.setCurrentRoomId(currentRoom->connectedRooms[choice]);
	}

	return 0;
}

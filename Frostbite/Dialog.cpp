#include "World.h"

#include <iostream>
#include <cctype>
#include <algorithm>
#include <sstream>

#include "Engine.h"

static const char* STAT_NAMES[] = {
		"STR",
		"DEX",
		"ATK",
		"DEF",
		"HP" ,
		"HP MAX"
};

static const char* EQ_SLOT_NAMES[] = {
		"",
		"Main hand"
};

index_t dialog(
	const std::string& message,
	const std::string& prompt,
	const std::vector<std::string>& choices,
	const std::vector<std::string>& systemChoices)
{
	if (isize(message) > 0) {
		writef("{}\n\n", message);
	}

	for (index_t i = 0; i < isize(choices); ++i) {
		if (isize(choices) == 1) {
			writef("\033[97m{}. \033[94m{}\033[0m \n\n", i + 1, choices[i]);
		}
		else { writef("\033[97m{}. \033[94m{}\033[0m \n", i + 1, choices[i]); }
	}

	std::vector<char> systemChars;
	for (index_t i = 0; i < isize(systemChoices); ++i) {
		char key = *(systemChoices[i].c_str());
		systemChars.push_back(tolower(key));
		writef("\033[94m{}\033[0m{}\n", key, systemChoices[i].c_str() + 1);
	}

	std::string input;
	index_t choice;
	do {
		choice = 0;
		writef("\n\033[93m{}> \033[0m", prompt);
		readln(input);
		if (isize(input) > 0) {
			for (index_t i = 0; i < isize(systemChars); ++i) {
				if (tolower(input[0]) == systemChars[i]) {
					return -(i + 1);
				}
			}

			choice = atoi(input.c_str());
		}
	} while (choice < 1 || choice > isize(choices));

	return choice - 1;
}

void printItem(const Item* item)
{
	const ItemDef* itemDef = g_itemRegistry.getDef(item->defId);
	writef("\n\033[96m{} \033[0m\n", item->name);
	if (!itemDef->description.empty()) {
		writef("{}\n", itemDef->description);
	}
	for (const auto& m : item->modifiers) {
		writef("{}: ", STAT_NAMES[m.stat]);
		if (m.add != 0) {
			writef("\033[92m{}{}\033[0m", (m.add > 0 ? "+" : ""), m.add);
			if (m.mult != 0) writef(", ");
		}

		if (m.mult != 0) {
			writef("\033[92m{}{}% \033[0m", (m.mult > 0 ? "+" : ""), m.mult);
		}
		writef("\n");
	}
}

void dialogCharacter(WorldState& state)
{
	const Stats& stats = state.getCharacterStats();
	const Equipment& equipment = state.getEquipment();
	const Table<Item>& itemTable = state.getItemTable();

	vtOpenWnd();

	int choice;
	writef("\n\n\033[97mCharacter stats: \033[0m\n");

	for (; ; ) {
		for (index_t i = 0; i < 5; ++i) {
			writef("{}: \033[92m{}\033[0m\n", STAT_NAMES[i], stats.getStat((StatName)i));
		}

		writef("{}: \033[92m{}/{}\033[0m\n", STAT_NAMES[HIT_POINTS], stats.getStat(HIT_POINTS), stats.getStat(HIT_POINTS_MAX));

		for (index_t i = 1; i < N_EQUIPMENT_SLOTS; ++i) {
			if (equipment[i].itemId == ID_UNDEF) {
				writef("{}: \033[96m[] \033[0m\n\n", EQ_SLOT_NAMES[i]);
			}
			else {
				const Item* item = state.getItemTable().get(equipment[i].itemId);
				writef("{}: \033[96m{}\033[0m\n\n", EQ_SLOT_NAMES[i], item->name);
			}
		}


		choice = dialog("", "What to do?", { "Unequip", "Inspect equipment" }, { "Exit" });
		if (choice == 0) {

			vtOpenWnd();

			index_t unequip = dialog("", "Unequip?", {
				EQ_SLOT_NAMES[EQ_MAIN_HAND] }, { "Exit" });
			switch (unequip) {
			case 0:
				state.unequipItem(EQ_MAIN_HAND);
				break;
			}
			vtCloseWnd();

		}
		else if (choice == 1) {
			vtOpenWnd();

			index_t inspect = dialog("", "Inspect?", {
				EQ_SLOT_NAMES[EQ_MAIN_HAND] }, { "Exit" });

			enum EquipmentSlotName slot = EQ_UNDEF;

			switch (inspect) {
			case 0:
				slot = EQ_MAIN_HAND;
				break;
			}

			if (equipment[slot].itemId != ID_UNDEF) {
				const Item* item = itemTable.get(equipment[slot].itemId);
				printItem(item);
				int unequip = dialog("", "What to do?", { "Unequip" }, { "Exit" });
				if (unequip == 0) {
					state.unequipItem(slot);
				}
			}
			vtCloseWnd();
		}
		else {
			vtCloseWnd();
			return;
		}
	};
}


void dialogInventory(WorldState& state)
{
	const Inventory& inventory = state.getInventory();
	const Table<Item>& itemTable = state.getItemTable();

	vtOpenWnd();

	int itemChoice;
	writef("Character Inventory: \n");
	for (;;) {
		std::vector<std::string> choices;
		std::transform(inventory.begin(), inventory.end(), std::back_inserter(choices),
			[itemTable](const InventoryItem& invItem) {
				if (invItem.itemId == ID_UNDEF) {
					return std::string("[]");
				}
				std::stringstream str;
				const Item* item = itemTable.get(invItem.itemId);
				str << item->name;
				if (invItem.count > 1) {
					str << " x" << invItem.count;
				}
				return str.str();

			});

		itemChoice = dialog("", "Item?", choices, { "Exit" });
		if (itemChoice >= 0) {
			vtOpenWnd();

			const Item* item = itemTable.get(inventory[itemChoice].itemId);
			const ItemDef* itemDef = g_itemRegistry.getDef(item->defId);
			printItem(item);
			std::vector<std::string> itemActions;
			if (itemDef->slot != EQ_UNDEF) {
				itemActions.push_back("Equip");
				itemActions.push_back("Try your luck");
			}
			
			index_t choice = dialog("", "What to do?", itemActions, { "Exit" });
			if (choice == 0) {
				state.equipItem(itemChoice);
			}
			else if (choice == 1) {
				state.changeAll();
			}
			vtCloseWnd();
		}
		else {
			vtCloseWnd();
			return;
		}
	};
}

index_t dialog(
	WorldState& state,
	const std::string& message,
	const std::string& prompt,
	const std::vector<std::string>& choices)
{
	int choice;
	for (;;) {
		choice = dialog(message, prompt, choices, { "Character", "Inventory" });

		if (choice == -1) {
			dialogCharacter(state);
		}
		else if (choice == -2) {
			dialogInventory(state);
		}
		else {
			return choice;
		}
	};
}

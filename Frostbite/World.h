#pragma once
#ifndef WORLD_H__
#define WORLD_H__

#include <string>
#include <vector>

#include "Character.h"
#include "Engine.h"


class WorldState {
public:
	void initNewGame(id_t startRoomId);

	index_t getRoomTriggerIndex(id_t id) const;
	index_t incRoomTriggerIndex(id_t id);

	int getCurrentRoomId() const;
	void setCurrentRoomId(int value);

	bool isGameEnded() const;
	void setGameEnded(bool value);

	Table<Item>& getItemTable();
	id_t generateItem(const ItemDef* itemDef);


	bool addToInventory(id_t itemId);
	id_t removeFromInventory(id_t itemId);
	bool changeAll();

	bool equipItem(index_t inventorySlot);
	bool unequipItem(enum EquipmentSlotName slot);

	const Inventory& getInventory() const;
	const Equipment& getEquipment() const;
	const Stats& getCharacterStats() const;

private:
	id_t takeItemFromInventory(index_t inventorySlot);
	int currentRoomId_;
	std::vector<int> rooms_;
	bool isGameEnded_;

	Inventory inventory_;
	Equipment equipment_;
	Stats characterStats_;

	Table<Item> itemTable_;

};

//TRIGGER SUBSYSTEM

class Trigger {
public:
	virtual ~Trigger() {}
	virtual bool execute(WorldState& state) = 0;
	virtual void loadFromStream(std::istream& is) = 0;
};

class TriggerFactory {
public:
	virtual ~TriggerFactory() {}
	virtual Trigger* create() = 0;
};

template<typename T>
class TriggerFactoryImpl : public TriggerFactory {
public:
	virtual T* create() override { return new T(); };
};

Trigger* createTriggerByName(const std::string& name);


index_t dialog(
	WorldState& state,
	const std::string& message,
	const std::string& prompt,
	const std::vector<std::string>& choices);


#endif // WORLD_H__
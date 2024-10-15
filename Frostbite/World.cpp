#include <cassert>

#include "World.h"
#include "Region.h"
#include "Character.h"

void WorldState::initNewGame(id_t startRoomId)
{
	rooms_.resize(g_roomRegistry.count());
	currentRoomId_ = startRoomId;

	characterStats_[STRENGHT] = 3;
	characterStats_[DEXTERITY] = 3;
	characterStats_[ATTACK] = 3;
	characterStats_[DEFENSE] = 3;
	characterStats_[HIT_POINTS] = 3;
	characterStats_[HIT_POINTS_MAX] = 3;
}

index_t WorldState::getRoomTriggerIndex(id_t id) const
{
	assert(id > 0 && isize(rooms_));
	return rooms_[id];
}

index_t WorldState::incRoomTriggerIndex(id_t id)
{
	assert(id > 0 && isize(rooms_));
	return ++rooms_[id];
}


int WorldState::getCurrentRoomId() const
{
	return currentRoomId_;
}
void WorldState::setCurrentRoomId(int value)
{
	currentRoomId_ = value;
}



bool WorldState::isGameEnded() const
{
	return isGameEnded_;
}


void WorldState::setGameEnded(bool value)
{
	isGameEnded_ = value;
}

Table<Item>& WorldState::getItemTable()
{
	return itemTable_;
}

id_t WorldState::generateItem(const ItemDef* itemDef)
{
	return itemTable_.generate([itemDef](Item& item) {
		item.defId = itemDef->id;
		item.name = itemDef->name;
		for (auto& mdef : itemDef->modifiers) {
			Modifier m;
			m.stat = mdef.stat;
			m.add = random(mdef.addLo, mdef.addHi);
			m.mult = random(mdef.multLo, mdef.multHi);
			item.modifiers.push_back(m);
		}
		});
}

bool WorldState::addToInventory(id_t itemId)
{
	const Item* item = itemTable_.get(itemId);
	const ItemDef* itemDef = g_itemRegistry.getDef(item->defId);

	for (auto& invItem : inventory_) {
		if (invItem.itemId == item->id) {

			if (itemDef->maxInStack == 1) {
				return true;
			}

			if (invItem.count < itemDef->maxInStack) {
				invItem.count += 1;
				return true;
			}
		}
	}

	for (auto& invItem : inventory_) {
		if (invItem.itemId == ID_UNDEF) {
			invItem.itemId = item->id;
			invItem.count = 1;
			return true;

		}
	}

	return false;
}

id_t WorldState::takeItemFromInventory(index_t inventorySlot)
{
	id_t ItemId = inventory_[inventorySlot].itemId;
	inventory_[inventorySlot].count -= 1;
	if (inventory_[inventorySlot].count == 0) {
		inventory_[inventorySlot].itemId = ID_UNDEF;
	}
	return ItemId;
}

id_t WorldState::removeFromInventory(id_t itemId)
{
	const Item* item = itemTable_.get(itemId);

	for (index_t i = 0; i < isize(inventory_); ++i) {
		if (inventory_[i].itemId == item->id) {
			return takeItemFromInventory(i);
		}
	}
	return ID_UNDEF;
}

bool WorldState::changeAll() {
	int countItems = 0;
	for (auto& invItem : inventory_) {
		if (invItem.itemId != ID_UNDEF) {
			countItems += 1;
		}
	}

	if (countItems > 1) {
		const id_t newItemDefIndex = 1;

		for (auto& invItem : inventory_) {
			if (invItem.itemId != ID_UNDEF) {
				removeFromInventory(invItem.itemId);
			}
		}

		id_t newItemId = generateItem(g_itemRegistry.getDef(1));
		addToInventory(newItemId);

		return true;
	}
	return false;
}


bool WorldState::equipItem(index_t inventorySlot)
{
	assert(inventorySlot < N_INVENTORY_ITEMS);

	id_t itemId = inventory_[inventorySlot].itemId;
	assert(itemId != ID_UNDEF);
	if (itemId == ID_UNDEF) {
		return false;
	}

	const Item* item = itemTable_.get(itemId);
	assert(item->defId != ID_UNDEF);
	
	const ItemDef* itemDef = g_itemRegistry.getDef(itemId);
	if (itemDef->slot == EQ_UNDEF) {
		return false;
	}
	enum EquipmentSlotName equipmentSlot = itemDef->slot;

	takeItemFromInventory(inventorySlot);
	bool unequipped = true;
	id_t equippedItemId = equipment_[equipmentSlot].itemId;
	if (equippedItemId != ID_UNDEF) {
		unequipped = addToInventory(equippedItemId);
	}
	if (unequipped) {
		if (equippedItemId != ID_UNDEF) {
			characterStats_.removeModifiers(equipmentSlot);
		}

		equipment_[itemDef->slot].itemId = itemId;
		const Item* item = itemTable_.get(itemId);
		characterStats_.addModifiers(equipmentSlot, item->modifiers);
		return true;
	}
	else {
		addToInventory(itemId);
		return false;
	}
}

bool WorldState::unequipItem(enum EquipmentSlotName slot)
{
	id_t equippedItemId = equipment_[slot].itemId;
	if (equippedItemId != ID_UNDEF) {
		if (!addToInventory(equippedItemId)) {
			return false;
		}
		equipment_[slot].itemId = ID_UNDEF;
		characterStats_.removeModifiers(slot);
	}
	return true;
}



const Inventory& WorldState::getInventory() const
{
	return inventory_;
}

const Equipment& WorldState::getEquipment() const
{
	return equipment_;
}

const Stats& WorldState::getCharacterStats() const
{
	return characterStats_;
}

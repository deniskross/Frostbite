#include "Trigger.h"
#include "Engine.h"
#include "Trap.h"
#include "Battle.h"

#include <sstream>


bool WinTrigger::execute(WorldState& state)
{

	writef("\n \033[92m{} \033[0m\n", message_);
	state.setGameEnded(true);
	return true;
}
void WinTrigger::loadFromStream(std::istream& is)
{
	is.ignore(1);
	getline(is, message_);
}

bool  LootTrigger::execute(WorldState& state)
{
	assert(itemDefId_ != ID_UNDEF);
	if (itemDefId_ == ID_UNDEF) {
		return true;
	}

	id_t itemId = state.generateItem(g_itemRegistry.getDef(itemDefId_));
	const Item* item = state.getItemTable().get(itemId);
	std::stringstream str;
	str << "Congratulations! You've got \033[96m" << item->name << "\033[0m!";

	int choice = dialog(state, str.str(), "Take it?", { "Yes", "No" });
	if (choice == 0) {
		state.addToInventory(item->id);
	}
	return true;
}

void LootTrigger::loadFromStream(std::istream& is)
{
	is >> itemDefId_;
}

std::pair<const char*, std::unique_ptr<TriggerFactory>> FACTORIES[] = {
	{ "trap", std::unique_ptr<TriggerFactory>(new TriggerFactoryImpl<TrapTrigger>()) },
	{ "win", std::unique_ptr<TriggerFactory>(new TriggerFactoryImpl<WinTrigger>()) },
	{ "loot", std::unique_ptr<TriggerFactory>(new TriggerFactoryImpl<LootTrigger>()) },
	{ "battle", std::unique_ptr<TriggerFactory>(new TriggerFactoryImpl<BattleTrigger>()) },
};

Trigger* createTriggerByName(const std::string& name)
{
	for (index_t i = 0; i < sizeof(FACTORIES) / sizeof(FACTORIES[0]); ++i) {
		if (_stricmp(name.c_str(), FACTORIES[i].first) == 0) {
			return FACTORIES[i].second->create();
		}
	}
	return nullptr;
}
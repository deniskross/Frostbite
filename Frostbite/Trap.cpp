#include "Trap.h"
#include "Engine.h"

#include<cassert>
#include <iostream>
#include <climits>

const int MAX_STRING_LEN = 255;

TrapRegistry g_trapRegistry;


std::istream& operator>>(std::istream& is, TrapDef& def)
{

	is >> def.id;
	is.ignore(INT_MAX, '\n');
	getline(is, def.description);
	is >> def.damage;
	is >> def.difficulty;

	return is;

}


bool TrapTrigger::execute(WorldState& state)
{
	const TrapDef* td = g_trapRegistry.getDef(trapId_);
	int choice = dialog(state, td->description, "What to do?", { "Accept trial", "Escape" });
	return choice == 0;

}

void TrapTrigger::loadFromStream(std::istream& is)
{
	is >> trapId_;
}
#pragma once
#ifndef TRAP_H__
#define TRAP_H__

#include <string>
#include <vector>

#include "Engine.h"
#include "Trigger.h"

struct TrapDef {
	id_t id;
	std::string description;
	int damage;
	int difficulty;
};

std::istream& operator>>(std::istream& is, TrapDef& def);



typedef Registry<TrapDef>  TrapRegistry;

extern TrapRegistry g_trapRegistry;

class TrapTrigger : public Trigger {
public:
	TrapTrigger()
		: trapId_(ID_UNDEF)
	{}

	virtual bool execute(WorldState& state) override;
	virtual void loadFromStream(std::istream& is) override;

private:
	id_t trapId_;

};

#endif //TRAP_H__

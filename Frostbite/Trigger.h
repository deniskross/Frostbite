#pragma once
#ifndef TRIGGER_H__
#define TRIGGER_H__

#include "Engine.h"
#include "World.h"

class WinTrigger : public Trigger {
public:
	WinTrigger()
	{}

	virtual bool execute(WorldState& state) override;
	virtual void loadFromStream(std::istream& is) override;

private:
	std::string message_;
};


class LootTrigger : public Trigger {
public:
	LootTrigger()
		: itemDefId_(ID_UNDEF)
	{}

	virtual bool execute(WorldState& state) override;
	virtual void loadFromStream(std::istream& is) override;

private:
	id_t itemDefId_;

};

#endif
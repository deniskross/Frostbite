#pragma once
#ifndef BATTLE_H__
#define BATTLE_H__

#include "Engine.h"
#include "Trigger.h"

const index_t IDX_UNDEF = -1;
const index_t IDX_PLAYER = 0;

class BattleCharacter
{
public:
	BattleCharacter(const MonsterDef* def, const Stats& stats);

	Stats& getStats();
	std::string getName() const;
	bool isDead() const;
	const std::vector<CharacterAbilityDef>& getAbilities() const;

	std::vector<AbilityModifier>& getAbilityModifiers();
	void addAbilityModifier(id_t id, const AbilityModifier& modifier);
	void removeAbilityModifiers(id_t id);

	void appendEffect(std::shared_ptr<Effect>& effect);
	void onStartTurn();

private:
	Stats stats_;
	const MonsterDef* def_;
	std::vector<CharacterAbilityDef> abilities_;
	std::vector<AbilityModifier> abilityModifiers_;
	std::vector<std::shared_ptr<Effect>> effects_;

};


class Battle
{
public:
	Battle(WorldState* state, const MonsterDef* playerDef, const MonsterDef* monsterDef);

	BattleCharacter& getCharacter(index_t index);

	void run();

private:
	void nextRound();
	void characterTurn();
	void monsterTurn();

private:
	std::vector<BattleCharacter> chars_;
	int roundNo_;
};


class BattleTrigger : public Trigger {
public:
	BattleTrigger()
		: monsterId_(ID_UNDEF)
	{}

	virtual bool execute(WorldState& state) override;
	virtual void loadFromStream(std::istream& is) override;

private:
	id_t monsterId_;

};

#endif // BATTLE_H__
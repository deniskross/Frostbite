#pragma once
#ifndef ABILITY_H__
#define ABILITY_H__

#include "Engine.h"

class Battle;


// EFFECTS

enum EffectName {
	EF_DMG,
	EF_HEAL,
	EF_BLEED,
	N_EFFECTS
};

class Effect
{
public:
	Effect(Battle* battle, index_t source, index_t target, int countTurns);
	virtual ~Effect() {}

	virtual void apply() = 0;

	const Battle* getBattle() const { return battle_; }
	index_t getSource() const { return source_; }
	index_t getTarget() const { return target_; }
	int getCountTurns() const { return countTurns_; };
	bool isExpired() const { return countTurns_ > 0; };
	bool isImmediate() const { return countTurns_ == 0; };
	int tickTurn();


protected:
	Battle* battle_;
	index_t source_;
	index_t target_;
	int countTurns_;

};

class PhysDmgEffect : public Effect
{
public:
	PhysDmgEffect(Battle* battle, index_t source, index_t target, int countTurns);

	virtual void apply() override;
};

class HealEffect : public Effect
{
public:
	virtual void apply() override;
};

class BleedEffect : public Effect
{
public:
	BleedEffect(Battle* battle, index_t source, index_t target,
		int damageLo, int damageHi, int countTurns);

	virtual void apply() override;

private:
	int damageLo_;
	int damageHi_;
};


// ABILITIES

enum AbilityName {
	AB_ATK,
	AB_USE_ITEM,
	N_ABILITIES
};

typedef std::vector<std::shared_ptr<Effect>> EffectList;

class Ability
{
public:
	virtual ~Ability() {}
	virtual EffectList getEffects(Battle* battle, index_t source, index_t target) = 0;
};

class PhysAttackAbility : public Ability
{
public:
	virtual EffectList getEffects(Battle* battle, index_t source, index_t target) override;
};

class UseItemAbility : public Ability
{
public:
	virtual EffectList getEffects(Battle* battle, index_t source, index_t target) override;
};

// Data Base

struct ItemEffectDef
{
	enum AbilityName ability;
	enum EffectName effect;
};

struct AbilityModifier
{
	enum AbilityName ability;
	enum EffectName effect;
	id_t id;
};

#endif // ABILITY_H__
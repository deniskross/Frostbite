#include "Ability.h"
#include "Battle.h"


Effect::Effect(Battle* battle, index_t source, index_t target, int countTurns)
	: battle_(battle)
	, source_(source)
	, target_(target)
	, countTurns_(countTurns)
{

}

int Effect::tickTurn()
{
	if (countTurns_ > 0) {
		countTurns_ -= 1;
	}

	return countTurns_;
}

PhysDmgEffect::PhysDmgEffect(Battle* battle, index_t source, index_t target, int countTurns)
	: Effect(battle, source, target, countTurns)
{
}

void PhysDmgEffect::apply()
{
	Stats& src = battle_->getCharacter(source_).getStats();
	Stats& tgt = battle_->getCharacter(target_).getStats();

	float srcStr = (float)src.getStat(STRENGHT);
	float tgtStr = (float)tgt.getStat(STRENGHT);

	int damage = (int)(src.getStat(ATTACK) * (srcStr /
		(srcStr + tgt.getStat(DEFENSE) * (tgtStr / srcStr))));



	tgt[HIT_POINTS] = tgt[HIT_POINTS] - damage;
	std::string targetName = battle_->getCharacter(target_).getName();
	std::string sourceName = battle_->getCharacter(source_).getName();


	writef("\n\033[91m{}\033[0m inflicted \033[91m{}\033[0m damage to \033[91m{}\033[0m\n",
		sourceName, damage, targetName);
}

BleedEffect::BleedEffect(Battle* battle, index_t source, index_t target,
	int damageLo, int damageHi, int countTurns)
	: Effect(battle, source, target, countTurns)
	, damageLo_(damageLo)
	, damageHi_(damageHi)
{
}

void BleedEffect::apply()
{
	Stats& src = battle_->getCharacter(source_).getStats();
	Stats& tgt = battle_->getCharacter(target_).getStats();

	float srcStr = (float)src.getStat(STRENGHT);
	float tgtStr = (float)tgt.getStat(STRENGHT);

	int baseDamage = random(damageLo_, damageHi_);
	int damage = (int)(baseDamage * (srcStr / (srcStr + tgt.getStat(DEFENSE) * (tgtStr / srcStr))));

	tgt[HIT_POINTS] = tgt[HIT_POINTS] - damage;
	std::string targetName = battle_->getCharacter(target_).getName();
	std::string sourceName = battle_->getCharacter(source_).getName();


	writef("\033[91m{}\033[0m bleeds for \033[91m{}\033[0m damage\n",
		targetName, damage);
}

EffectList PhysAttackAbility::getEffects(Battle* battle, index_t source, index_t target)
{
	EffectList effects;
	effects.push_back(std::make_shared<PhysDmgEffect>(battle, source, target, 0));

	BattleCharacter& character = battle->getCharacter(source);
	auto& abilityEffects = character.getAbilityModifiers();
	for (auto& effect : abilityEffects) {
		if (effect.ability != AB_ATK) continue;
		switch (effect.effect) {
		case EF_DMG:
			effects.push_back(std::make_shared<PhysDmgEffect>(battle, source, target, 0));
			break;
		case EF_HEAL:
			break;
		case EF_BLEED:
			effects.push_back(std::make_shared<BleedEffect>(battle, source, target,
				3, 7, 3));
			break;
		}
	}
	return effects;
}
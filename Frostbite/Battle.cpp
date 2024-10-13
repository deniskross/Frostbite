#include "Battle.h"
#include "Engine.h"
#include "Character.h"

#include <cassert>
#include <iostream>
#include <algorithm>


BattleCharacter::BattleCharacter(const MonsterDef* def, const Stats& stats)
	: def_(def)
	, stats_(stats)
{
	CharacterAbilityDef pa;
	pa.name = AB_ATK;
	CharacterAbilityDef ua;
	ua.name = AB_USE_ITEM;

	abilities_.push_back(pa);
	abilities_.push_back(ua);
}

Stats& BattleCharacter::getStats()
{
	return stats_;
}

std::string BattleCharacter::getName() const
{
	if (def_->name.size() > 0) {
		return def_->name;
	}
	return "You";
}

bool BattleCharacter::isDead() const
{
	return stats_.getStat(HIT_POINTS) <= 0;
}

const std::vector<CharacterAbilityDef>& BattleCharacter::getAbilities() const
{
	return abilities_;
}

std::vector<AbilityModifier>& BattleCharacter::getAbilityModifiers()
{
	return abilityModifiers_;
}

void BattleCharacter::addAbilityModifier(id_t id, const AbilityModifier& modifier)
{
	abilityModifiers_.push_back(modifier);
}

void BattleCharacter::removeAbilityModifiers(id_t id)
{
	std::remove_if(abilityModifiers_.begin(), abilityModifiers_.end(),
		[id](const AbilityModifier& m) { return m.id == id; });
}

void BattleCharacter::appendEffect(std::shared_ptr<Effect>& effect)
{
	effects_.push_back(effect);
}

void BattleCharacter::onStartTurn()
{
	for (auto& e : effects_) {
		e->apply();
		e->tickTurn();
	}

	std::remove_if(effects_.begin(), effects_.end(),
		[](std::shared_ptr<Effect>& e) { return e->isExpired(); });
}



Battle::Battle(WorldState* state, const MonsterDef* playerDef, const MonsterDef* monsterDef)
	: roundNo_(0)
{
	BattleCharacter player(playerDef, state->getCharacterStats());
	const Equipment& equipment = state->getEquipment();
	for (index_t i = 0; i < isize(equipment); ++i) {
		if (equipment[i].itemId != ID_UNDEF) {
			const Item* item = state->getItemTable().get(equipment[i].itemId);
			const ItemDef* itemDef = g_itemRegistry.getDef(item->defId);
			for (auto& effect : itemDef->effects) {
				AbilityModifier modifier;
				modifier.id = i;
				modifier.ability = effect.ability;
				modifier.effect = effect.effect;
				player.addAbilityModifier(i, modifier);
			}
		}
	}
	chars_.push_back(player);

	BattleCharacter monster(monsterDef, monsterDef->stats);
	chars_.push_back(monster);
}

BattleCharacter& Battle::getCharacter(index_t index)
{
	assert(index >= 0 && index < isize(chars_));
	return chars_[index];
}

void Battle::run()
{
	bool isFinished = false;
	while (!isFinished) {
		nextRound();
		characterTurn();
		monsterTurn();

		isFinished = chars_[0].isDead()
			|| chars_[1].isDead();
	}
}

void Battle::nextRound()
{
	roundNo_ += 1;
	writef("\nRound \033[97m{}\033[0m\n", roundNo_);
}


static const char* ABILITY_NAME[] = {
	"Attack", "Use Item"
};

void Battle::characterTurn()
{
	chars_[0].onStartTurn();

	auto& abilities = chars_[0].getAbilities();

	std::vector <std::string> choices;
	std::transform(
		abilities.begin(),
		abilities.end(),
		std::back_inserter(choices),
		[](const CharacterAbilityDef& abilityDef) { return ABILITY_NAME[abilityDef.name]; });

	index_t choice = dialog("Your turn.", "Action?", choices, {});
	if (choice == 0) {
		std::unique_ptr<Ability> ability(new PhysAttackAbility());
		auto effects = ability->getEffects(this, IDX_PLAYER, 1);
		for (auto& effect : effects) {
			if (effect->isImmediate()) {
				effect->apply();
			}
			else {
				index_t target = effect->getTarget();
				BattleCharacter& character = getCharacter(target);
				character.appendEffect(effect);
			}
		}
	}
}

void Battle::monsterTurn()
{
	chars_[1].onStartTurn();
}

bool BattleTrigger::execute(WorldState& state)
{
	Battle battle(&state, g_monsterRegistry.getDef(0), g_monsterRegistry.getDef(monsterId_));
	battle.run();
	return true;

}

void BattleTrigger::loadFromStream(std::istream& is)
{
	is >> monsterId_;
}
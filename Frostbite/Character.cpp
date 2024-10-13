#include <cassert>
#include "Character.h"

ItemRegistry g_itemRegistry;
MonsterRegistry g_monsterRegistry;

static std::pair<const char*, enum StatName> STAT_NAMES[] = {
	{ "str", STRENGHT }, { "dex", DEXTERITY }, { "atk", ATTACK },
	{ "def", DEFENSE }, { "hp", HIT_POINTS }, { "hp_max", HIT_POINTS_MAX },
	{ "", N_STATS }
};

static std::pair<const char*, enum EffectName> EFFECT_NAMES[] = {
	{ "dmg", EF_DMG }, { "heal", EF_HEAL }, { "bleed", EF_BLEED },
	{ "", N_EFFECTS }
};

static std::pair<const char*, enum AbilityName> ABILITY_NAMES[] = {
	{ "atk", AB_ATK }, { "use_item", AB_USE_ITEM },
	{ "", N_ABILITIES }
};

#define NELEMS(a)   (sizeof(a) / sizeof(a[0]))

template<typename N>
N parse(const char* name, std::pair<const char*, N> kw[], index_t kwCount)
{
	for (index_t i = 0; i < kwCount; ++i) {
		if (_stricmp(name, kw[i].first) == 0) {
			return kw[i].second;
		}
	}
	return kw[kwCount - 1].second;
}

Stats::Stats()
{
	std::fill(stats_.begin(), stats_.end(), 0);
}

Stats::Stats(const std::array<int, N_STATS> stats)
{
	std::copy(stats.begin(), stats.end(), stats_.begin());
}

int& Stats::operator[](enum StatName stat)
{
	assert(stat >= 0 && stat < N_STATS);
	return stats_[stat];
}

int Stats::getStat(enum StatName stat) const
{
	assert(stat >= 0 && stat < N_STATS);

	int result = stats_[stat];
	int multiplier = 100;
	for (auto& m : modifiers_[stat]) {
		result += m.second.add;
		multiplier += m.second.mult;
	}

	return multiplier == 100 ? result : (result * multiplier) / 100;
}

void Stats::addModifier(id_t id, const Modifier& modifier)
{
	modifiers_[modifier.stat][id] = modifier;
}

void Stats::addModifiers(id_t id, const std::vector<Modifier>& modifiers)
{
	for (auto& m : modifiers) {
		addModifier(id, m);
	}
}

void Stats::removeModifiers(id_t id)
{
	for (auto& map : modifiers_) {
		map.erase(id);
	}
}

std::istream& operator>>(std::istream& is, MonsterDef& def)
{
	is >> def.id;
	is.ignore(INT_MAX, '\n');
	getline(is, def.name);
	getline(is, def.description);

	for (int i = 0; i < N_STATS; ++i) {
		is >> def.stats[i];
	}

	index_t count;
	is >> count;
	for (index_t i = 0; i < count; ++i) {
		CharacterAbilityDef abilityDef;
		std::string ability;
		is >> ability;
		abilityDef.name = parse(ability.c_str(), ABILITY_NAMES, NELEMS(ABILITY_NAMES));
		if (abilityDef.name != N_ABILITIES) {
			def.abilities.push_back(abilityDef);
		}
	}

	return is;
}

std::istream& operator>>(std::istream& is, ItemDef& def)
{
	is >> def.id;
	is.ignore(INT_MAX, '\n');
	getline(is, def.name);
	getline(is, def.description);
	is >> def.maxInStack;
	int slot;
	is >> slot;
	def.slot = (enum EquipmentSlotName)slot;

	int count;
	is >> count;
	for (int i = 0; i < count; ++i) {
		ModifierDef modifier;
		std::string stat;
		is >> stat;
		is >> modifier.addLo >> modifier.addHi;
		is >> modifier.multLo >> modifier.multHi;
		modifier.stat = parse(stat.c_str(), STAT_NAMES, NELEMS(STAT_NAMES));
		if (modifier.stat != N_STATS) {
			def.modifiers.push_back(modifier);
		}
	}

	is >> count;
	for (int i = 0; i < count; ++i) {
		ItemEffectDef effectDef;
		std::string ability, effect;
		is >> ability;
		is >> effect;

		effectDef.ability = parse(ability.c_str(), ABILITY_NAMES, NELEMS(ABILITY_NAMES));
		effectDef.effect = parse(effect.c_str(), EFFECT_NAMES, NELEMS(EFFECT_NAMES));

		if (effectDef.ability != N_ABILITIES && effectDef.effect != N_EFFECTS) {
			def.effects.push_back(effectDef);
		}
	}

	return is;
}
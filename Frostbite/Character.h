#pragma once
#ifndef CHARACTER_H__
#define CHARACTER_H__
#include <array>
#include <map>
#include <string>
#include <vector>
#include "Engine.h"
#include "Ability.h"

enum StatName {
    STRENGHT = 0,
    DEXTERITY = 1,
    ATTACK = 2,
    DEFENSE = 3,
    HIT_POINTS = 4,
    HIT_POINTS_MAX = 5,
    N_STATS
};

struct Modifier {
    enum StatName stat;
    int add;
    int mult;
};

class Stats {
public:
    Stats();
    Stats(const std::array<int, N_STATS> stats);
    int& operator[](enum StatName stat);

    // returns modified stat
    int getStat(enum StatName stat) const;

    void addModifier(id_t id, const Modifier& modifier);
    void addModifiers(id_t id, const std::vector<Modifier>& modifiers);
    void removeModifiers(id_t id);

private:
    std::array<int, N_STATS> stats_;
    std::array<std::map<int, Modifier>, N_STATS> modifiers_;
};

struct CharacterAbilityDef
{
    enum AbilityName name;
};

struct MonsterDef {
    id_t id;
    std::string name;
    std::string description;
    std::array<int, N_STATS> stats;
    std::vector<CharacterAbilityDef> abilities;
};

std::istream& operator>>(std::istream& is, MonsterDef& def);

typedef Registry<MonsterDef> MonsterRegistry;
extern MonsterRegistry g_monsterRegistry;

struct ModifierDef {
    enum StatName stat;
    int addLo;  //valuem like + 1
    int addHi;
    int multLo;  //percent like + 10%
    int multHi;
};

struct ItemDef {
    id_t id;
    std::string name;
    std::string description;
    int maxInStack;
    enum EquipmentSlotName slot;
    std::vector<ModifierDef> modifiers;
    std::vector<ItemEffectDef> effects;
};

std::istream& operator>>(std::istream& is, ItemDef& def);

typedef Registry<ItemDef> ItemRegistry;
extern ItemRegistry g_itemRegistry;


struct Item {
    id_t id;
    id_t defId;
    std::string name;
    std::vector<Modifier> modifiers;
};


struct InventoryItem {
    /// id_t of Item in the item table
    id_t itemId;
    int count;
    std::string itemName;

    InventoryItem()
        : itemId(ID_UNDEF)
        , count(0)
    {}

};

//constexpr позволяет вычислять значение переменной на этапе компиляции
constexpr int N_INVENTORY_ITEMS = 8;
typedef std::array<InventoryItem, N_INVENTORY_ITEMS> Inventory;


enum EquipmentSlotName {
    EQ_UNDEF = 0,
    EQ_MAIN_HAND = 1,
    //EQ_OFF_HAND  = 2,
    //EQ_BODY      = 3,
    N_EQUIPMENT_SLOTS
};

struct EquipmentSlot {
    id_t itemId;

    EquipmentSlot()
        : itemId(ID_UNDEF)
    {}
};

typedef std::array<EquipmentSlot, N_EQUIPMENT_SLOTS> Equipment;


#endif
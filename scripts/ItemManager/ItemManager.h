#ifndef ITEMMANAGER_H
#define ITEMMANAGER_H
#include "../stdafx.h"
#include "../ProjectileController/ProjectileController.h"

#define SIZEITEMSPRITE 64

typedef enum
{
	loot_attack,
	loot_defense,
	loot_skill,
	loot_consumable
}TypeItem;


typedef enum
{
	feu,
	eau,
	poison
}Element;

typedef struct
{
	Element element;
	float baseDmg;
	float critPer;
	float critDmg;
	float compDmg;
	float compCdr;
}Attaque;

typedef struct
{
	float staMax;
	float staRegen;
	float staCost;
	float defMax;
	float hpMax;
	float hpRegen;
}Defense;

typedef struct
{
	Attaque attaque;
	Defense defense;
	TypeItem type;
	int ID;
}Item;

struct ItemChaine
{
	struct ItemChaine* next;
	Item item;
	sfVector2f posMap;
	sfBool picked;
};

typedef struct
{
	struct ItemChaine* first;
}Loot_Item;

Item GenerateItem();

void Spawn_Loot(Loot_Item *loot_item, sfVector2f pos);

void Delete_Loot(struct ItemChaine* toDel, Loot_Item *loot_item);

void DeleteAllLoot(Loot_Item* list);

void Render_Loot(Loot_Item loot_item, Layers* layer, ItemSprites sprites);

#endif 
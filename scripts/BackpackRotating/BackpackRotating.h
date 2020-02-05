#ifndef BACKPACKROTATING_H
#define BACKPACKROTATING_H

#include "../stdafx.h"
#include "../TextManager/textManager.h"
#include "../Command/command.h"
#include "../ItemManager/ItemManager.h"

#define QTITY 8

#define INVENTORYTIME 0.75f

#define CASENB 5
#define CASESIZE 84
#define MAXQTITE 64

typedef enum
{
	BP_opening,
	BP_open,
	BP_closing,
	BP_close
}animEtat;

typedef enum
{
	BP_selectSlot,
	BP_selectItem,
	BP_gestion,
	BP_Equip,
	BP_FastEquip
}BP_etat;

// struct BackPack
typedef struct
{
	Item equipable;

	int itemID;
	int quantity;
	TypeItem type;
}BagItem;

typedef struct
{
	BagItem** bag;
}Inventory;

typedef struct
{
	BagItem item;

	sfVector2i posTab;
	sfBool isMoving;
}BackpackSelect;

typedef struct
{
	sfVector2f posengrenageInit;
	sfVector2f posengrenageTarg;
	sfVector2f posdents;
	sfVector2f pos;

	float timeNeeded;
	float time;
	animEtat etat;
}BackpackAnim;

typedef struct
{
	BackpackAnim BPAnim;
	BackpackSelect current;
	Inventory backPack;

	sfVector2f pos;
	sfVector2f selecteur;
	
	sfBool isOpen;
}BackpackDatas;

//struct BackpackRotating
typedef struct
{
	sfVector2f pos;
	int num;
	TypeItem type;
}RotatingBPSelect;

typedef struct
{
	sfVector2f pos;
	sfVector2f cible;
	sfBool moving;
	float time;
}RotatingBPPoints;

typedef struct
{
	RotatingBPPoints engrenages[QTITY];
	RotatingBPSelect posSelect;
	BagItem equiped[8];
	sfVector2f posObjets[QTITY];
	sfVector2f a[3];
	sfVector2f vertical;
	sfVector2f dep;
	sfVector2f debutRot;
	int bagState;
	float angleSize;
	BP_etat etat;
}RotatingBackpackDatas;

float calculAngleSelection(sfVector2f vect, int angleOffset);

sfVector2f Lerp2f(sfVector2f start, sfVector2f target, float time, float timeNeeded);

void RotatingBPPointsUpdate(RotatingBPPoints obj[QTITY], float Dt, float gameTime, int *bagState);

RotatingBPPoints iniRotatingBPPoints(sfVector2f pos, sfVector2f cible);

void initPosGears(RotatingBackpackDatas* data);

RotatingBackpackDatas initRotatingBackpack();

RotatingBPSelect getGearNumber(RotatingBackpackDatas data, RotatingBPSelect select, sfRenderWindow* window);

void BlitGears(RotatingBackpackDatas RBPDatas, RotatingBPSelect posSelect, RotatingBackpackSprite sprites, BackpackSprite spritesItem, sfRenderWindow* window);
#endif // !BACKPACKROTATING_H


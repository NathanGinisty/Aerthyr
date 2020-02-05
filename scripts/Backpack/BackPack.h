#ifndef BACKPACK_H
#define BACKPACK_H


#include "../BackpackRotating/BackpackRotating.h"

BackpackAnim InitBackpackAnim();

void UpdateEngrenage(BackpackAnim* data, float Dt, float needed);

BackpackDatas InitInventory();

void AddBackPackItem2(int ID, int qtite, int type, Inventory* inventory);

void IniBackPack(Inventory* backPack);

void GestionLeftClickInventory(BackpackDatas* inventaire, sfVector2f posCurseur, sfRenderWindow* window);

void DestroyEquipable(Item* item);

void GestionEquip(BackpackDatas* inventaire, RotatingBackpackDatas* rotBP, sfVector2f posCurseur);

sfBool GestionFastEquip(BackpackDatas* inventaire, RotatingBackpackDatas* rotBP, sfVector2f posCurseur);

void BlitBackPack(BackpackDatas inventaire, RotatingBackpackDatas rotBP, BackpackSprite sprites, TextInfo* text, sfRenderWindow* window);

void BlitCurrentBackPackItem(BackpackDatas inventaire, RotatingBackpackDatas rotBP, BackpackSprite sprites, TextInfo* text, sfRenderWindow* window);

void DeleteCurrentItem(BackpackDatas* inventaire);

void MoveBackpackSelect(Command command, BackpackDatas* inventaire);

void Catch_Loot(Loot_Item* loot_item, sfVector2f posPlayer, Inventory* playerInv);

void InventoryDisplay(BackpackDatas inventaire, RotatingBackpackDatas rotatingBP, BackpackAnim BPAnim, SpritePack spritePack, TextInfo* text, sfRenderWindow* window);
#endif // !BACKPACK_H


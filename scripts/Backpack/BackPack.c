#include "BackPack.h"



BackpackAnim InitBackpackAnim()
{
	BackpackAnim temp = { 0 };
	temp.posengrenageInit = (sfVector2f) { 65 + 15, 1080 + 68 };
	temp.posengrenageTarg = (sfVector2f) { temp.posengrenageInit.x, temp.posengrenageInit.y - 510 };
	temp.pos = temp.posengrenageInit;
	temp.timeNeeded = 1;
	temp.etat = BP_close;

	temp.posdents = (sfVector2f) { 0, 1080 - 527 };
	return temp;
}

void UpdateEngrenage(BackpackAnim* data, float Dt, float needed)
{
	sfVector2f posengrenageInit = { 65 + 15, 1080 + 68 };
	sfVector2f posengrenageTarg = { posengrenageInit.x, posengrenageInit.y - 515 };
	if (data->etat == BP_opening || data->etat == BP_closing)
	{
		data->time += Dt;
	}

	if (data->etat == BP_closing)
	{
		data->pos = lerpVec2f(posengrenageInit, posengrenageTarg, data->time, needed);
		if (data->time > data->timeNeeded)
		{
			data->etat = BP_open;
		}
	}
	if (data->etat == BP_opening)
	{
		data->pos = lerpVec2f(posengrenageTarg, posengrenageInit, data->time, needed);
		if (data->time > data->timeNeeded)
		{
			data->etat = BP_close;
		}
	}


}

BackpackDatas InitInventory()
{
	BackpackDatas tempInv;
	IniBackPack(&tempInv.backPack);
	tempInv.isOpen = sfFalse;
	tempInv.pos = (sfVector2f) { 84, 588 };
	tempInv.selecteur = (sfVector2f) { 93 + CASESIZE / 2, 645 + CASESIZE / 2 };

	tempInv.current = (BackpackSelect) { sfFalse, -1 };
	return tempInv;
}

void AddBackPackItem2(int ID, int qtite, int type, Inventory* inventory)
{
	sfBool fullInventory = sfTrue;
	sfBool Break = sfFalse;
	for (int j = 0; j < CASENB; j++)
	{
		for (int i = 0; i < CASENB; i++)
		{
			if ((inventory->bag[i][j].itemID == ID && inventory->bag[i][j].quantity < MAXQTITE))
			{
				if (inventory->bag[i][j].quantity + qtite <= MAXQTITE)
				{
					inventory->bag[i][j].quantity += qtite;
					inventory->bag[i][j].type = type;
				}
				else if (inventory->bag[i][j].quantity + qtite > MAXQTITE)
				{
					int total = inventory->bag[i][j].quantity + qtite;
					int diff = total - MAXQTITE;
					inventory->bag[i][j].quantity = MAXQTITE;
					AddBackPackItem2(ID, diff, type, inventory);
				}
				fullInventory = sfFalse;
				i = CASENB;
				j = CASENB;
				Break = sfTrue;
			}
		}
	}
	if (!Break)
	{
		for (int j = 0; j < CASENB; j++)
		{
			for (int i = 0; i < CASENB; i++)
			{
				if (inventory->bag[i][j].itemID == -1)
				{
					if (qtite <= MAXQTITE)
					{
						inventory->bag[i][j].quantity = qtite;
						inventory->bag[i][j].itemID = ID;
						inventory->bag[i][j].type = type;
					}
					else
					{
						int diff = qtite - MAXQTITE;
						inventory->bag[i][j].itemID = ID;
						inventory->bag[i][j].quantity = MAXQTITE;
						AddBackPackItem2(ID, diff, type, inventory);
					}
					fullInventory = sfFalse;
					i = CASENB;
					j = CASENB;
				}
			}
		}
	}

	if (fullInventory)
	{
		printf("sac plein\n");
	}
}

void IniBackPack(Inventory* backPack)
{
	backPack->bag = (BagItem**)malloc(CASENB * sizeof(BagItem*));
	for (int i = 0; i < CASENB; i++)
	{
		backPack->bag[i] = (BagItem*)malloc(CASENB * sizeof(BagItem));
	}

	//init les ID a -1
	for (int i = 0; i < CASENB; i++)
	{
		for (int j = 0; j < CASENB; j++)
		{
			backPack->bag[i][j].itemID = -1;
			backPack->bag[i][j].quantity = 0;
		}
	}
}

void GestionLeftClickInventory(BackpackDatas* inventaire, sfVector2f posCurseur, sfRenderWindow* window)
{
	// BP_gestion des items
	sfVector2i mouse = (sfVector2i) { posCurseur.x, posCurseur.y };
	if (inventaire->isOpen && mouse.x > inventaire->pos.x && mouse.x < inventaire->pos.x + CASENB * CASESIZE &&
		mouse.y > inventaire->pos.y && mouse.y < inventaire->pos.y + CASENB * CASESIZE)
	{
		mouse.x = (mouse.x - inventaire->pos.x) / CASESIZE;
		mouse.y = (mouse.y - inventaire->pos.y) / CASESIZE;
		//quand on clique sur un item et qu on a rien en current
		if (inventaire->backPack.bag[mouse.x][mouse.y].itemID != -1 && inventaire->current.isMoving == sfFalse)
		{
			inventaire->current.isMoving = sfTrue;
			inventaire->current.item = inventaire->backPack.bag[mouse.x][mouse.y];
			inventaire->backPack.bag[mouse.x][mouse.y].itemID = -1;
			inventaire->backPack.bag[mouse.x][mouse.y].quantity = 0;
			inventaire->current.posTab = mouse;
		}
		//quand on clique sur un bloc full stack et qu on a un current
		else if (inventaire->backPack.bag[mouse.x][mouse.y].quantity == MAXQTITE && inventaire->current.isMoving == sfTrue)
		{
			BagItem provi = inventaire->current.item;
			inventaire->current.item = inventaire->backPack.bag[mouse.x][mouse.y];
			inventaire->backPack.bag[mouse.x][mouse.y] = provi;
		}
		//quand on complete un item avec le current
		else if (inventaire->backPack.bag[mouse.x][mouse.y].itemID == inventaire->current.item.itemID &&
			inventaire->backPack.bag[mouse.x][mouse.y].quantity + inventaire->current.item.quantity <= MAXQTITE)
		{
			inventaire->backPack.bag[mouse.x][mouse.y].quantity += inventaire->current.item.quantity;
			inventaire->current.isMoving = sfFalse;

		}
		//quand on complete un item avec le current et que c est > max
		else if (inventaire->backPack.bag[mouse.x][mouse.y].itemID == inventaire->current.item.itemID &&
			inventaire->backPack.bag[mouse.x][mouse.y].quantity + inventaire->current.item.quantity > MAXQTITE)
		{
			int provi = inventaire->backPack.bag[mouse.x][mouse.y].quantity + inventaire->current.item.quantity;
			inventaire->backPack.bag[mouse.x][mouse.y].quantity = MAXQTITE;
			AddBackPackItem2(inventaire->backPack.bag[mouse.x][mouse.y].itemID, provi % MAXQTITE, inventaire->backPack.bag[mouse.x][mouse.y].type, &inventaire->backPack);
			DeleteCurrentItem(inventaire);
		}
		//echnage de 2 items
		else if (inventaire->backPack.bag[mouse.x][mouse.y].itemID != -1 && inventaire->current.isMoving == sfTrue)
		{
			BagItem provi = inventaire->current.item;
			inventaire->current.item = inventaire->backPack.bag[mouse.x][mouse.y];
			inventaire->backPack.bag[mouse.x][mouse.y] = provi;
		}

		else
		{
			inventaire->backPack.bag[mouse.x][mouse.y] = inventaire->current.item;
			DeleteCurrentItem(inventaire);
		}
	}
}

void DestroyEquipable(Item* item)
{
	switch (item->type)
	{
	case loot_attack: item->attaque.baseDmg = -1;
		item->attaque.compCdr = -1;
		item->attaque.compDmg = -1;
		item->attaque.critDmg = -1;
		item->attaque.critPer = -1;
		item->attaque.element = -1;
		break;

	default: loot_defense: item->defense.defMax = -1;
		item->defense.hpMax = -1;
		item->defense.hpRegen = -1;
		item->defense.staCost = -1;
		item->defense.staMax = -1;
		item->defense.staRegen = -1;
		break;
	}
}

void GestionEquip(BackpackDatas* inventaire, RotatingBackpackDatas* rotBP, sfVector2f posCurseur)
{
	sfVector2i mouse = (sfVector2i) { posCurseur.x, posCurseur.y };
	if (inventaire->isOpen && mouse.x > inventaire->pos.x && mouse.x < inventaire->pos.x + CASENB * CASESIZE &&
		mouse.y > inventaire->pos.y && mouse.y < inventaire->pos.y + CASENB * CASESIZE)
	{
		mouse.x = (mouse.x - inventaire->pos.x) / CASESIZE;
		mouse.y = (mouse.y - inventaire->pos.y) / CASESIZE;

		if (inventaire->backPack.bag[mouse.x][mouse.y].type == rotBP->posSelect.type && rotBP->equiped[rotBP->posSelect.num].itemID == -1)
		{
			rotBP->equiped[rotBP->posSelect.num].itemID = inventaire->backPack.bag[mouse.x][mouse.y].itemID;
			rotBP->equiped[rotBP->posSelect.num].type = inventaire->backPack.bag[mouse.x][mouse.y].type;
			rotBP->equiped[rotBP->posSelect.num].equipable = inventaire->backPack.bag[mouse.x][mouse.y].equipable;

			inventaire->backPack.bag[mouse.x][mouse.y].itemID = -1;
			inventaire->backPack.bag[mouse.x][mouse.y].type = -1;
			DestroyEquipable(&inventaire->backPack.bag[mouse.x][mouse.y].equipable);
		}
		else if (inventaire->backPack.bag[mouse.x][mouse.y].type == rotBP->posSelect.type && rotBP->equiped[rotBP->posSelect.num].itemID != -1)
		{
			BagItem provi = rotBP->equiped[rotBP->posSelect.num];
			rotBP->equiped[rotBP->posSelect.num].itemID = inventaire->backPack.bag[mouse.x][mouse.y].itemID;
			rotBP->equiped[rotBP->posSelect.num].type = inventaire->backPack.bag[mouse.x][mouse.y].type;
			rotBP->equiped[rotBP->posSelect.num].equipable = inventaire->backPack.bag[mouse.x][mouse.y].equipable;

			inventaire->backPack.bag[mouse.x][mouse.y].itemID = provi.itemID;
			inventaire->backPack.bag[mouse.x][mouse.y].type = provi.type;
			inventaire->backPack.bag[mouse.x][mouse.y].equipable = provi.equipable;
		}
		else if (inventaire->backPack.bag[mouse.x][mouse.y].itemID == -1)
		{
			inventaire->backPack.bag[mouse.x][mouse.y].itemID = rotBP->equiped[rotBP->posSelect.num].itemID;
			inventaire->backPack.bag[mouse.x][mouse.y].type = rotBP->equiped[rotBP->posSelect.num].type;
			inventaire->backPack.bag[mouse.x][mouse.y].equipable = rotBP->equiped[rotBP->posSelect.num].equipable;

			rotBP->equiped[rotBP->posSelect.num].itemID = -1;
			rotBP->equiped[rotBP->posSelect.num].type = -1;
			DestroyEquipable(&rotBP->equiped[rotBP->posSelect.num].equipable);
		}
	}
}

sfBool GestionFastEquip(BackpackDatas* inventaire, RotatingBackpackDatas* rotBP, sfVector2f posCurseur)
{
	sfVector2i mouse = (sfVector2i) { posCurseur.x, posCurseur.y };
	if (inventaire->isOpen && mouse.x > inventaire->pos.x && mouse.x < inventaire->pos.x + CASENB * CASESIZE &&
		mouse.y > inventaire->pos.y && mouse.y < inventaire->pos.y + CASENB * CASESIZE)
	{
		mouse.x = (mouse.x - inventaire->pos.x) / CASESIZE;
		mouse.y = (mouse.y - inventaire->pos.y) / CASESIZE;


		sfVector2i bornes = { -1, -1 };
		int boucle = 0;
		sfBool full = sfTrue;
		switch (inventaire->backPack.bag[mouse.x][mouse.y].type)
		{
		case loot_defense: bornes.x = 0; bornes.y = 2;
			break;
		case loot_skill:bornes.x = 3; bornes.y = 4;
			break;
		case loot_attack:bornes.x = 5; bornes.y = 7;
			break;
		case loot_consumable:bornes.x = -1; bornes.y = -1;
			break;
		}
		if (inventaire->backPack.bag[mouse.x][mouse.y].type != loot_skill)
		{
			boucle = 3;
		}
		else
		{
			boucle = 2;
		}
		if (bornes.x != -1 && bornes.y != -1)
		{
			for (int i = 0; i < boucle; i++)
			{
				if (rotBP->equiped[bornes.x + i].itemID == -1)
				{
					rotBP->equiped[bornes.x + i].itemID = inventaire->backPack.bag[mouse.x][mouse.y].itemID;
					rotBP->equiped[bornes.x + i].type = inventaire->backPack.bag[mouse.x][mouse.y].type;
					rotBP->equiped[bornes.x + i].equipable = inventaire->backPack.bag[mouse.x][mouse.y].equipable;

					inventaire->backPack.bag[mouse.x][mouse.y].itemID = -1;
					inventaire->backPack.bag[mouse.x][mouse.y].type = -1;
					DestroyEquipable(&inventaire->backPack.bag[mouse.x][mouse.y].equipable);
					full = sfFalse;
					i = 4;
				}
			}
		}
		return full;
	}
}

void BlitBackPack(BackpackDatas inventaire, RotatingBackpackDatas rotBP, BackpackSprite sprites, TextInfo* text, sfRenderWindow* window)
{
	int decalText = 68;
	int add = 0;
	for (int i = 0; i < CASENB; i++)
	{
		for (int j = 0; j < CASENB; j++)
		{
			if (inventaire.backPack.bag[i][j].itemID != -1)
			{
				sfVector2f tempPos = { (float)(i * CASESIZE + inventaire.pos.x),(float)(j * CASESIZE + inventaire.pos.y) };
				if (sprites.items[inventaire.backPack.bag[i][j].itemID] != NULL)
				{

					if (inventaire.backPack.bag[i][j].type == loot_defense)
					{
						add = 6;
					}
					else if (inventaire.backPack.bag[i][j].type == loot_skill)
					{
						add = 12;
					}
					else
					{
						add = 0;
					}
					BlitSprite(sprites.items[inventaire.backPack.bag[i][j].itemID + add], tempPos, window);
				}
			}
		}
	}
	// affichage du texte __________________________
	for (int i = 0; i < CASENB; i++)
	{
		for (int j = 0; j < CASENB; j++)
		{
			if (inventaire.backPack.bag[i][j].type == loot_consumable)
			{
				if (inventaire.backPack.bag[i][j].itemID != -1)
				{
					sfVector2f posProvi = { (float)(i * CASESIZE + decalText + inventaire.pos.x),(float)(j * CASESIZE + decalText + inventaire.pos.y) };
					char provi[10];
					_itoa(inventaire.backPack.bag[i][j].quantity, provi, 10);
					BlitText(text, posProvi, 10, provi, sfWhite, window);
				}
			}
		}
	}

}

void BlitCurrentBackPackItem(BackpackDatas inventaire, RotatingBackpackDatas rotBP, BackpackSprite sprites, TextInfo* text, sfRenderWindow* window)
{
	int decalText = 68;
	//affiche celui qui bouge par dessus les autres + selecteur
	if (rotBP.etat == BP_selectItem || rotBP.etat == BP_gestion)
	{
		if (inventaire.current.isMoving == sfTrue)
		{
			int add = 0;
			if (inventaire.current.item.type == loot_defense)
			{
				add = 6;
			}
			else if (inventaire.current.item.type == loot_skill)
			{
				add = 12;
			}
			else
			{
				add = 0;
			}
			sfVector2f temp = { inventaire.pos.x + inventaire.current.posTab.x * CASESIZE , inventaire.pos.y + inventaire.current.posTab.y * CASESIZE };
			BlitSprite(sprites.items[inventaire.current.item.itemID + add], temp, window);
			char provi[10];
			if (inventaire.current.item.quantity > MAXQTITE)
			{
				inventaire.current.item.quantity = MAXQTITE;
			}
			_itoa(inventaire.current.item.quantity, provi, 10);
			if (inventaire.current.item.type == loot_consumable)
			{
				BlitText(text, (sfVector2f) { (float)(temp.x + decalText), (float)(temp.y + decalText) }, 12, provi, sfWhite, window);
			}
		}
	}
	if ((rotBP.etat == BP_selectItem || rotBP.etat == BP_gestion) && rotBP.bagState == BP_open)
	{
		BlitSprite(sprites.selecteur, inventaire.selecteur, window);
	}
}

void DeleteCurrentItem(BackpackDatas* inventaire)
{
	inventaire->current.item.itemID = -1;
	inventaire->current.item.quantity = 0;
	inventaire->current.isMoving = sfFalse;
}

void MoveBackpackSelect(Command command, BackpackDatas* inventaire)
{
	if (command.left.pressed)
	{
		if (inventaire->selecteur.x - CASESIZE > inventaire->pos.x)
		{
			inventaire->selecteur.x -= CASESIZE;
		}
	}
	if (command.right.pressed)
	{
		if (inventaire->selecteur.x + CASESIZE < inventaire->pos.x + 5 * CASESIZE)
		{
			inventaire->selecteur.x += CASESIZE;
		}
	}
	if (command.up.pressed)
	{
		if (inventaire->selecteur.y - CASESIZE > inventaire->pos.y)
		{
			inventaire->selecteur.y -= CASESIZE;
		}
	}
	if (command.down.pressed)
	{
		if (inventaire->selecteur.y + CASESIZE < inventaire->pos.y + 5 * CASESIZE)
		{
			inventaire->selecteur.y += CASESIZE;
		}
	}
}


sfVector2i TrouvePremiereCaseVide(Inventory* inventory)
{
	sfVector2i ret = { -1,-1 };
	for (int j = 0; j < CASENB; j++)
	{
		for (int i = 0; i < CASENB; i++)
		{
			if (inventory->bag[i][j].itemID == -1)
			{
				ret = (sfVector2i) { i, j };
				i = CASENB;
				j = CASENB;
			}
		}
	}
	return ret;
}

void Catch_Loot(Loot_Item* loot_item, sfVector2f posPlayer, Inventory* playerInv)
{
	if (loot_item->first != NULL)
	{
		struct ItemChaine* temp = loot_item->first;
		struct ItemChaine* save = temp->next;

		while (temp != NULL)
		{
			float dist = getDistance(temp->posMap, posPlayer);
			if (dist <= SIZE_TILE * 1.5f/*TestPosDansCarre(temp->posMap, SIZEITEMSPRITE, posPlayer)*/)
			{
				sfVector2i test = TrouvePremiereCaseVide(playerInv);
				if (test.x != -1 && test.y != -1)
				{
					playerInv->bag[test.x][test.y].equipable = temp->item;
					playerInv->bag[test.x][test.y].type = temp->item.type;
					playerInv->bag[test.x][test.y].itemID = temp->item.ID;
					Delete_Loot(temp, loot_item);
					temp = save;
				}
			}
			if (temp != NULL)
			{
				save = temp;
				temp = temp->next;
			}
		}
	}
}

void InventoryDisplay(BackpackDatas inventaire, RotatingBackpackDatas rotatingBP, BackpackAnim BPAnim, SpritePack spritePack, TextInfo* text, sfRenderWindow* window)
{
	if (rotatingBP.bagState != BP_close)
	{

		BlitSprite(spritePack.inventorySprites.BPSprites.engrenage, BPAnim.pos, window);
		BlitSprite(spritePack.inventorySprites.BPSprites.engrenage, (sfVector2f) { BPAnim.pos.x, BPAnim.pos.y + 446 }, window);
		BlitSprite(spritePack.inventorySprites.BPSprites.fondGrille, BPAnim.pos, window);

		BlitBackPack(inventaire, rotatingBP, spritePack.inventorySprites.BPSprites, text, window);

		BlitSprite(spritePack.inventorySprites.BPSprites.dents, BPAnim.posdents, window);


		BlitSprite(spritePack.inventorySprites.BPSprites.grille, BPAnim.pos, window);

		BlitCurrentBackPackItem(inventaire, rotatingBP, spritePack.inventorySprites.BPSprites, text, window);
	}

	BlitGears(rotatingBP, rotatingBP.posSelect, spritePack.inventorySprites.rotattingBPSprite, spritePack.inventorySprites.BPSprites, window);


}
#include "ItemManager.h"

Item GenerateItem()
{
	int testType = rand() % 3;
	Item tmpItem = { 0 };

	if (testType == 0)
	{
		int test = rand() % 6;
		switch (test)
		{
		case 0: tmpItem.attaque.element = rand() % 4;
			break;
		case 1: tmpItem.attaque.baseDmg = rand() % 20;
			break;
		case 2: tmpItem.attaque.critPer = rand() % 20;
			break;
		case 3: tmpItem.attaque.critDmg = rand() % 3;
			break;
		case 4:  tmpItem.attaque.compDmg = rand() % 20;
			break;
		case 5: tmpItem.attaque.compCdr = rand() % 20;
			break;
		default:
			break;
		}

		tmpItem.type = loot_attack;
		tmpItem.ID = test;
	}
	else if (testType == 1)
	{
		int test = rand() % 6;
		switch (test)
		{
		case 0: tmpItem.defense.staMax = rand() % 10;
			break;
		case 1: tmpItem.defense.staRegen = rand() % 10;
			break;
		case 2: tmpItem.defense.staCost = rand() % 10;
			break;
		case 3: tmpItem.defense.defMax = rand() % 10;
			break;
		case 4: tmpItem.defense.hpMax = rand() % 10;
			break;
		case 5: tmpItem.defense.hpRegen = rand() % 10;
			break;
		default:
			break;
		}

		tmpItem.type = loot_defense;
		tmpItem.ID = test;
	}
	else
	{
		int test = rand() % 3;

		tmpItem.type = loot_skill;
		tmpItem.ID = test;
	}

	return tmpItem;
}

void Spawn_Loot(Loot_Item *loot_item, sfVector2f pos)
{
	struct ItemChaine* temp = (struct ItemChaine*) calloc(sizeof(struct ItemChaine), 1);
	temp->item = GenerateItem();
	temp->posMap = (sfVector2f) {pos.x, pos.y - 50};

	//Insertion de l'élément
	temp->next = loot_item->first;
	loot_item->first = temp;
}

void Delete_Loot(struct ItemChaine* toDel, Loot_Item *loot_item)
{
	struct ItemChaine* tmpItem = loot_item->first;

	if (tmpItem == toDel) //Cas particulier du premier élément
	{
		loot_item->first = toDel->next;
		free(toDel);
	}
	else //Si ce n'est pas le premier élément
	{
		while (tmpItem != 0) //On cherche l'élément précédent celui qu'on veut supprimer
		{
			if (tmpItem->next == toDel)
			{
				struct ItemChaine* next = tmpItem->next;
				tmpItem->next = next->next; //On rebranche par dessus
				free(toDel);	//Puis le fait sauter
				return;
			}
			tmpItem = tmpItem->next;
		}
	}
}

void DeleteAllLoot(Loot_Item* list)
{
	if (list->first != NULL)
	{
		struct ItemChaine* tmp = list->first;
		struct ItemChaine* save = tmp->next;

		while (tmp != NULL)
		{
			if (tmp != NULL)
			{
				Delete_Loot(tmp, list);
				tmp = save;
			}

			if (tmp != NULL)
			{
				save = tmp;
				tmp = tmp->next;
			}
		}
	}
}

void Render_Loot(Loot_Item loot_item, Layers* layer, ItemSprites sprites)
{
	struct ItemChaine* temp = loot_item.first;

	while (temp != NULL)
	{
		if (temp->item.type == loot_attack)
		{
			addToLayer(&layer[LAYER_ITEMS].premier, &sprites.items[temp->item.ID], temp->posMap, DEFAULT_INFOBLIT);
		}
		else if (temp->item.type == loot_defense)
		{
			addToLayer(&layer[LAYER_ITEMS].premier, &sprites.items[temp->item.ID + 6], temp->posMap, DEFAULT_INFOBLIT);
		}
		else
		{
			addToLayer(&layer[LAYER_ITEMS].premier, &sprites.items[temp->item.ID + 12], temp->posMap, DEFAULT_INFOBLIT);
		}
		temp = temp->next;
	}
}



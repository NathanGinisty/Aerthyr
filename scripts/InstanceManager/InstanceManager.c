#include "InstanceManager.h"

void UpdateInstance(struct Enemy** enemies, Loot_Item* itemList, Player* player, EnemiesAnimsInfos e_AnimsInfos, InstanceMaps* instMaps, Fade* fade, ParallaxObjectPack* paralax)
{
	if (*enemies == NULL && fade->infos.changeTo != menu)
	{
		if (instMaps->actualMap < instMaps->mapsNb -1)
		{
			int idMap;
			instMaps->actualMap ++;
			idMap = instMaps->actualMap;
			LoadTiledEnemy(instMaps->maps[idMap], 1, enemies, &player->entity2D.middlePos, e_AnimsInfos);
			*player = InitPlayer();
			DeleteAllParallaxObject(paralax);
			DeleteAllLoot(itemList);
			sfVector2f parallaxMaxPos = (sfVector2f) { (instMaps->maps[idMap].width + 10) * SIZE_TILE, (instMaps->maps[idMap].height + 10) * SIZE_TILE };
			CreateParallax(paralax, (sfVector2f) { -SIZE_TILE * 10, -SIZE_TILE * 10 }, (sfVector2f) { parallaxMaxPos.x, parallaxMaxPos.y });
		}
		else
		{
			fade->infos.changeTo = menu;
			fade->infos.state = F_fadeIn;
			DeleteAllParallaxObject(paralax);
			DeleteAllLoot(itemList);
		}
	}
	else if (player->stats.hpActual <= 0)
	{
		fade->infos.changeTo = menu;
		fade->infos.state = F_fadeIn;
		RemoveAllEnemies(enemies);
		DeleteAllParallaxObject(paralax);
		DeleteAllLoot(itemList);
	}
}


void RenderMapCollider(CameraViews camera, InstanceMaps instance, CollisionMap collisionMap, sfRenderWindow* window)
{
	int jMin = (int)((camera.pos.x - SIZE_TILE * 2 - camera.size.x / 2) / SIZE_TILE);
	int jMax = (int)((camera.pos.x + SIZE_TILE * 2 + camera.size.x / 2) / SIZE_TILE);
	if (jMin < 0)
	{
		jMin = 0;
	}
	if (jMax > instance.maps[instance.actualMap].width)
	{
		jMax = instance.maps[instance.actualMap].width;
	}

	int iMin = (int)((camera.pos.y - SIZE_TILE * 2 - camera.size.y / 2) / SIZE_TILE);
	int iMax = (int)((camera.pos.y + SIZE_TILE * 2 + camera.size.y / 2) / SIZE_TILE);
	if (iMin < 0)
	{
		iMin = 0;
	}
	if (iMax > instance.maps[instance.actualMap].height)
	{
		iMax = instance.maps[instance.actualMap].height;
	}


	int tmpID = -1;

	for (int i = iMin; i < iMax; i++)
	{
		for (int j = jMin; j < jMax; j++)
		{
			sfVector2f tmpPos = { (float)(j * SIZE_TILE),(float)(i * SIZE_TILE) };
			//CENTER use backSprite[0]

			tmpID = instance.maps[instance.actualMap].colliderID[j][i];
			if (tmpID != -1)
			{
				for (int k = 0; k < collisionMap.prefabs[tmpID].nbLine; k++)
				{
					sfVector2f A = collisionMap.prefabs[tmpID].line[k].pointA;
					A.x += SIZE_TILE * j;
					A.y += SIZE_TILE * i;
					sfVector2f B = collisionMap.prefabs[tmpID].line[k].pointB;
					B.x += SIZE_TILE * j;
					B.y += SIZE_TILE * i;
					BlitSegment(A, B, sfRed, window);
				}
			}
		}
	}
}

void RenderMapInstance(CameraViews camera, MapSprites mapSprites, Layers layers[], SpritePack spritePack, InstanceMaps instance, int indexMap)
{
	int jMin = (int)((camera.pos.x - SIZE_TILE * 2 - camera.size.x / 2) / SIZE_TILE);
	int jMax = (int)((camera.pos.x + SIZE_TILE * 2 + camera.size.x / 2) / SIZE_TILE);
	if (jMin < 0)
	{
		jMin = 0;
	}
	if (jMax > instance.maps[indexMap].width)
	{
		jMax = instance.maps[indexMap].width;
	}

	int iMin = (int)((camera.pos.y - SIZE_TILE * 2 - camera.size.y / 2) / SIZE_TILE);
	int iMax = (int)((camera.pos.y + SIZE_TILE * 2 + camera.size.y / 2) / SIZE_TILE);
	if (iMin < 0)
	{
		iMin = 0;
	}
	if (iMax > instance.maps[indexMap].height)
	{
		iMax = instance.maps[indexMap].height;
	}


	int tmpID = -1;

	for (int i = iMin; i < iMax; i++)
	{
		for (int j = jMin; j < jMax; j++)
		{
			sfVector2f tmpPos = { (float)(j * SIZE_TILE),(float)(i * SIZE_TILE) };
			//CENTER use backSprite[0]

			tmpID = instance.maps[indexMap].assetBack[2][j][i];
			if (tmpID != -1)
			{
				addToLayer(&layers[54].premier, &mapSprites.Tileset_Bloc[0][tmpID], tmpPos, DEFAULT_INFOBLIT);
			}


			//BACK
			tmpID = instance.maps[indexMap].assetBack[0][j][i];
			if (tmpID != -1)
			{
				addToLayer(&layers[50].premier, &mapSprites.Tileset_Bloc[0][tmpID], tmpPos, DEFAULT_INFOBLIT);
			}

			tmpID = instance.maps[indexMap].assetBack[1][j][i];
			if (tmpID != -1)
			{
				addToLayer(&layers[52].premier, &mapSprites.Tileset_Front[0][tmpID], tmpPos, DEFAULT_INFOBLIT);
			}

			tmpID = instance.maps[indexMap].assetBack[3][j][i];
			if (tmpID != -1)
			{
				addToLayer(&layers[56].premier, &mapSprites.Tileset_Back[0][tmpID], tmpPos, DEFAULT_INFOBLIT);
			}


			tmpID = instance.maps[indexMap].assetBack[4][j][i];
			if (tmpID != -1)
			{
				addToLayer(&layers[58].premier, &mapSprites.Tileset_Back[0][tmpID], tmpPos, DEFAULT_INFOBLIT);
			}

			//FRONT

			tmpID = instance.maps[indexMap].assetFront[0][j][i];
			if (tmpID != -1)
			{
				addToLayer(&layers[22].premier, &mapSprites.Tileset_Front[0][tmpID], tmpPos, DEFAULT_INFOBLIT);
			}

			tmpID = instance.maps[indexMap].assetFront[1][j][i];
			if (tmpID != -1)
			{
				addToLayer(&layers[20].premier, &mapSprites.Tileset_Front[0][tmpID], tmpPos, DEFAULT_INFOBLIT);
			}

		}
	}

	//render Image
	ImageTile* imageTmp;
	//BACK
	imageTmp = instance.maps[indexMap].backImage.first;

	while (imageTmp != NULL)
	{
		addToLayer(&layers[imageTmp->layer].premier, &spritePack.tileImage.sprite[imageTmp->imageID], imageTmp->pos, DEFAULT_INFOBLIT);
		imageTmp = imageTmp->ptrNext;
	}
	//FRONT
	imageTmp = instance.maps[indexMap].frontImage.first;

	while (imageTmp != NULL)
	{
		addToLayer(&layers[imageTmp->layer].premier, &spritePack.tileImage.sprite[imageTmp->imageID], imageTmp->pos, DEFAULT_INFOBLIT);
		imageTmp = imageTmp->ptrNext;
	}
}
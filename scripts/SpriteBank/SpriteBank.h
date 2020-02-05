#ifndef SPRITE_BANK_H
#define SPRITE_BANK_H

#include <stdlib.h>
#include <stdio.h>
#include "SFML\Graphics.h"
#include "../AnimManager/animManager.h"

typedef struct
{
	sfSprite* **Tileset_Back;
	sfSprite* **Tileset_Bloc;
	sfSprite* **Tileset_Front;
}MapSprites;

typedef struct
{
	sfSprite* background[5];
	sfSprite*** isle;
	sfSprite** cloud;
} ParallaxSprite;

typedef struct
{
	sfSprite* engrenages[8];
	sfSprite* selecteurs;
	sfSprite* grosEngrenage;
}RotatingBackpackSprite;

typedef struct
{
	sfSprite** items;
}ItemSprites;

typedef struct
{
	sfSprite* selecteur;
	sfSprite* items[18];
	sfSprite* dents;
	sfSprite* engrenage;
	sfSprite* grille;
	sfSprite* fondGrille;
}BackpackSprite;

typedef struct
{
	sfSprite* smoke[1];
	sfSprite* star[1];
	sfSprite* heal[1];
} ParticleSprite;

typedef struct
{
	RotatingBackpackSprite rotattingBPSprite;
	BackpackSprite BPSprites;
}InventorySprites;

typedef struct
{
	sfSprite** sprite;
	int nbSprite;
}TileImage;

typedef struct
{
	AnimSprite* player;
	AnimSprite** enemiesSprites;
	ParallaxSprite parallax;
	ItemSprites items;
	InventorySprites inventorySprites;
	ParticleSprite particle;
	sfSprite* projectiles[3];
	sfSprite* cursorMenu;
	sfSprite* shadow;
	TileImage tileImage;
} SpritePack;

MapSprites LoadMapSprites();

RotatingBackpackSprite LoadRotatingBPSprite();

BackpackSprite LoadBPSprite();

ItemSprites InitItemSprites();

TileImage LoadTileSprite();

#endif // !SPRITE_BANK_H
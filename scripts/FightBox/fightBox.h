#ifndef FIGHTBOX_H
#define FIGHTBOX_H
#include "../stdafx.h"
#include "../Player/Player.h"
#include "../EnemyController/EnemyController.h"

typedef enum
{
	circle,
	rect,
	noBox
}TypeHitBox;

typedef struct
{
	Circle circle;
	sfFloatRect rect;
	TypeHitBox type;
}HitBox;

typedef struct
{
	HitBox damageHitBox;
	HitBox bodyHitBox;
}FightBox;

typedef struct
{
	FightBox** fightBox;
}EnemyFightBox;

typedef struct
{
	FightBox ** player;
	EnemyFightBox enemies[3];
	FightBox projectiles[3];
}FightBoxPack;

FightBoxPack LoadFightBox();
void RenderFightBoxDebug(FightBox fightBox, sfVector2f posEntity, sfVector2f OriginSprite, int dir, sfRenderWindow* window);
void UpdateFightBox(FightBoxPack fightBoxPack, Player* player, Enemy** e_first, Projectile** proj_first, SpritePack spritePack);

#endif // !FIGHTBOX_H
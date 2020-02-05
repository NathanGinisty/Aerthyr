#ifndef PARALLAX_H
#define PARALLAX_H

#include "../stdafx.h"
#include "../Camera/camera.h"

#define PARALLAXDECAL 10

typedef enum
{
	isle,
	cloud
} ParallaxObjectType;

typedef enum
{
	Prx_object,
	Prx_light
} ParallaxSpriteType;


typedef struct ParallaxObject ParallaxObject;
struct ParallaxObject
{
	ParallaxObject* next;
	ParallaxObjectType type;
	int layer;
	int sprUsed;
	sfVector2f pos;
	sfVector2f scale;
	sfColor color;
	float angle;
	float timer;
	float spd;
};

typedef struct
{
	ParallaxObject *firstParticle;
	float wind;
	int nbObject;
} ParallaxObjectPack;

ParallaxObjectPack InitParallaxPack();
ParallaxSprite LoadParallaxSprite();
void CreateParallax(ParallaxObjectPack* p, sfVector2f minPos, sfVector2f maxPos);
void DeleteParallaxObject(ParallaxObject* current, ParallaxObjectPack* pack);
void DeleteAllParallaxObject(ParallaxObjectPack* pack);
void UpdateParallax(ParallaxObjectPack *p, CameraViews *camera, Player player, sfVector2i mapSize, float dt);
void RenderParallax(sfRenderWindow* window, ParallaxObjectPack *p, CameraViews cam, SpritePack sprite, Layers *layer);
void RenderBackground(sfRenderWindow* window, SpritePack sprite);

#endif // !PARALLAX_H
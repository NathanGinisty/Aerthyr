#ifndef LAYERMANAGER_H
#define LAYERMANAGER_H

#include "../stdafx.h"
#include "../SpriteBank/SpriteBank.h"
#include "../Physic/Physics.h"

#define NB_LAYERS 100


//_______________LAYER DEFINE_______________
//__________________________________________


#define LAYER_BACKGROUND 19

#define LAYER_BGOBJECT4 17
#define LAYER_BGOBJECT3 16
#define LAYER_BGOBJECT2 15
#define LAYER_BGOBJECT1 14

#define LAYER_MAPBLOCK 12
#define LAYER_ITEMS 10
#define LAYER_PLAYER 35
#define LAYER_ENEMY 30
#define LAYER_PROJECTILE 29

#define LAYER_PARTICLE 6


#define DEFAULT_INFOBLIT (InfoBlitSprite) {(sfVector2f){1,1},sfWhite,0.f,sfFalse }

// ------- LIST LAYER ------- //
// 19 : Background
// 17 - 14 : background object floating (isle, cloud )
// 12 : Map Block 
// 9 : Player
// 8 : Enemy


typedef struct
{
	sfVector2f scale;
	sfColor color;
	float rotation;
	sfBool active;
}InfoBlitSprite;

typedef struct
{
	struct Chaine* suivant;
	InfoBlitSprite infoBlitSprite;
	sfSprite** sprite;
	sfVector2f pos;
	int type;
}Chaine;

typedef struct
{
	struct Chaine* premier;
}Layers;


void addToLayer(Chaine** first, sfSprite** sprite, sfVector2f pos, InfoBlitSprite infoBlitSprite);
void RenderLayers(Layers Liste[NB_LAYERS], sfRenderWindow* window);

#endif // !LAYERMANAGER_H

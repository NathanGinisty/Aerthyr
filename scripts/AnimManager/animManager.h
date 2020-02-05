#ifndef ANIMATION_H
#define ANIMATION_H

#include "../stdafx.h"
#include "../Utilities/Utilities.h"

// ----- STRUCT -----//

typedef struct
{
	sfSprite* *sprite;
}AnimSprite;

typedef struct
{
	int actualImage;
	float timer;
	float speedAnim;
	int nbImage;
}AnimInfo;

// ----- FUNCTION -----//



AnimSprite LoadAnimation(char* sNom, int origin, int nbImage);
AnimSprite* LoadAnimSpriteByFile(char* filename);

AnimInfo LoadAnimInfo(float speed, int nbImage);
AnimInfo* LoadAnimInfoByFile(char* filename);

void UpdateAnimation(AnimInfo *animInfo, AnimSprite *animSprite, float dt);

void RenderAnimation(AnimInfo animInfo, AnimSprite animSprite, sfVector2f pos, sfRenderWindow* window);


#endif  // !ANIMATION_H

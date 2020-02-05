#ifndef FADEMANAGER_H
#define FADEMANAGER_H

#include "../stdafx.h"
#include "../Utilities/Utilities.h"
#include "../GameStateManager/GameStateManager.h"

typedef enum
{
	F_fadeIn,
	F_active,
	F_fadeOut,
	F_inactive
}FadeState;

typedef struct
{
	float timeActual;
	float timeWanted;
	FadeState state;
	sfBool needChangeState;
	GameState changeTo;

}FadeInfo;

typedef struct
{
	sfSprite* sprite;
	FadeInfo infos;
}Fade;


Fade LoadFade();

void RenderFade(Fade fade, sfRenderWindow* window);

void UpdateFade(Fade* fade, GameState* gameState, float dt);


#endif // !FADEMANAGER_H
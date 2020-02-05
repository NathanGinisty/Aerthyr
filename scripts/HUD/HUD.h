#ifndef HUD_H
#define HUD_H

#include "../stdafx.h"
#include "../Player/Player.h"


typedef struct
{
	int actualImage;
	float timer;
	float speedAnim;
	int nbImage;
}TextureInfo;

typedef struct
{
	sfRenderStates renderstateStamina;
	sfVertexArray* arrayStamina;
	sfTexture* textureStamina;
	sfVector2f staminaPos;
	sfVector2f arrayPoints[5];
	sfBool staminaRegain;
	sfBool staminaTexturMoving;
	float staminaPercentage;
}StaminaGestion;

typedef struct
{
	sfRenderStates renderstateVitality;
	sfVertexArray* arrayVitality;
	sfTexture* tabTexturesVie[16];
	sfTexture* textureVitality;
	TextureInfo vieTextureInfo;
	sfVector2f vitalityPos;
	sfVector2f arrayPoints[5];
	float viePercentage;
}VitalityGestion;

typedef struct
{
	StaminaGestion stamina;
	VitalityGestion vitality;
	sfSprite* gauge;
	sfVector2f gaugePos;

}HUDStruct;

typedef struct
{
	sfSprite* spriteGears[6];
	sfVector2f posGears[6];
	float angle[6];
}HUDGearsAnim;



void BaseRenderState(sfRenderStates* state, sfTexture* texture);

float Anglevect(sfVector2f u, sfVector2f v);

void InitPointTexture(sfVector2f posBase, sfVector2f points[5], sfVector2f size);

StaminaGestion InitStamina(sfTexture* tempText, sfVector2f textureSize);

void UpdateStaminaPercentage(StaminaGestion* stamina, Player player, float Dt);

void UpdateStaminaTexturePoints(StaminaGestion* stamina, float Dt);


void GetTextures(sfTexture* texture[], int nbImage);

void UpdateTexture(TextureInfo *animInfo, float dt);

VitalityGestion InitVitality(sfVector2f textureSize);

void UpdateVitalityTexture(VitalityGestion* vitality, float Dt);

void UpdateVitalityPercentage(VitalityGestion* vitality, Player player, float Dt);

void UpdateVitalityTexturePoints(VitalityGestion* vitality, float Dt);

HUDStruct InitHudDatas();

void UpdateHud(HUDStruct* BP_gestion, Player player, float deltaTime);

void BlitHud(HUDStruct BP_gestion, sfRenderWindow* window);

HUDGearsAnim InitHudGears();

void UpdateHudGears(float angle[6], float time, float Dt);

void BlitHudGears(HUDGearsAnim hudAnim, sfRenderWindow* window);
#endif // !HUD_H


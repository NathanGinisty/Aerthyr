#include "HUD.h"
#include "../MathsElements/MathElements.h"
#include "../Utilities/Utilities.h"

void BaseRenderState(sfRenderStates* state, sfTexture* texture)
{
	state->blendMode = sfBlendAlpha;
	state->transform = sfTransform_Identity;
	state->texture = texture;
	state->shader = NULL;
}

StaminaGestion InitStamina(sfTexture* tempText, sfVector2f textureSize)
{
	StaminaGestion stamina;
	BaseRenderState(&stamina.renderstateStamina, tempText);

	//utils var
	stamina.staminaPos = (sfVector2f) { 102, 103 };
	stamina.staminaRegain = sfTrue;
	stamina.staminaTexturMoving = sfFalse;
	stamina.staminaPercentage = 100;

	//array to display the texture on
	stamina.arrayStamina = sfVertexArray_create();
	sfVertexArray_setPrimitiveType(stamina.arrayStamina, sfQuads);

	//set the points of the vertex
	InitPointTexture(stamina.staminaPos, stamina.arrayPoints, textureSize);
	return stamina;
}

void InitPointTexture(sfVector2f posBase, sfVector2f points[5], sfVector2f size)
{
	points[0] = posBase;
	points[1] = (sfVector2f) { posBase.x + size.x, posBase.y };
	points[2] = (sfVector2f) { posBase.x + size.x, posBase.y + size.x };
	points[3] = (sfVector2f) { posBase.x, posBase.y + size.x };
	points[4] = (sfVector2f) { posBase.x + size.x, posBase.y };
}

void UpdateStaminaPercentage(StaminaGestion* stamina, Player player, float Dt)
{
	stamina->staminaPercentage = player.stats.staminaActual / player.stats.staminaMax * 100;

}

void UpdateStaminaTexturePoints(StaminaGestion* stamina, float Dt)
{

	sfVertex vertex;
	vertex.color = sfWhite;

	//modif pos point
	stamina->arrayPoints[1] = RotatageAutour(stamina->arrayPoints[4], stamina->arrayPoints[0], (90 - stamina->staminaPercentage * (90.0f / 100.0f)) * DEG2RAD);


	sfVector2f adj = AddSub2f(stamina->arrayPoints[3], stamina->arrayPoints[0], 0);
	sfVector2f hyp = AddSub2f(stamina->arrayPoints[1], stamina->arrayPoints[0], 0);

	float angleTest = Anglevect(adj, hyp);
	//set des points 
	vertex.position = stamina->arrayPoints[0];
	vertex.texCoords = AddSub2f(stamina->arrayPoints[0], stamina->staminaPos, 0);
	sfVertexArray_append(stamina->arrayStamina, vertex);
	vertex.position = stamina->arrayPoints[1];
	vertex.texCoords = AddSub2f(stamina->arrayPoints[1], stamina->staminaPos, 0);
	sfVertexArray_append(stamina->arrayStamina, vertex);
	if (angleTest > 45.0f)
	{
		vertex.position = stamina->arrayPoints[2];
		vertex.texCoords = AddSub2f(stamina->arrayPoints[2], stamina->staminaPos, 0);
		sfVertexArray_append(stamina->arrayStamina, vertex);
	}
	else
	{
		vertex.position = stamina->arrayPoints[1];
		vertex.texCoords = AddSub2f(stamina->arrayPoints[1], stamina->staminaPos, 0);
		sfVertexArray_append(stamina->arrayStamina, vertex);
	}
	vertex.position = stamina->arrayPoints[3];
	vertex.texCoords = AddSub2f(stamina->arrayPoints[3], stamina->staminaPos, 0);
	sfVertexArray_append(stamina->arrayStamina, vertex);
}




void GetTextures(sfTexture* texture[], int nbImage)
{
	for (int i = 0; i < nbImage; i++)
	{
		char file[255];
		sprintf(file, "Assets/Hud/animationVie/jauge_vie_frame_000%02d.png", i);
		texture[i] = sfTexture_createFromFile(file, NULL);
	}
}

void UpdateTexture(TextureInfo *animInfo, float dt)
{
	animInfo->timer += 1 * dt;

	if (animInfo->timer >= animInfo->speedAnim)
	{
		if (animInfo->actualImage < animInfo->nbImage)
		{
			animInfo->actualImage++;
		}
		if (animInfo->actualImage >= animInfo->nbImage)
		{
			animInfo->actualImage = 0;
		}
		animInfo->timer = 0;
	}
}

VitalityGestion InitVitality(sfVector2f textureSize)
{
	VitalityGestion vitality;
	//texture et render
	GetTextures(vitality.tabTexturesVie, 16);
	vitality.textureVitality = sfTexture_copy(vitality.tabTexturesVie[0]);

	BaseRenderState(&vitality.renderstateVitality, vitality.textureVitality);
	vitality.vieTextureInfo = (TextureInfo) { 0, 0, 0.1f, 16 };

	// var
	vitality.vitalityPos = (sfVector2f) { 56, 72 };
	vitality.viePercentage = 100;
	//array to display the texture on
	vitality.arrayVitality = sfVertexArray_create();
	sfVertexArray_setPrimitiveType(vitality.arrayVitality, sfQuads);

	//set the points of the vertex
	InitPointTexture(vitality.vitalityPos, vitality.arrayPoints, textureSize);
	return vitality;
}

void UpdateVitalityTexture(VitalityGestion* vitality, float Dt)
{
	//update de la texture actuelle utilisee
	UpdateTexture(&vitality->vieTextureInfo, Dt);
	vitality->renderstateVitality.texture = vitality->tabTexturesVie[vitality->vieTextureInfo.actualImage];
}

void UpdateVitalityPercentage(VitalityGestion* vitality, Player player, float Dt)
{
	vitality->viePercentage = player.stats.hpActual / player.stats.hpMax * 100;
}

void UpdateVitalityTexturePoints(VitalityGestion* vitality, float Dt)
{
	UpdateVitalityTexture(vitality, Dt);

	sfVertex vertex;
	vertex.color = sfWhite;
	//modif pos point
	vitality->arrayPoints[0];
	vitality->arrayPoints[0] = AddSub2f((sfVector2f) { 0, (125 - vitality->viePercentage * (125.0f / 100.0f)) }, vitality->vitalityPos, 1);
	vitality->arrayPoints[1].y = vitality->arrayPoints[0].y;

	//set des points 
	vertex.position = vitality->arrayPoints[0];
	vertex.texCoords = AddSub2f(vitality->arrayPoints[0], vitality->vitalityPos, 0);
	sfVertexArray_append(vitality->arrayVitality, vertex);
	vertex.position = vitality->arrayPoints[1];
	vertex.texCoords = AddSub2f(vitality->arrayPoints[1], vitality->vitalityPos, 0);
	sfVertexArray_append(vitality->arrayVitality, vertex);
	vertex.position = vitality->arrayPoints[2];
	vertex.texCoords = AddSub2f(vitality->arrayPoints[2], vitality->vitalityPos, 0);
	sfVertexArray_append(vitality->arrayVitality, vertex);
	vertex.position = vitality->arrayPoints[3];
	vertex.texCoords = AddSub2f(vitality->arrayPoints[3], vitality->vitalityPos, 0);
	sfVertexArray_append(vitality->arrayVitality, vertex);
}




HUDStruct InitHudDatas()
{
	HUDStruct HudDatas;
	sfTexture* tempText = sfTexture_createFromFile("Assets/Hud/stamina.png", NULL);
	HudDatas.stamina = InitStamina(tempText, (sfVector2f) { 237, 260 });
	HudDatas.vitality = InitVitality((sfVector2f) { 125, 125 });


	HudDatas.gauge = LoadSprite("Assets/Hud/HUD.png", 0);
	HudDatas.gaugePos = (sfVector2f) { 0, 0 };

	return HudDatas;
}

void UpdateHud(HUDStruct* BP_gestion, Player player, float deltaTime)
{
	UpdateStaminaPercentage(&BP_gestion->stamina, player, deltaTime);
	UpdateStaminaTexturePoints(&BP_gestion->stamina, deltaTime);
	UpdateVitalityPercentage(&BP_gestion->vitality, player, deltaTime);
	UpdateVitalityTexturePoints(&BP_gestion->vitality, deltaTime);
}

void BlitHud(HUDStruct BP_gestion, sfRenderWindow* window)
{
	sfRenderWindow_drawVertexArray(window, BP_gestion.stamina.arrayStamina, &BP_gestion.stamina.renderstateStamina);
	sfRenderWindow_drawVertexArray(window, BP_gestion.vitality.arrayVitality, &BP_gestion.vitality.renderstateVitality);
	sfVertexArray_clear(BP_gestion.stamina.arrayStamina);
	sfVertexArray_clear(BP_gestion.vitality.arrayVitality);
	BlitSprite(BP_gestion.gauge, BP_gestion.gaugePos, window);
}

HUDGearsAnim InitHudGears()
{
	HUDGearsAnim temp;
	temp.posGears[0] = (sfVector2f) { 166, 169 };
	temp.posGears[1] = (sfVector2f) { 302, 54 };
	temp.posGears[2] = (sfVector2f) { 131, 47 };
	temp.posGears[3] = (sfVector2f) { 15, 258 };
	temp.posGears[4] = (sfVector2f) { 6, 124 };
	temp.posGears[5] = (sfVector2f) { 139, 157 };

	temp.spriteGears[0] = LoadSprite("Assets/Hud/Rouages_HUD/Rouages3.png", 1);
	temp.spriteGears[1] = LoadSprite("Assets/Hud/Rouages_HUD/Rouages2.png", 1);
	temp.spriteGears[2] = LoadSprite("Assets/Hud/Rouages_HUD/Rouages1.png", 1);
	temp.spriteGears[3] = LoadSprite("Assets/Hud/Rouages_HUD/Rouages5.png", 1);
	temp.spriteGears[4] = LoadSprite("Assets/Hud/Rouages_HUD/Rouages6.png", 1);
	temp.spriteGears[5] = LoadSprite("Assets/Hud/Rouages_HUD/Rouages4.png", 1);

	for (int i = 0; i < 6; i++)
	{
		temp.angle[i] = 0;
	}
	return temp;
}

void UpdateHudGears(float angle[6], float time, float Dt)
{
	angle[0] += (1 * Dt * 80);
	angle[1] -= ((1 + (64.f / 120.f)) * Dt * 80);

	angle[3] = (sinf(time * 45) * Dt * 20250 * DEG2RAD);

	angle[4] += (1.0f * Dt * 50);
	angle[5] -= (((83.0f / 60.f) - 0.5f)* Dt * 50);
}

void BlitHudGears(HUDGearsAnim hudAnim, sfRenderWindow* window)
{
	for (int i = 0; i < 6; i++)
	{
		sfSprite_setRotation(hudAnim.spriteGears[i], hudAnim.angle[i]);
		BlitSprite(hudAnim.spriteGears[i], hudAnim.posGears[i], window);
	}
}

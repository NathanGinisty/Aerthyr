#include "BackpackRotating.h"

float calculAngleSelection(sfVector2f vect, int angleOffset)
{
	float ret = atan2f(-vect.y, -vect.x) * RAD2DEG + angleOffset;
	return ret;
}

sfVector2f Lerp2f(sfVector2f start, sfVector2f target, float time, float timeNeeded)
{
	sfVector2f tempPos = { 0, 0 };
	sfVector2f tempTurn = { 0, 0 };
	if (time < timeNeeded)
	{
		tempPos = AddSub2f(target, start, 0);
		sfVector2f tmpPerp = { tempPos.y ,-tempPos.x };
		float dist = sqrtf(tmpPerp.x * tmpPerp.x + tmpPerp.y * tmpPerp.y);

		tmpPerp.x /= dist;
		tmpPerp.y /= dist;


		tmpPerp.x *= 100 * sinf(time * PI / timeNeeded);
		tmpPerp.y *= 100 * sinf(time * PI / timeNeeded);

		tempPos = AddSub2f(start, MultDivFloat(tempPos, time / timeNeeded, 1), 1);
		tempPos = AddSub2f(tempPos, tmpPerp, 1);
	}
	return tempPos;
}

void RotatingBPPointsUpdate(RotatingBPPoints obj[QTITY], float Dt, float gameTime, int *bagState)
{
	sfBool end = sfFalse;
	for (int i = 0; i < QTITY; i++)
	{
		if (*bagState == BP_opening)
		{
			obj[i].time += Dt;
			obj[i].pos = Lerp2f((sfVector2f) { SCREENW / 2, SCREENH / 2 }, obj[i].cible, obj[i].time, INVENTORYTIME);
		}
		else if (*bagState == BP_closing)
		{
			obj[i].time += Dt;
			obj[i].pos = Lerp2f(obj[i].cible, (sfVector2f) { SCREENW / 2, SCREENH / 2 }, obj[i].time, INVENTORYTIME);
		}

		if (obj[i].time > INVENTORYTIME)
		{
			obj[i].moving = sfFalse;
			obj[i].pos = obj[i].cible;
			obj[i].time = 0;
			if (i == QTITY - 1)
			{
				end = sfTrue;
			}
		}
	}

	if (end)
	{
		if (*bagState == BP_opening)
		{
			*bagState = BP_open;
		}
		else if (*bagState == BP_closing)
		{
			*bagState = BP_close;
		}
	}
}

RotatingBPPoints iniRotatingBPPoints(sfVector2f pos, sfVector2f cible)
{
	RotatingBPPoints tempObj;
	tempObj.pos = pos;
	tempObj.cible = cible;
	tempObj.moving = sfTrue;
	tempObj.time = 0;

	return tempObj;
}

void initPosGears(RotatingBackpackDatas* data)
{
	float angle = PI / (QTITY / 2.0f);
	data->angleSize = 360 / QTITY;
	float angleCroiss = 22.5 * DEG2RAD;
	for (int i = 0; i < QTITY; i++)
	{
		data->a[2] = RotatageAutour(data->a[1], data->a[0], angleCroiss);
		angleCroiss += angle;
		data->posObjets[i] = data->a[2];
	}

	for (int i = 0; i < QTITY; i++)
	{
		data->engrenages[i] = iniRotatingBPPoints(data->dep, ShiftDecal(data->posObjets[i], data->dep));
	}

	data->posSelect.pos = data->engrenages[0].cible;

}

RotatingBackpackDatas initRotatingBackpack()
{
	RotatingBackpackDatas tempRot;
	tempRot.vertical = (sfVector2f) { 0, -100 };
	tempRot.dep = (sfVector2f) { SCREENW / 2, SCREENH / 2 };
	tempRot.vertical = ShiftDecal(tempRot.vertical, tempRot.dep);
	tempRot.a[0] = (sfVector2f) { 0, 0 };
	// definit distance entre point dep et l endroit ou s arretent engranges
	tempRot.a[1] = (sfVector2f) { 0, -226 };
	tempRot.bagState = BP_close;
	tempRot.debutRot = tempRot.a[1];
	tempRot.debutRot = RotatageAutour(tempRot.debutRot, tempRot.a[0], -45 * DEG2RAD);
	tempRot.debutRot = ShiftDecal(tempRot.debutRot, tempRot.dep);
	tempRot.posSelect.num = 0;
	tempRot.posSelect.pos = VEC2F_NULL;
	for (int i = 0; i < 8; i++)
	{
		tempRot.equiped[i].itemID = -1;
		tempRot.equiped[i].quantity = -1;
		tempRot.equiped[i].type = -1;
	}
	tempRot.etat = BP_selectSlot;
	tempRot.equiped[0].type = loot_attack;
	tempRot.equiped[1].type = loot_attack;
	tempRot.equiped[2].type = loot_attack;

	tempRot.equiped[3].type = loot_skill;
	tempRot.equiped[4].type = loot_skill;

	tempRot.equiped[5].type = loot_defense;
	tempRot.equiped[6].type = loot_defense;
	tempRot.equiped[7].type = loot_defense;


	return tempRot;
}

RotatingBPSelect getGearNumber(RotatingBackpackDatas data, RotatingBPSelect select, sfRenderWindow* window)
{
	RotatingBPSelect ret = select;
	// remplacer ret par ancienne pos
	float xAb = sfJoystick_getAxisPosition(0, sfJoystickX);
	float yAb = sfJoystick_getAxisPosition(0, sfJoystickY);
	if (fabsf(xAb) > 40 || fabsf(yAb) > 40)
	{
		sfVector2f ABVect = { xAb, yAb };
		ABVect = ShiftDecal(ABVect, data.dep);
		sfVector2f angle2 = AddSub2f(ABVect, data.dep, 0);

		float testAngle = calculAngleSelection(angle2, 180);

		for (int i = 0; i < QTITY; i++)
		{
			float truc = i * data.angleSize + 270;

			float angleLeft = (truc - (data.angleSize / 2));
			float angleRight = (truc + (data.angleSize / 2));

			if (angleLeft > 360)
			{
				angleLeft -= 360;
			}
			if (angleRight > 360)
			{
				angleRight -= 360;
			}

			if (angleRight < angleLeft)
			{
				if (testAngle > angleLeft || testAngle < angleRight)
				{
					ret.pos = data.engrenages[i].pos;
					ret.num = i;
				}
			}
			else
			{
				if (testAngle > angleLeft && testAngle < angleRight)
				{
					ret.pos = data.engrenages[i].pos;
					ret.num = i;
				}
			}
		}
	}
	if (ret.num >= 0 && ret.num <= 2)
	{
		ret.type = loot_defense;
	}
	else if (ret.num <= 4)
	{
		ret.type = loot_skill;
	}
	else
	{
		ret.type = loot_attack;
	}
	return ret;
}

void BlitGears(RotatingBackpackDatas RBPDatas, RotatingBPSelect posSelect, RotatingBackpackSprite sprites, BackpackSprite spritesItem, sfRenderWindow* window)
{
	if (RBPDatas.bagState != BP_close)
	{
		//rotation et scale
		sfVector2f testA = AddSub2f(RBPDatas.engrenages[0].pos, RBPDatas.dep, 0);
		float testL = sqrtf(testA.x * testA.x + testA.y * testA.y);
		sfVector2f scale = { testL / 226.f,testL / 226.f };

		sfVector2f tempA = AddSub2f(RBPDatas.engrenages[0].pos, RBPDatas.dep, 0);
		sfVector2f tempB = AddSub2f(RBPDatas.debutRot, RBPDatas.dep, 0);

		float angleBG = Anglevect(tempB, tempA) + 22.5f;

		sfSprite_setScale(sprites.grosEngrenage, scale);
		sfSprite_setRotation(sprites.grosEngrenage, fabsf(angleBG));

		BlitSprite(sprites.grosEngrenage, RBPDatas.dep, window);
		sfColor selColor = sfColor_fromRGB(120, 120, 120);

		if (RBPDatas.etat == BP_gestion)
		{
			for (int i = 0; i < QTITY; i++)
			{
				sfSprite_setColor(sprites.engrenages[i], selColor);
				BlitSprite(sprites.engrenages[i], RBPDatas.engrenages[i].pos, window);
			}
		}
		else
		{
			for (int i = 0; i < QTITY; i++)
			{
				if (i != posSelect.num)
				{
					sfSprite_setColor(sprites.engrenages[i], selColor);
				}
				else
				{
					sfSprite_setColor(sprites.engrenages[i], sfWhite);
				}
				BlitSprite(sprites.engrenages[i], RBPDatas.engrenages[i].pos, window);
			}
		}
		
	}
	if (RBPDatas.bagState == BP_open)
	{
		//sprites.selecteurs = sfSprite_copy(sprites.engrenages[posSelect.num]);
		//sfColor selColor = sfColor_fromRGB(255, 210, 50);
		//
		//sfSprite_setColor(sprites.selecteurs, selColor);
		//BlitSprite(sprites.selecteurs, posSelect.pos, window);
	}
	if (RBPDatas.bagState != BP_close)
	{
		for (int i = 0; i < 8; i++)
		{
			if (RBPDatas.equiped[i].itemID != -1)
			{
				int add = 0;
				if (RBPDatas.equiped[i].type == loot_defense)
				{
					add = 6;
				}
				else if (RBPDatas.equiped[i].type == loot_skill)
				{
					add = 12;
				}
				else
				{
					add = 0;
				}
				sfVector2f temp = { RBPDatas.engrenages[i].pos.x - CASESIZE / 2, RBPDatas.engrenages[i].pos.y - CASESIZE / 2 };
				BlitSprite(spritesItem.items[RBPDatas.equiped[i].itemID + add], temp, window);
			}
		}
	}
}
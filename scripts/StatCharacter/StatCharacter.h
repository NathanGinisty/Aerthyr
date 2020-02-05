#ifndef STATCHARACTER_H
#define STATCHARACTER_H

#include "../stdafx.h"
#include "../Utilities/Utilities.h"

typedef struct
{
	sfBool invincible;
	sfBool invisible;

	float movespd;
	float jump;

	int jumpNbActual;
	int jumpNbMax;

	int physicAtk;
	float spellPower; // pourcentage
	float critDmg;    // pourcentage
	float critChance; // pourcentage
	float cdReduct;   // pourcentage

	float hpActual;
	float hpMax;
	float hpRegen;
	int armor;

	float staminaActual;
	float staminaMax;
	float staminaRegen;
} StatPlayer;

#endif // !STATCHARACTER_H
#include "SpellManager.h"

// RETURN TO : player.spell[0] ou player.spell[1]

Spell EquipSpell(SpellName name)
{
	Spell tmp;

	switch (name)
	{
	case spl_minorHeal:
		tmp.cdMax = 5;
		break;
	case spl_dotHeal:
		tmp.cdMax = 10;
		break;
	case spl_tripleJump:
		tmp.cdMax = 5;
		break;
	case spl_fireBall:
		tmp.cdMax = 0.f;
		break;
	default:
		tmp.cdMax = 15;
		break;
	}

	tmp.name = name;
	tmp.active = sfFalse;
	tmp.cdActual = tmp.cdMax;

	return tmp;
}


//Keyboard Event
void UseSpell(Spell* spell)
{
	if (spell->cdActual == spell->cdMax && spell->active == sfFalse)
	{
		spell->active = sfTrue;
	}
}


void UpdateSpellCD(Spell *spell, float dt)
{

	if (spell->cdActual < spell->cdMax)
	{
		spell->cdActual += dt;

		if (spell->cdActual > spell->cdMax)
		{
			spell->cdActual = spell->cdMax;
		}
	}
}

// Pointeur de fonction de spell :
#pragma region SpellBook

void MinorHeal(Spell *spell, StatPlayer *player, Entity2D *entity, Projectile** first, float dt)
{
	UpdateSpellCD(spell, dt);

	if (spell->active)
	{
		printf("SPELL CAST : MINOR HEAL\n");
		player->hpActual += 100 * player->spellPower;
		spell->cdActual = 0;
		spell->active = sfFalse;
	}
}

void TripleJump(Spell *spell, StatPlayer *player, Entity2D *entity, Projectile** first, float dt)
{
	UpdateSpellCD(spell, dt);

	player->jumpNbMax = 3;

	if (spell->active)
	{
		printf("SPELL CAST : TripleJump-Reset\n");
		player->staminaActual += 100 * player->spellPower;
		spell->cdActual = 0;
		spell->active = sfFalse;
	}
}

void DotHeal(Spell *spell, StatPlayer *player,Entity2D *entity, Projectile** first, float dt)
{
	static sfBool firstLoop = sfTrue;
	static float timer = 0;

	UpdateSpellCD(spell, dt);

	if (spell->active)
	{
		if (firstLoop)
		{
			printf("SPELL CAST : DOT HEAL\n");
			player->hpRegen += 50 * player->spellPower;
			spell->cdActual = 0;
			firstLoop = sfFalse;
			timer += dt;
		}
		else if (!firstLoop)
		{
			timer += dt;
			if (timer > 5)
			{
				player->hpRegen -= 50 * player->spellPower;
				timer = 0;
				firstLoop = sfTrue;
				spell->active = sfFalse;
			}
		}
	}
}

void FireBall(Spell *spell, StatPlayer *player, Entity2D *entity, Projectile** first, float dt)
{
	UpdateSpellCD(spell, dt);

	if (spell->active)
	{
		sfVector2f dir;
		dir.x = 0;// entity->dir;
		dir.y = 1.f;

		printf("SPELL CAST : FAYELU BALLU\n");
		AddProjectile_Vector(Proj_Player_FireBall, Proj_Player, entity->middlePos, dir, 3000, first);
		spell->cdActual = 0;
		spell->active = sfFalse;
	}
}

#pragma endregion

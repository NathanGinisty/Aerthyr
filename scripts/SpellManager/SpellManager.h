#ifndef SPELLMANAGER_H
#define SPELLMANAGER_H

#include "../stdafx.h"
#include "../Command/command.h"
#include "../SoundManager/soundManager.h"
#include "../StatCharacter/StatCharacter.h"
#include "../ParticleManager/ParticleManager.h"

#define NBSPELL 2

typedef enum
{
	spl_minorHeal,
	spl_dotHeal,
	spl_tripleJump,
	spl_fireBall,
	nbEnumSpellName
} SpellName;

typedef struct
{
	SpellName name;
	sfBool active;
	float cdMax;
	float cdActual;
} Spell;

Spell EquipSpell(SpellName name);
void UseSpell(Spell* spell);

//spellbook
void MinorHeal(Spell *spell, StatPlayer *player, Entity2D *entity, Projectile** first, float dt);
void DotHeal(Spell *spell, StatPlayer *player, Entity2D *entity, Projectile** first, float dt);
void TripleJump(Spell *spell, StatPlayer *player, Entity2D *entity, Projectile** first, float dt);
void FireBall(Spell *spell, StatPlayer *player, Entity2D *entity, Projectile** first, float dt);
#endif // !SPELLMANAGER_H
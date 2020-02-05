#ifndef PARTICLEMANAGER_H
#define PARTICLEMANAGER_H

#include "../stdafx.h"
#include "../ProjectileController/ProjectileController.h"

// ----------------- ENUM 
typedef enum
{
	explosion,
	quickplosion,
	exploWithAttenuation,
	spiral,
	breath,
	dust,
	snow
} ParticleTypes;

typedef enum
{
	clear,
	snowfall
} Meteos;

// ----------------- STRUCT 
typedef struct Particle Particle;
struct Particle
{
	Particle* next;
	ParticleTypes type;
	sfVector2f pos;
	sfVector2f scale;
	sfColor color;
	float lifetimeActual;
	float lifetimeMax;
	float alpha;
	float angle;
	float spd;
	int spriteUsed; //ex: sprite.smoke[spriteUsed]
};

typedef struct
{
	Particle *firstParticle;
	Meteos meteo;

	float timerSnow;
	float wind;
	int nbParticle;
} ParticleEffect;

// ----------------- RETURN FUNCTION

ParticleEffect InitParticle();
ParticleSprite LoadParticleSprite();
void CreateParticles(ParticleEffect* p, ParticleTypes type, int maxParticle, sfVector2f pos);
void DeleteParticle(Particle* current, Particle** first);
void UpdateParticles(ParticleEffect *p, float dt);
void RenderParticles(Layers* layer, ParticleEffect *p, SpritePack sprite);

#endif // !PARTICLEMANAGER_H
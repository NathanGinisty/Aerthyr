#ifndef PROJECTILECONTROLLER_H
#define PROJECTILECONTROLLER_H

#include "../stdafx.h"
#include "../LayerManager/layerManager.h"

typedef enum
{
	Proj_Ranger,
	Proj_Player_FireBall
}ProjectileType;

typedef enum
{
	Proj_Player,
	Proj_Enemy
}Proj_Launcher;

typedef struct Projectile Projectile;

struct Projectile 
{
	rigidBody body;
	ProjectileType type;
	Proj_Launcher launcher;

	float rotation;
	float rotationSpeed;

	float lifeTime;
	float currentLifeTime;

	int dir;
	int damageDealed;

	sfBool hasHit;

	void(*Update)(struct Projectile*, float);

	struct Projectile* ptrNext;
};

void AddProjectile_Target(ProjectileType type, Proj_Launcher launcher, sfVector2f pos, sfVector2f targetPos, int damageDealed, struct Projectile** first);
void AddProjectile_Vector(ProjectileType type, Proj_Launcher launcher, sfVector2f pos, sfVector2f vDir, int damageDealed, struct Projectile** first);
void RevertProjectile(Projectile* projectile);
void UpdateProjectiles(struct Projectile** first, float dt);
void RenderProjectiles(SpritePack sp, struct Projectile** first, Layers* layer);

#endif // !PROJECTILECONTROLLER_H
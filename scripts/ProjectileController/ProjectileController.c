#include "ProjectileController.h"

sfVector2f GetInitialDir(sfVector2f pos, sfVector2f targetPos)
{
	float dist = getDistance(pos, targetPos);
	float distY = GetAxeDistance(pos.y, targetPos.y);
	float distX = GetAxeDistance(pos.x, targetPos.x);

	float offset = dist + distY + distX;
	sfVector2f vDir;
	vDir.x = targetPos.x - pos.x;
	vDir.y = targetPos.y - pos.y;

	vDir = Normalize(vDir);

	return vDir;
}

void UpdateRangerProjectile(struct Projectile* projectile, float dt)
{
	RigidBodyUpdate(&projectile->body, dt);
	projectile->rotation += projectile->rotationSpeed * projectile->dir * dt;
}

void UpdatePlayerFireBall(struct Projectile* projectile, float dt)
{
	RigidBodyUpdate(&projectile->body, dt);
	projectile->rotation += projectile->rotationSpeed * projectile->dir * dt;
}

struct Projectile CreateRangerProjectile(sfVector2f pos, sfVector2f targetPos, int damageDealed)
{
	Projectile tmpProj;
	sfVector2f vDir = GetInitialDir(pos, targetPos);
	float speed = 1650.0f + rand()% 100;
	tmpProj.body = RigidbodyLoader(pos, 0.5f, sfFalse);

	tmpProj.body.vel.x = vDir.x * speed;
	tmpProj.body.vel.y = vDir.y * speed;

	if (vDir.x >= 0)
	{
		tmpProj.dir = 1;
	}
	else if (vDir.x < 0)
	{
		tmpProj.dir = -1;
	}

	tmpProj.type = Proj_Ranger;
	tmpProj.launcher = Proj_Enemy;

	tmpProj.rotation = 0.0f;
	tmpProj.rotationSpeed = 800.0f;

	tmpProj.lifeTime = 2.0f;
	tmpProj.currentLifeTime = 0.0f;

	tmpProj.damageDealed = damageDealed;
	tmpProj.hasHit = sfFalse;

	tmpProj.Update = &UpdateRangerProjectile;
	tmpProj.ptrNext = NULL;

	return tmpProj;
}

struct Projectile CreatePlayeFireBall(sfVector2f pos, sfVector2f* targetPos, sfVector2f* vectorDir, int damageDealed)
{
	Projectile tmpProj;
	sfVector2f vDir;
	if (vectorDir == NULL)
	{
		 vDir = GetInitialDir(pos, *targetPos);
	}
	else
	{
		vDir = *vectorDir;
	}

	float speed = 1650.0f + rand() % 100;
	tmpProj.body = RigidbodyLoader(pos, 0.5f, sfFalse);

	tmpProj.body.vel.x = vDir.x * speed;
	tmpProj.body.vel.y = vDir.y * speed;

	if (vDir.x >= 0)
	{
		tmpProj.dir = 1;
	}
	else if (vDir.x < 0)
	{
		tmpProj.dir = -1;
	}

	tmpProj.type = Proj_Player_FireBall;
	tmpProj.launcher = Proj_Player;

	tmpProj.rotation = 0.0f;
	tmpProj.rotationSpeed = 800.0f;

	tmpProj.lifeTime = 2.0f;
	tmpProj.currentLifeTime = 0.0f;

	tmpProj.damageDealed = damageDealed;
	tmpProj.hasHit = sfFalse;

	tmpProj.Update = &UpdatePlayerFireBall;
	tmpProj.ptrNext = NULL;

	return tmpProj;
}

void AddProjectile_Target(ProjectileType type, Proj_Launcher launcher, sfVector2f pos, sfVector2f targetPos, int damageDealed, struct Projectile** first)
{
	struct Projectile* tempProjectile = (struct Projectile*) calloc(sizeof(struct Projectile), 1);
	switch (type)
	{
	case Proj_Ranger:
		*tempProjectile = CreateRangerProjectile(pos, targetPos, damageDealed);
		break;
	case Proj_Player_FireBall:
		*tempProjectile = CreatePlayeFireBall(pos, &targetPos, NULL, damageDealed);
		break;
	default:
		break;
	}
	
	tempProjectile->ptrNext = *first;
	*first = tempProjectile;
}

void AddProjectile_Vector(ProjectileType type, Proj_Launcher launcher, sfVector2f pos, sfVector2f vDir, int damageDealed, struct Projectile** first)
{
	struct Projectile* tempProjectile = (struct Projectile*) calloc(sizeof(struct Projectile), 1);
	switch (type)
	{
	case Proj_Player_FireBall:
		*tempProjectile = CreatePlayeFireBall(pos, NULL, &vDir, damageDealed);
		break;
	default:
		break;
	}

	tempProjectile->ptrNext = *first;
	*first = tempProjectile;
}

void RemoveProjectile(Projectile* current, struct Projectile** first)
{
	Projectile* tmpProj = *first;

	if (tmpProj == current) //Cas particulier du premier élément
	{
		*first = current->ptrNext;
		free(current);
	}
	else //Si ce n'est pas le premier élément
	{
		while (tmpProj != 0) //On cherche l'élément précédent celui qu'on veut supprimer
		{
			if (tmpProj->ptrNext == current)
			{
				Projectile* next = tmpProj->ptrNext;
				tmpProj->ptrNext = next->ptrNext; //On rebranche par dessus
				free(current);	//Puis le fait sauter
				return;
			}
			tmpProj = tmpProj->ptrNext;
		}
	}
}

void RevertProjectile(Projectile* projectile)
{
	projectile->dir *= -1;
	projectile->body.vel.x *= -1;
	projectile->body.vel.y *= -1;
}

void UpdateProjectiles(struct Projectile** first, float dt)
{
	if (*first != NULL)
	{
		struct Projectile* tmpProj = *first;
		struct Projectile* savedProj = tmpProj->ptrNext;

		while (tmpProj != NULL)
		{
			tmpProj->Update(tmpProj, dt);
			tmpProj->currentLifeTime += dt;

			if (tmpProj->hasHit || tmpProj->currentLifeTime >= tmpProj->lifeTime)
			{
				RemoveProjectile(tmpProj, first);
				tmpProj = savedProj;
			}

			if (tmpProj != NULL)
			{
				savedProj = tmpProj;
				tmpProj = tmpProj->ptrNext;
			}
		}
	}
}

void RenderProjectiles(SpritePack sp, struct Projectile** first, Layers* layer)
{
	for (Projectile* tmpProj = *first; tmpProj != NULL; tmpProj = tmpProj->ptrNext)
	{
		InfoBlitSprite ibs = DEFAULT_INFOBLIT;
		ibs.rotation = tmpProj->rotation;
		ibs.active = sfTrue;
		addToLayer(&layer[LAYER_PROJECTILE].premier, &sp.projectiles[tmpProj->type], tmpProj->body.pos, ibs);
	}
}
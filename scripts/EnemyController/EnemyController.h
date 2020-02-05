#ifndef ENEMYCONTROLLER_H
#define ENEMYCONTROLLER_H

#include "../ItemManager/ItemManager.h"

typedef enum
{
	E_Runner,
	E_Ranger,
	E_Heavy
}EnemyType;

typedef enum
{
	E_Patrol,
	E_Idle,
	E_Attack,
	E_Stun,
	E_Invincible,
	E_Rest,
	E_Death,
	E_Dead,
	E_LookAtTarget,
	E_MoveToTarget,
	E_Jump,
	E_Escape
}EnemyState;

typedef enum
{
	E_Anim_Walk,
	E_Anim_Idle,
	E_Anim_Attack_1,
	E_Anim_Attack_2,
	E_Anim_Stun,
	E_Anim_Rest,
	E_Anim_HitSomething,
	E_Anim_TakeDamage,
	E_Anim_Death,
	E_Anim_NUMBER
}EnemyAnimState;

typedef enum
{
	E_Atk_CaC,
	E_Atk_Distance,
	E_Atk_Charge,
	E_Atk_Area
}EnemyAttackType;

typedef struct EnemiesAnimsInfos
{
	AnimInfo* runner;
	AnimInfo* ranger;
	AnimInfo* heavy;
}EnemiesAnimsInfos;

typedef struct EnemyAttackData EnemyAttackData;
typedef struct EnemyAttackData
{
	sfVector2f startPos;
	sfVector2f baseVelToAdd;
	sfVector2f velToAdd;

	float attackRange;

	float chargeDist;
	float chargeMoveSpeed;

	float baseStunTime;
	float stunTime;

	int baseDamage;
	int damage;

	sfBool isRepulsive;
	sfBool hasThrown;
	sfBool hasHit;

	EnemyAttackType type;

	void(*update)(struct EnemyAttackData*, sfVector2f, EnemyType, float, float);
}EnemyAttackData;


typedef struct
{
	EnemyAttackData* atksDatas;

	float reloadTime;
	float currentTime;

	int atkNB;
	int atkID;

	sfBool canBeUse;
}EnemyAttack;

typedef struct
{
	float timer;
	float currentTimer;

	int atkID;
	int hitCount;

	sfBool canBeHit;
}PlayerHit;

typedef struct Enemy Enemy;
struct Enemy
{
	Entity2D entity;
	EnemyType type;
	EnemyState previousState;
	EnemyState currentState;
	EnemyAnimState animState;
	EnemyAttack* attackArray;

	sfVector2f rightBound;
	sfVector2f leftBound;

	sfVector2f* playerPos;

	SegmentLine ray;

	AnimInfo* animInfo;
	sfVector2f savedPos;

	float speedMove;
	float moveToSpeed;
	float repulseSpeed;

	float jumpForce;

	float patrolDistance;

	float detectionRange;
	float chaseRange;
	float escapeRange;

	// -- TIMER -- \\

	float idleTime;
	float currentIdleTime;

	float lookAtTimer;
	float currentLookTime;

	float stunTime;
	float currentStunTime;

	float repulseTime;

	float invincibilityTime;
	float currentInvicibleTime;

	int moveDir;
	int lookDir;

	int baseLife;
	int life;

	int patrolStateNB;
	int patrolID;

	int attacksNB;
	int attackActiveID;

	int animNB;

	sfBool isBoundsSet;
	sfBool hasSpawn;
	sfBool isBlocked_R;
	sfBool isBlocked_L;
	sfBool hasLoot;

	struct Enemy* ptrNext;

	/*
	Function use to damage the enemy.
	Parameters :
	- the enemy affected
	- the amount of damage
	- the speed of the repulse
	- the time of the repulse state
	*/
	void(*takeDamages)(struct Enemy*, int, float, float);
	void(*patrol)(struct Enemy*, CollisionMap, InstanceMaps, float);
	void(*lookAt)(struct Enemy*, CollisionMap, InstanceMaps, float);
	void(*idle)(struct Enemy*, CollisionMap, InstanceMaps, float);
	void(*moveTo)(struct Enemy*, CollisionMap, InstanceMaps, float);
	void(*attack_1)(struct Enemy*, void**, CollisionMap, InstanceMaps, float);
	void(*attack_2)(struct Enemy*, void**, CollisionMap, InstanceMaps, float);
	void(*jump)(struct Enemy*, CollisionMap, InstanceMaps, float);
	void(*stun)(struct Enemy*, CollisionMap, InstanceMaps, float);
	void(*invincible)(struct Enemy*, CollisionMap, InstanceMaps, float);
	void(*escape)(struct Enemy*, CollisionMap, InstanceMaps, float);
	void(*death)(struct Enemy*, CollisionMap, InstanceMaps, float);
};

AnimSprite** LoadEnemiesAnim(EnemiesAnimsInfos* animsInfos);
void RemoveEnemy(Enemy* current, struct Enemy** first);
void RemoveAllEnemies(struct Enemy** first);
void UpdateEnemies(struct Enemy** first, struct Projectile** firstProj, Loot_Item* itemList, CollisionMap map, InstanceMaps instanceMaps, float dt);
void RenderEnemies(SpritePack sp, struct Enemy** first, Layers* layer);
void LoadTiledEnemy(TiledMap map, char area, struct Enemy** first, sfVector2f* playerPos, EnemiesAnimsInfos animsInfos);

#endif // !ENEMYCONTROLLER_H
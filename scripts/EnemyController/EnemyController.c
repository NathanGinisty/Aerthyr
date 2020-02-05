#include "EnemyController.h"

#define E_RUNNER_LIFE_DIFF1 450
#define E_RANGER_LIFE_DIFF1 700
#define E_HEAVY_LIFE_DIFF1 1200

// -------------------------------- ENEMIES MANAGEMENT FUNCTIONS --------------------------------
#pragma region ENEMIES MANAGEMENT
AnimSprite** LoadEnemiesAnim(EnemiesAnimsInfos* animsInfos)
{
	AnimSprite **anims = (AnimSprite**)malloc(3 * sizeof(AnimSprite*));

	anims[E_Runner] = LoadAnimSpriteByFile("Enemies/Runner/animFile_1-1");
	anims[E_Ranger] = LoadAnimSpriteByFile("Enemies/Ranger/animFile_1-2");
	anims[E_Heavy] = LoadAnimSpriteByFile("Enemies/Heavy/animFile_1-3");

	animsInfos->runner = LoadAnimInfoByFile("Enemies/Runner/animFile_1-1");
	animsInfos->ranger = LoadAnimInfoByFile("Enemies/Ranger/animFile_1-2");
	animsInfos->heavy = LoadAnimInfoByFile("Enemies/Heavy/animFile_1-3");

	return anims;
}

void TakeDamages(Enemy* enemy, int amount, float repulseValue, float repulseTime)
{
	enemy->life -= amount;

	if (enemy->animState == E_Anim_Attack_1 || enemy->animState == E_Anim_Attack_2)
	{
		enemy->animInfo[enemy->animState].actualImage = 0;
		enemy->animInfo[enemy->animState].timer = 0.0f;
		enemy->attackArray[enemy->attackActiveID].currentTime = enemy->attackArray[enemy->attackActiveID].reloadTime;
		enemy->attackArray[enemy->attackActiveID].canBeUse = sfFalse;
	}

	if (enemy->life <= 0)
	{
		enemy->previousState = enemy->currentState;
		enemy->currentState = E_Death;
		enemy->animState = E_Anim_TakeDamage;

		enemy->invincibilityTime = 0.3f;
		enemy->currentInvicibleTime = enemy->invincibilityTime;

		enemy->entity.body.vel.y = 0.0f;
		enemy->entity.body.vel.x = 0.0f;
	}
	else
	{
		int testDir = enemy->playerPos->x - enemy->entity.body.pos.x;

		if (!IsSameSign(enemy->moveDir, testDir))
		{
			enemy->moveDir *= -1;
			enemy->entity.dir = enemy->moveDir;
		}
		enemy->previousState = enemy->currentState;
		enemy->currentState = E_Invincible;
		enemy->animState = E_Anim_TakeDamage;

		enemy->invincibilityTime = 0.4f;
		enemy->currentInvicibleTime = enemy->invincibilityTime;

		enemy->entity.body.vel.x = 0.0f;
		enemy->entity.body.vel.y = 0.0f;

		enemy->repulseTime = 0.2f;
		enemy->repulseSpeed = repulseValue;
	}
}

void AddEnemy(struct Enemy toAdd, struct Enemy** first)
{
	struct Enemy* tempEnemy = (struct Enemy*) calloc(sizeof(struct Enemy), 1);
	*tempEnemy = toAdd;

	tempEnemy->ptrNext = *first;
	*first = tempEnemy;
}

void RemoveEnemy(Enemy* current, struct Enemy** first)
{
	Enemy* tmpEnemy = *first;

	if (tmpEnemy == current) //Cas particulier du premier élément
	{
		*first = current->ptrNext;
		free(current);
	}
	else //Si ce n'est pas le premier élément
	{
		while (tmpEnemy != 0) //On cherche l'élément précédent celui qu'on veut supprimer
		{
			if (tmpEnemy->ptrNext == current)
			{
				Enemy* next = tmpEnemy->ptrNext;
				tmpEnemy->ptrNext = next->ptrNext; //On rebranche par dessus
				free(current);	//Puis le fait sauter
				return;
			}
			tmpEnemy = tmpEnemy->ptrNext;
		}
	}
}

void RemoveAllEnemies(struct Enemy** first)
{
	if (*first != NULL)
	{
		struct Enemy* enemy = *first;
		struct Enemy* nextEnemy = enemy->ptrNext;

		while (enemy != NULL)
		{
			if (enemy != NULL)
			{
				RemoveEnemy(enemy, first);
				enemy = nextEnemy;
			}

			if (enemy != NULL)
			{
				nextEnemy = enemy;
				enemy = enemy->ptrNext;
			}
		}
	}
}

sfBool IsBlockSlopped(CollisionMap map, int blockID)
{
	sfBool isSlopped = sfFalse;

	if (blockID >= 0)
	{
		int nbLine = map.prefabs[blockID].nbLine;
		int testedgID = GetGroundSegmentID(map, blockID);

		if (map.prefabs[blockID].line[testedgID].dir.y != 0.0f)
		{
			isSlopped = sfTrue;
		}
	}

	return isSlopped;
}

sfBool IsChainedBlock(Enemy enemy, CollisionMap map, InstanceMaps instanceMaps, int dir)
{
	int actualgID = GetGroundSegmentID(map, enemy.entity.blocID);
	int blockID;
	int testedgID;
	sfVector2i mapPoint;
	sfVector2i currentBlocPos = enemy.entity.blocPos;
	sfVector2i testedBlocPos;

	sfBool isChained = sfFalse;

	blockID = instanceMaps.maps[instanceMaps.actualMap].colliderID[currentBlocPos.x][currentBlocPos.y];

	if (dir > 0)
	{
		int testedBlocID;
		sfVector2i testedPoint;
		testedBlocPos.x = currentBlocPos.x + 1;
		testedBlocPos.y = currentBlocPos.y + 1;

		testedBlocID = instanceMaps.maps[instanceMaps.actualMap].colliderID[testedBlocPos.x][testedBlocPos.y];

		if (testedBlocID >= 0 && blockID >= 0)
		{
			testedgID = GetGroundSegmentID(map, testedBlocID);

			testedPoint.x = map.prefabs[testedBlocID].line[testedgID].pointA.x + testedBlocPos.x * SIZE_TILE;
			testedPoint.y = map.prefabs[testedBlocID].line[testedgID].pointA.y + testedBlocPos.x * SIZE_TILE;

			mapPoint.x = map.prefabs[blockID].line[actualgID].pointB.x + currentBlocPos.x * SIZE_TILE;
			mapPoint.y = map.prefabs[blockID].line[actualgID].pointB.y + currentBlocPos.y * SIZE_TILE;

			if (mapPoint.x == testedPoint.x && mapPoint.y == testedPoint.y || IsBlockSlopped(map, testedBlocID))
			{
				isChained = sfTrue;
			}
		}

		testedBlocPos.x = currentBlocPos.x + 1;
		testedBlocPos.y = currentBlocPos.y - 1;

		testedBlocID = instanceMaps.maps[instanceMaps.actualMap].colliderID[testedBlocPos.x][testedBlocPos.y];

		if (testedBlocID >= 0 && blockID >= 0)
		{
			testedgID = GetGroundSegmentID(map, testedBlocID);

			testedPoint.x = map.prefabs[testedBlocID].line[testedgID].pointA.x + testedBlocPos.x * SIZE_TILE;
			testedPoint.y = map.prefabs[testedBlocID].line[testedgID].pointA.y + testedBlocPos.x * SIZE_TILE;

			mapPoint.x = map.prefabs[blockID].line[actualgID].pointB.x + currentBlocPos.x * SIZE_TILE;
			mapPoint.y = map.prefabs[blockID].line[actualgID].pointB.y + currentBlocPos.y * SIZE_TILE;

			if (mapPoint.x == testedPoint.x && mapPoint.y == testedPoint.y || IsBlockSlopped(map, testedBlocID))
			{
				isChained = sfTrue;
			}
		}
	}
	else if (dir < 0)
	{
		int testedBlocID;
		sfVector2i testedPoint;
		testedBlocPos.x = currentBlocPos.x - 1;
		testedBlocPos.y = currentBlocPos.y + 1;

		testedBlocID = instanceMaps.maps[instanceMaps.actualMap].colliderID[testedBlocPos.x][testedBlocPos.y];

		if (testedBlocID >= 0 && blockID >= 0)
		{
			testedgID = GetGroundSegmentID(map, testedBlocID);

			testedPoint.x = map.prefabs[testedBlocID].line[testedgID].pointB.x + testedBlocPos.x * SIZE_TILE;
			testedPoint.y = map.prefabs[testedBlocID].line[testedgID].pointB.y + testedBlocPos.x * SIZE_TILE;

			mapPoint.x = map.prefabs[blockID].line[actualgID].pointA.x + currentBlocPos.x * SIZE_TILE;
			mapPoint.y = map.prefabs[blockID].line[actualgID].pointA.y + currentBlocPos.y * SIZE_TILE;

			if (mapPoint.x == testedPoint.x && mapPoint.y == testedPoint.y || IsBlockSlopped(map, testedBlocID))
			{
				isChained = sfTrue;
			}
		}

		testedBlocPos.x = currentBlocPos.x - 1;
		testedBlocPos.y = currentBlocPos.y - 1;

		testedBlocID = instanceMaps.maps[instanceMaps.actualMap].colliderID[testedBlocPos.x][testedBlocPos.y];

		if (testedBlocID >= 0 && blockID >= 0)
		{
			testedgID = GetGroundSegmentID(map, testedBlocID);

			testedPoint.x = map.prefabs[testedBlocID].line[testedgID].pointB.x + testedBlocPos.x * SIZE_TILE;
			testedPoint.y = map.prefabs[testedBlocID].line[testedgID].pointB.y + testedBlocPos.x * SIZE_TILE;

			mapPoint.x = map.prefabs[blockID].line[actualgID].pointA.x + currentBlocPos.x * SIZE_TILE;
			mapPoint.y = map.prefabs[blockID].line[actualgID].pointA.y + currentBlocPos.y * SIZE_TILE;

			if (mapPoint.x == testedPoint.x && mapPoint.y == testedPoint.y || IsBlockSlopped(map, testedBlocID))
			{
				isChained = sfTrue;
			}
		}
	}

	return isChained;
}

sfBool PlayerIsVisible(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps)
{
	sfVector2i A;
	sfVector2i B;
	int indexMap = instanceMaps.actualMap;

	enemy->ray.pointA = *enemy->playerPos;
	enemy->ray.pointB = enemy->entity.middlePos;

	if (enemy->playerPos->x < instanceMaps.maps[instanceMaps.actualMap].width * SIZE_TILE &&
		enemy->playerPos->x > 0.f &&
		enemy->playerPos->y < instanceMaps.maps[instanceMaps.actualMap].height * SIZE_TILE &&
		enemy->playerPos->y > 0.f)
	{

		if (enemy->ray.pointA.x < enemy->ray.pointB.x)
		{
			A.x = enemy->ray.pointA.x / SIZE_TILE;
			B.x = enemy->ray.pointB.x / SIZE_TILE;
		}
		else
		{
			A.x = enemy->ray.pointB.x / SIZE_TILE;
			B.x = enemy->ray.pointA.x / SIZE_TILE;
		}

		if (enemy->ray.pointA.y < enemy->ray.pointB.y)
		{
			A.y = enemy->ray.pointA.y / SIZE_TILE;
			B.y = enemy->ray.pointB.y / SIZE_TILE;
		}
		else
		{
			A.y = enemy->ray.pointB.y / SIZE_TILE;
			B.y = enemy->ray.pointA.y / SIZE_TILE;
		}

		for (int i = A.x; i <= B.x; i++)
		{
			for (int j = A.y; j <= B.y; j++)
			{
				if (i > 0 && i < instanceMaps.maps[indexMap].width && j > 0 && j < instanceMaps.maps[indexMap].height)
				{
					int id = instanceMaps.maps[indexMap].colliderID[i][j];
					if (id >= 0)
					{
						float offsetX = i * SIZE_TILE;
						float offsetY = j * SIZE_TILE;

						for (int k = 0; k < map.prefabs[id].nbLine; k++)
						{
							sfBool isCollide;

							SegmentLine tmpSeg = map.prefabs[id].line[k];
							tmpSeg.pointA.x += offsetX;
							tmpSeg.pointA.y += offsetY;
							tmpSeg.pointB.x += offsetX;
							tmpSeg.pointB.y += offsetY;

							isCollide = IsCollidingSegSeg(enemy->ray.pointA, enemy->ray.pointB, tmpSeg.pointA, tmpSeg.pointB);

							if (isCollide)
							{
								return sfFalse;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		return sfFalse;
	}

	return sfTrue;
}

void SetBound(Enemy* enemy)
{
	if (enemy->moveDir > 0)
	{
		enemy->rightBound.x = ((enemy->entity.blocPos.x + 1) * SIZE_TILE) - (SIZE_TILE / 2); // Convert the blocPos to the world pos and get the middle pos of the bloc
		enemy->rightBound.y = enemy->entity.blocPos.y * SIZE_TILE;
		enemy->savedPos = enemy->rightBound;
		enemy->moveDir = -abs(enemy->moveDir); // change the dir
	}
	else if (enemy->moveDir < 0)
	{
		enemy->leftBound.x = (enemy->entity.blocPos.x * SIZE_TILE) + (SIZE_TILE / 2); // Convert the blocPos to the world pos and get the middle pos of the bloc
		enemy->leftBound.y = enemy->entity.blocPos.y * SIZE_TILE;
		enemy->savedPos = enemy->leftBound;
		enemy->moveDir = abs(enemy->moveDir);  // change the dir
	}
}
#pragma endregion

// Temporary function for the jump, it's need to be more specific for each enemy
void EnemyJump(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance

	if (enemy->entity.isOnGround)
	{
		if (enemy->moveDir < 0)
		{
			if (enemy->entity.blocPos.x == 0)
			{
				enemy->entity.body.vel.y = -(enemy->jumpForce * 1.5f);
				enemy->entity.isOnGround = sfFalse;
			}
			else if (instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x - 1][enemy->entity.blocPos.y] < 0)
			{
				int testBlockID = instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x - 1][enemy->entity.blocPos.y + 1];
				sfBool isSlopped = IsBlockSlopped(map, testBlockID);

				if (!isSlopped && instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x - 1][enemy->entity.blocPos.y - 1] > 0)
				{
					if (instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x - 1][enemy->entity.blocPos.y - 2] < 0)
					{
						if (instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x - 1][enemy->entity.blocPos.y - 3] < 0)
						{
							enemy->entity.body.vel.y = -(enemy->jumpForce * 1.5f);
							enemy->entity.isOnGround = sfFalse;
						}
					}
				}
				else if (!isSlopped)
				{
					enemy->entity.body.vel.y = -(enemy->jumpForce);
					enemy->entity.isOnGround = sfFalse;
				}
			}
			else
			{
				int testBlockID = instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x - 1][enemy->entity.blocPos.y - 1];

				if (testBlockID >= 0)
				{
					if (!IsBlockSlopped(map, testBlockID) && instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x - 1][enemy->entity.blocPos.y - 2] < 0)
					{
						if (instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x - 1][enemy->entity.blocPos.y - 3] < 0)
						{
							enemy->entity.body.vel.y = -(enemy->jumpForce * 1.5f);
							enemy->entity.isOnGround = sfFalse;
						}
					}
				}
			}
		}
		else if (enemy->moveDir > 0)
		{
			if (enemy->entity.blocPos.x + 1 == instanceMaps.maps[indexMap].width)
			{
				enemy->entity.body.vel.y = -(enemy->jumpForce * 1.5f);
				enemy->entity.isOnGround = sfFalse;
			}
			else if (instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x + 1][enemy->entity.blocPos.y] < 0)
			{
				int testBlockID = instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x + 1][enemy->entity.blocPos.y + 1];
				sfBool isSlopped = IsBlockSlopped(map, testBlockID);

				if (!isSlopped && instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x + 1][enemy->entity.blocPos.y - 1] > 0)
				{
					if (instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x + 1][enemy->entity.blocPos.y - 2] < 0)
					{
						if (instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x + 1][enemy->entity.blocPos.y - 3] < 0)
						{
							enemy->entity.body.vel.y = -(enemy->jumpForce * 1.5f);
							enemy->entity.isOnGround = sfFalse;
						}
					}
				}
				else if (!isSlopped)
				{
					enemy->entity.body.vel.y = -(enemy->jumpForce);
					enemy->entity.isOnGround = sfFalse;
				}
			}
			else
			{
				int testBlockID = instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x + 1][enemy->entity.blocPos.y - 1];

				if (testBlockID >= 0)
				{
					if (!IsBlockSlopped(map, testBlockID) && instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x + 1][enemy->entity.blocPos.y - 2] < 0)
					{
						if (instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x + 1][enemy->entity.blocPos.y - 3] < 0)
						{
							enemy->entity.body.vel.y = -(enemy->jumpForce * 1.5f);
							enemy->entity.isOnGround = sfFalse;
						}
					}
				}
			}
		}
	}
}

// -------------------------------- ATTACKS UPDATE --------------------------------
#pragma region ATTACKS UPDATE

void Update_Charge_Attack(EnemyAttackData* atk, sfVector2f enemyPos, EnemyType type, float dirX, float dt)
{
	if (type == E_Runner)
	{
		atk->chargeDist = getDistance(enemyPos, atk->startPos);
		int damageMultiplier = (atk->chargeDist / 350) + (atk->chargeMoveSpeed / 1000);
		atk->damage = (atk->baseDamage * damageMultiplier) + (atk->baseDamage / 2);

		/*atk->velToAdd.x = dirX * (((atk->chargeMoveSpeed / 2) + (atk->chargeDist * 1.5f)) / 2 + atk->damage);
		atk->velToAdd.y = 0.f;*/

		atk->stunTime = atk->baseStunTime + (atk->chargeDist + atk->damage) / (atk->chargeMoveSpeed + atk->damage);

		atk->velToAdd.x = dirX * (atk->baseVelToAdd.x + (((atk->chargeMoveSpeed + atk->chargeDist) / 2.f) + atk->damage) * atk->stunTime);

	}
	else if (type == E_Ranger)
	{
		atk->velToAdd.x = atk->baseVelToAdd.x * dirX;
	}
}

void Update_AttackTimer(EnemyAttack* atks, int atkNB, float dt)
{
	for (int i = 0; i < atkNB; i++)
	{
		if (!atks[i].canBeUse && atks[i].currentTime > 0.f)
		{
			atks[i].currentTime -= dt;
			if (atks[i].currentTime <= 0.0f)
			{
				atks[i].canBeUse = sfTrue;
				atks[i].currentTime = atks[i].reloadTime;
			}
		}
	}
}

#pragma endregion

// -------------------------------- ATTACKS CREATIONS --------------------------------
#pragma region ATTACKS CREATION FUNCTIONS

EnemyAttack CreateAttack(int atkNB, float reloadTime)
{
	EnemyAttack atk;

	atk.atkNB = atkNB;
	atk.atkID = 0;

	atk.atksDatas = (EnemyAttackData*)malloc(sizeof(EnemyAttackData) * atk.atkNB);

	atk.reloadTime = reloadTime;
	atk.currentTime = reloadTime;

	return atk;
}

EnemyAttackData Create_CaC_Attack(float range, sfBool isRepulsive, int damage)
{
	EnemyAttackData atk;
	atk.type = E_Atk_CaC;
	atk.attackRange = range;

	atk.chargeDist = -1;
	atk.chargeMoveSpeed = -1;
	atk.startPos.x = -1;
	atk.startPos.y = -1;

	atk.baseDamage = damage;
	atk.damage = damage;

	atk.isRepulsive = isRepulsive;
	atk.hasThrown = sfFalse;
	atk.hasHit = sfFalse;

	atk.update = NULL;

	return atk;
}

EnemyAttackData Create_Distance_Attack(float range, int damage)
{
	EnemyAttackData atk;
	atk.type = E_Atk_Distance;
	atk.attackRange = range;

	atk.chargeDist = -1;
	atk.chargeMoveSpeed = -1;
	atk.startPos.x = -1;
	atk.startPos.y = -1;

	atk.baseDamage = damage;
	atk.damage = damage;

	atk.isRepulsive = sfFalse;
	atk.hasThrown = sfFalse;
	atk.hasHit = sfFalse;

	atk.update = NULL;

	return atk;
}

EnemyAttackData Create_Charge_Attack(float range, float moveSpeed, sfVector2f baseVelToAdd, int damage)
{
	EnemyAttackData atk;
	atk.type = E_Atk_Charge;
	atk.attackRange = range;
	atk.baseVelToAdd = baseVelToAdd;
	atk.velToAdd = baseVelToAdd;

	atk.chargeDist = -1;
	atk.chargeMoveSpeed = moveSpeed;
	atk.startPos.x = -1;
	atk.startPos.y = -1;

	atk.baseStunTime = 0.15f;

	atk.baseDamage = damage;
	atk.damage = damage;

	atk.isRepulsive = sfTrue;
	atk.hasThrown = sfFalse;
	atk.hasHit = sfFalse;

	atk.update = &Update_Charge_Attack;

	return atk;
}

EnemyAttackData Create_Area_Attack()
{
	EnemyAttackData atk;

	return atk;
}
#pragma endregion

// -------------------------------- ENEMIES FUNCTIONS --------------------------------
#pragma region ENEMIES FUNCTIONS

#pragma region RUNNER
void RunnerJump(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance

	if (enemy->entity.isOnGround)
	{
		if (enemy->moveDir < 0)
		{
			if (enemy->entity.blocPos.x == 0)
			{
				enemy->entity.body.vel.y = -(enemy->jumpForce * 1.5f);
				enemy->entity.isOnGround = sfFalse;
			}
			else if (instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x - 1][enemy->entity.blocPos.y] < 0)
			{
				if (!IsChainedBlock(*enemy, map, instanceMaps, enemy->moveDir))
				{
					enemy->entity.body.vel.y = -(enemy->jumpForce);
					enemy->entity.isOnGround = sfFalse;
				}
			}
		}
		else if (enemy->moveDir > 0)
		{
			if (enemy->entity.blocPos.x + 1 == instanceMaps.maps[indexMap].width)
			{
				enemy->entity.body.vel.y = -(enemy->jumpForce * 1.5f);
				enemy->entity.isOnGround = sfFalse;
			}
			else if (instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x + 1][enemy->entity.blocPos.y] < 0)
			{
				if (!IsChainedBlock(*enemy, map, instanceMaps, enemy->moveDir))
				{
					enemy->entity.body.vel.y = -(enemy->jumpForce);
					enemy->entity.isOnGround = sfFalse;
				}
			}
		}
	}
}

void Runner_Movements(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance

	float playerDistance = getDistance(*enemy->playerPos, enemy->entity.body.pos);

	if (enemy->entity.moveDirActive)
	{
		enemy->entity.body.vel.x = enemy->speedMove * (enemy->moveDir * enemy->entity.moveDir.x);
		enemy->entity.body.vel.y = enemy->speedMove * enemy->moveDir * enemy->entity.moveDir.y;
	}
	else
	{
		enemy->entity.body.vel.x = enemy->speedMove * enemy->moveDir;
	}

	enemy->entity.dir = enemy->moveDir;

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (enemy->isBoundsSet && (enemy->entity.body.pos.x > enemy->rightBound.x + 5 || enemy->entity.body.pos.x < enemy->leftBound.x - 5))
	{
		enemy->hasSpawn = sfFalse;
		enemy->isBoundsSet = sfFalse;
		enemy->leftBound.x = -1;
		enemy->rightBound.x = -1;
		return;
	}
	if (enemy->moveDir > 0)
	{
		if (enemy->entity.body.pos.x >= enemy->rightBound.x)
		{
			enemy->entity.body.pos.x = enemy->rightBound.x;
			enemy->moveDir = -abs(enemy->moveDir);

			enemy->idleTime = 0.5f + (float)(rand() % 10) / 10.0f; // set the idle state timer ( time of the idle State )
			enemy->currentIdleTime = enemy->idleTime;

			enemy->previousState = enemy->currentState;
			enemy->currentState = E_Idle;
			enemy->animState = E_Anim_Idle;
			enemy->entity.body.vel.x = 0.0f;
			enemy->entity.body.vel.y = 0.0f;
		}
		else if (enemy->entity.hasHitWall)
		{
			enemy->rightBound = enemy->entity.body.pos;
			enemy->entity.body.pos = enemy->rightBound;
			enemy->moveDir = -abs(enemy->moveDir);
			enemy->leftBound.x = -1;
			enemy->isBoundsSet = sfFalse;
		}
	}
	else if (enemy->moveDir < 0)
	{
		if (enemy->entity.body.pos.x <= enemy->leftBound.x)
		{
			enemy->entity.body.pos.x = enemy->leftBound.x;
			enemy->moveDir = abs(enemy->moveDir);

			enemy->idleTime = 0.5f + (float)(rand() % 10) / 10.0f; // set the idle state timer ( time of the idle State )
			enemy->currentIdleTime = enemy->idleTime;

			enemy->previousState = enemy->currentState;
			enemy->currentState = E_Idle;
			enemy->animState = E_Anim_Idle;
			enemy->entity.body.vel.x = 0.0f;
			enemy->entity.body.vel.y = 0.0f;
		}
		else if (enemy->entity.hasHitWall)
		{
			enemy->leftBound = enemy->entity.body.pos;
			enemy->entity.body.pos = enemy->entity.body.pos;
			enemy->moveDir = abs(enemy->moveDir);
			enemy->rightBound.x = -1;
			enemy->isBoundsSet = sfFalse;
		}
	}

	if (playerDistance <= enemy->detectionRange	&& PlayerIsVisible(enemy, map, instanceMaps, indexMap))
	{
		int testDir = enemy->playerPos->x - enemy->entity.body.pos.x;

		if (IsSameSign((float)testDir, (float)enemy->entity.dir))
		{
			enemy->previousState = enemy->currentState;
			enemy->currentState = E_LookAtTarget;
			enemy->animState = E_Anim_Idle;
			enemy->entity.body.vel.x = 0.0f;
			enemy->entity.body.vel.y = 0.0f;
		}
	}
}

void Runner_GetBounds(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance

	float playerDistance = getDistance(*enemy->playerPos, enemy->entity.body.pos);

	if (enemy->entity.moveDirActive)
	{
		enemy->entity.body.vel.x = enemy->speedMove * (enemy->moveDir * enemy->entity.moveDir.x);
		enemy->entity.body.vel.y = enemy->speedMove * enemy->moveDir * enemy->entity.moveDir.y;
	}
	else
	{
		enemy->entity.body.vel.x = enemy->speedMove * enemy->moveDir;
	}

	enemy->entity.dir = enemy->moveDir;

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (enemy->entity.blocID > -1)
	{
		if (enemy->moveDir > 0 && enemy->rightBound.x == -1)
		{
			if (enemy->entity.blocPos.x + 1 == instanceMaps.maps[indexMap].width)
			{
				SetBound(enemy);
			}
			else if (instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x + 1][enemy->entity.blocPos.y] == -1)
			{
				/*int testBlockID = instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x + 1][enemy->entity.blocPos.y + 1];

				if (!IsBlockSlopped(map, testBlockID))
				{
				SetBound(enemy);
				}*/

				if (!IsChainedBlock(*enemy, map, instanceMaps, enemy->moveDir))
				{
					SetBound(enemy);
				}
			}
			else if (enemy->entity.hasHitWall)
			{
				enemy->rightBound = enemy->entity.body.pos;
				enemy->entity.body.pos = enemy->rightBound;
				enemy->moveDir = -abs(enemy->moveDir);
			}
		}
		else if (enemy->moveDir < 0 && enemy->leftBound.x == -1)
		{
			if (enemy->entity.blocPos.x == 0)
			{
				SetBound(enemy);
			}
			else if (instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x - 1][enemy->entity.blocPos.y] == -1)
			{
				/*int testBlockID = instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x - 1][enemy->entity.blocPos.y + 1];

				if (!IsBlockSlopped(map, testBlockID))
				{
				SetBound(enemy);
				}*/
				if (!IsChainedBlock(*enemy, map, instanceMaps, enemy->moveDir))
				{
					SetBound(enemy);
				}
			}
			else if (enemy->entity.hasHitWall)
			{
				enemy->leftBound = enemy->entity.body.pos;
				enemy->entity.body.pos = enemy->entity.body.pos;
				enemy->moveDir = abs(enemy->moveDir);
			}
		}
	}

	if (enemy->leftBound.x > -1 && enemy->rightBound.x > -1)
	{
		enemy->isBoundsSet = sfTrue;
	}

	if (playerDistance <= enemy->detectionRange	&&	PlayerIsVisible(enemy, map, instanceMaps, indexMap))
	{
		int testDir = enemy->playerPos->x - enemy->entity.body.pos.x;

		if (IsSameSign((float)testDir, (float)enemy->entity.dir))
		{
			enemy->hasSpawn = sfFalse;
			enemy->isBoundsSet = sfFalse;
			enemy->leftBound.x = -1;
			enemy->rightBound.x = -1;

			enemy->previousState = enemy->currentState;
			enemy->currentState = E_LookAtTarget;
			enemy->animState = E_Anim_Idle;
		}
	}
}

void Runner_Patrol(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance
	if (!enemy->hasSpawn)
	{
		UpdateEntity(&enemy->entity.body, dt);
		Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

		if (enemy->entity.isOnGround)
		{
			enemy->hasSpawn = sfTrue;
		}
	}
	else
	{
		if (!enemy->isBoundsSet)
		{
			Runner_GetBounds(enemy, map, instanceMaps, dt);
		}
		else if (enemy->entity.blocID > -1)
		{
			Runner_Movements(enemy, map, instanceMaps, dt);
		}
	}
}

void Runner_LookAt(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance

	float playerDistance = getDistance(*enemy->playerPos, enemy->entity.body.pos);
	float playerDistY = GetAxeDistance(enemy->playerPos->y, enemy->entity.body.pos.y);

	// Look at timer update
	if (enemy->currentLookTime > 0.0f)
	{
		enemy->currentLookTime -= dt;
	}

	// Update the look dir of the enemy according to the pos of the player
	if (enemy->playerPos->x < enemy->entity.body.pos.x)
	{
		enemy->entity.dir = -1;
		enemy->moveDir = -1;
	}
	else if (enemy->playerPos->x > enemy->entity.body.pos.x)
	{
		enemy->entity.dir = 1;
		enemy->moveDir = 1;
	}

	if (playerDistance > enemy->detectionRange || !PlayerIsVisible(enemy, map, instanceMaps, indexMap))
	{
		enemy->previousState = enemy->currentState;
		enemy->currentState = E_Patrol;
		enemy->animState = E_Anim_Walk;
		enemy->currentLookTime = enemy->lookAtTimer;
		return;
	}
	else if (((playerDistance <= enemy->attackArray[0].atksDatas[0].attackRange) || (enemy->currentLookTime <= 0.0f)) && (playerDistY <= (SIZE_TILE * 3)) && enemy->attackArray[0].canBeUse)
	{
		enemy->attackArray[0].atksDatas[0].startPos = enemy->entity.body.pos;
		enemy->previousState = enemy->currentState;
		enemy->currentState = E_Attack;
		enemy->animState = E_Anim_Attack_1;
		enemy->currentLookTime = enemy->lookAtTimer;
		enemy->attackActiveID = 0;
	}
}

void Runner_Idle(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance

	float playerDistance = getDistance(*enemy->playerPos, enemy->entity.body.pos);

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (playerDistance <= enemy->detectionRange && PlayerIsVisible(enemy, map, instanceMaps, indexMap))
	{
		int testDir = enemy->playerPos->x - enemy->entity.body.pos.x;

		if (IsSameSign((float)testDir, (float)enemy->entity.dir))
		{
			enemy->currentIdleTime = enemy->idleTime;

			enemy->previousState = enemy->currentState;
			enemy->currentState = E_LookAtTarget;
			enemy->animState = E_Anim_Idle;
			return;
		}
	}

	enemy->currentIdleTime -= dt;

	if (enemy->currentIdleTime <= 0.0f)
	{
		enemy->currentIdleTime = enemy->idleTime;
		enemy->currentState = enemy->previousState;
		enemy->previousState = E_Idle;
		enemy->animState = enemy->currentState;
	}
}

//void Runner_MoveTo(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
//{
//	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance
//
//	float playerDistance = getDistance(*enemy->playerPos, enemy->entity.body.pos);
//
//	if (enemy->playerPos->x > enemy->entity.body.pos.x)
//	{
//		enemy->moveDir = abs(enemy->moveDir);
//	}
//	else
//	{
//		enemy->moveDir = -abs(enemy->moveDir);
//	}
//
//	if (enemy->entity.moveDirActive)
//	{
//		enemy->entity.body.vel.x = enemy->speedMove * (enemy->moveDir * enemy->entity.moveDir.x);
//		enemy->entity.body.vel.y = enemy->speedMove * enemy->moveDir * enemy->entity.moveDir.y;
//	}
//	else
//	{
//		enemy->entity.body.vel.x = enemy->speedMove * enemy->moveDir;
//	}
//
//	enemy->entity.dir = enemy->moveDir;
//
//	//----------------------------------------------------------------------------------------------------- JUMP ------------------
//	EnemyJump(enemy, map, instanceMaps);
//	//----------------------------------------------------------------------------------------------------- END JUMP ------------------
//
//	UpdateEntity(&enemy->entity.body, dt);
//	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);
//
//	if (enemy->isBoundsSet && (enemy->entity.body.pos.x > enemy->rightBound.x || enemy->entity.body.pos.x < enemy->leftBound.x))
//	{
//		enemy->hasSpawn = sfFalse;
//		enemy->isBoundsSet = sfFalse;
//		enemy->leftBound.x = -1;
//		enemy->rightBound.x = -1;
//		enemy->entity.body.vel.x = 0.0f;
//	}
//
//	if (playerDistance > enemy->detectionRange || !PlayerIsVisible(enemy, map, instanceMaps, indexMap))
//	{
//		enemy->previousState = enemy->currentState;
//		enemy->currentState = E_Patrol;
//	}
//	else if (playerDistance <= enemy->attackRange)
//	{
//		enemy->previousState = enemy->currentState;
//		enemy->currentState = E_Attack;
//	}
//}

void Runner_Attack(Enemy* enemy, struct Projectile** firstProj, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance
	if (enemy->entity.moveDirActive)
	{
		enemy->entity.body.vel.x = enemy->attackArray[0].atksDatas[0].chargeMoveSpeed * (enemy->moveDir * enemy->entity.moveDir.x);
		enemy->entity.body.vel.y = enemy->attackArray[0].atksDatas[0].chargeMoveSpeed * (enemy->moveDir * enemy->entity.moveDir.y);
	}
	else
	{
		enemy->entity.body.vel.x = enemy->attackArray[0].atksDatas[0].chargeMoveSpeed * enemy->moveDir;
	}

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	enemy->attackArray[0].atksDatas[0].update(&enemy->attackArray[0].atksDatas[0], enemy->entity.body.pos, enemy->type, enemy->moveDir, dt); // ATTACK update

	if (enemy->isBoundsSet && (enemy->entity.body.pos.x > enemy->rightBound.x || enemy->entity.body.pos.x < enemy->leftBound.x))
	{
		enemy->hasSpawn = sfFalse;
		enemy->isBoundsSet = sfFalse;
		enemy->leftBound.x = -1;
		enemy->rightBound.x = -1;
	}

	RunnerJump(enemy, map, instanceMaps);

	if (enemy->entity.hasHitWall || enemy->attackArray[0].atksDatas[0].hasHit)
	{
		if (enemy->attackArray[0].atksDatas[0].hasHit)
		{
			enemy->attackArray[0].atksDatas[0].hasHit = sfFalse;
		}
		enemy->attackArray[0].canBeUse = sfFalse;
		enemy->attackArray[0].atksDatas[0].damage = 0;

		enemy->stunTime = 1.5f;
		enemy->currentStunTime = enemy->stunTime;

		enemy->previousState = E_Patrol;
		enemy->currentState = E_Stun;
		enemy->animState = E_Anim_HitSomething;
		enemy->entity.body.vel.x = 0.0f;
		enemy->entity.body.vel.y = 0.0f;
	}
	else if (!enemy->entity.isOnGround)
	{
		enemy->previousState = E_Patrol;
		enemy->currentState = E_Jump;
	}
}

void Runner_Jump(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance
	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (enemy->entity.isOnGround)
	{
		enemy->previousState = enemy->currentState;
		enemy->currentState = E_Patrol;
		enemy->animState = E_Anim_Walk;
		enemy->attackArray[0].canBeUse = sfFalse;
	}
	if (enemy->entity.hasHitWall || enemy->attackArray[0].atksDatas[0].hasHit)
	{
		if (enemy->attackArray[0].atksDatas[0].hasHit)
		{
			enemy->attackArray[0].atksDatas[0].hasHit = sfFalse;
			enemy->attackArray[0].atksDatas[0].damage = 0;
		}
		enemy->attackArray[0].canBeUse = sfFalse;
		enemy->attackArray[0].atksDatas[0].damage = 0;

		enemy->stunTime = 1.5f;
		enemy->currentStunTime = enemy->stunTime;

		enemy->previousState = E_Patrol;
		enemy->currentState = E_Stun;
		enemy->animState = E_Anim_HitSomething;
		enemy->entity.body.vel.x = 0.0f;
		enemy->entity.body.vel.y = 0.0f;
	}
}

void Runner_Stun(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance
	enemy->entity.body.vel.x = 0.0f;

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	enemy->currentStunTime -= dt;

	if (enemy->currentStunTime <= 0.0f)
	{
		enemy->currentStunTime = enemy->stunTime;
		enemy->currentState = enemy->previousState;
		enemy->previousState = E_Stun;
		enemy->animState = E_Anim_Walk;
	}
}

void Runner_Invincible(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance
	if (enemy->invincibilityTime - enemy->currentInvicibleTime < enemy->repulseTime)
	{
		if (enemy->entity.moveDirActive)
		{
			enemy->entity.body.vel.x = enemy->repulseSpeed * (enemy->moveDir * -1 * enemy->entity.moveDir.x);
			enemy->entity.body.vel.y = enemy->repulseSpeed * (enemy->moveDir * -1 * enemy->entity.moveDir.y);
		}
		else
		{
			enemy->entity.body.vel.x = enemy->repulseSpeed * enemy->moveDir * -1;
		}
	}
	else
	{
		enemy->entity.body.vel.x = 0.f;
		if (enemy->entity.isOnGround)
		{
			enemy->entity.body.vel.y = 0.f;
		}
	}

	enemy->currentInvicibleTime -= dt;

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (enemy->currentInvicibleTime <= 0)
	{
		enemy->currentState = E_Patrol;
		enemy->animState = E_Anim_Walk;
	}
}

void Runner_Death(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance
	enemy->entity.body.vel.x = 0.00f;
	enemy->currentInvicibleTime -= dt;

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (enemy->currentInvicibleTime <= 0)
	{
		enemy->entity.body.vel.y = -1000.f;
		enemy->entity.body.Enablegravity = sfTrue;
		enemy->entity.isOnGround = sfFalse;
		enemy->currentState = E_Dead;
		// Spawn Item;
	}
}
#pragma endregion

#pragma region RANGER

void Ranger_Patrol_1(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance

	float playerDistance = getDistance(*enemy->playerPos, enemy->entity.body.pos);

	enemy->animState = E_Anim_Idle;

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (playerDistance <= enemy->detectionRange	&& PlayerIsVisible(enemy, map, instanceMaps, indexMap))
	{
		int testDir = enemy->playerPos->x - enemy->entity.body.pos.x;

		if (IsSameSign((float)testDir, (float)enemy->entity.dir))
		{
			enemy->previousState = enemy->currentState;
			enemy->currentState = E_LookAtTarget;
			enemy->animState = E_Anim_Idle;
			enemy->entity.body.vel.y = 0.0f;
			enemy->entity.body.vel.x = 0.0f;
		}
		else if (playerDistance <= (enemy->detectionRange - SIZE_TILE))
		{
			enemy->previousState = enemy->currentState;
			enemy->currentState = E_LookAtTarget;
			enemy->animState = E_Anim_Idle;
			enemy->entity.body.vel.x = 0.0f;
			enemy->entity.body.vel.y = 0.0f;
		}
	}
}

void Ranger_Patrol_2(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance

	float playerDistance = getDistance(*enemy->playerPos, enemy->entity.body.pos);

	if (enemy->animInfo[enemy->animState].actualImage > 2 && enemy->animInfo[enemy->animState].actualImage < 9)
	{
		if (enemy->entity.moveDirActive)
		{
			enemy->entity.body.vel.x = enemy->speedMove * (enemy->moveDir * enemy->entity.moveDir.x);
			enemy->entity.body.vel.y = enemy->speedMove * enemy->moveDir * enemy->entity.moveDir.y;
		}
		else
		{
			enemy->entity.body.vel.x = enemy->speedMove * enemy->moveDir;
		}
	}
	else
	{
		enemy->entity.body.vel.x = 0.f;
		if (enemy->entity.isOnGround)
		{
			enemy->entity.body.vel.y = 0.f;
		}
	}

	enemy->entity.dir = enemy->moveDir;

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (enemy->moveDir > 0)
	{
		if (enemy->entity.body.pos.x >= enemy->rightBound.x)
		{
			enemy->entity.body.pos.x = enemy->rightBound.x;
			enemy->moveDir = -abs(enemy->moveDir);

			enemy->idleTime = 1.5f + (float)(rand() % 10) / 10.0f; // set the idle state timer ( time of the idle State )
			enemy->currentIdleTime = enemy->idleTime;

			enemy->animInfo[enemy->animState].timer = 0.0f; // Reset the animation to the beginning
			enemy->animInfo[enemy->animState].actualImage = 0;

			enemy->previousState = enemy->currentState;
			enemy->currentState = E_Idle;
			enemy->animState = E_Anim_Idle;

			enemy->animInfo[enemy->animState].timer = 0.0f;
			enemy->animInfo[enemy->animState].actualImage = 0;

			enemy->entity.body.vel.x = 0.0f;
			enemy->entity.body.vel.y = 0.0f;
		}
		else if (enemy->entity.hasHitWall)
		{
			enemy->rightBound = enemy->entity.body.pos;
			enemy->entity.body.pos = enemy->rightBound;
			enemy->moveDir = -abs(enemy->moveDir);
			enemy->leftBound.x = -1;
			enemy->isBoundsSet = sfFalse;
		}
	}
	else if (enemy->moveDir < 0)
	{
		if (enemy->entity.body.pos.x <= enemy->leftBound.x)
		{
			enemy->entity.body.pos.x = enemy->leftBound.x;
			enemy->moveDir = abs(enemy->moveDir);

			enemy->idleTime = 1.5f + (float)(rand() % 10) / 10.0f; // set the idle state timer ( time of the idle State )
			enemy->currentIdleTime = enemy->idleTime;

			enemy->animInfo[enemy->animState].timer = 0.0f; // Reset the animation to the beginning
			enemy->animInfo[enemy->animState].actualImage = 0;

			enemy->previousState = enemy->currentState;
			enemy->currentState = E_Idle;
			enemy->animState = E_Anim_Idle;

			enemy->animInfo[enemy->animState].timer = 0.0f;
			enemy->animInfo[enemy->animState].actualImage = 0;

			enemy->entity.body.vel.x = 0.0f;
			enemy->entity.body.vel.y = 0.0f;
		}
		else if (enemy->entity.hasHitWall)
		{
			enemy->leftBound = enemy->entity.body.pos;
			enemy->entity.body.pos = enemy->entity.body.pos;
			enemy->moveDir = abs(enemy->moveDir);
			enemy->rightBound.x = -1;
			enemy->isBoundsSet = sfFalse;
		}
	}

	if (playerDistance <= enemy->detectionRange	&& PlayerIsVisible(enemy, map, instanceMaps, indexMap))
	{
		int testDir = enemy->playerPos->x - enemy->entity.body.pos.x;

		if (IsSameSign((float)testDir, (float)enemy->entity.dir))
		{
			enemy->previousState = enemy->currentState;
			enemy->currentState = E_LookAtTarget;
			enemy->animState = E_Anim_Idle;
			enemy->entity.body.vel.y = 0.0f;
			enemy->entity.body.vel.x = 0.0f;
		}
		else if (playerDistance <= (enemy->detectionRange - SIZE_TILE))
		{
			enemy->previousState = enemy->currentState;
			enemy->currentState = E_LookAtTarget;
			enemy->animState = E_Anim_Idle;
			enemy->entity.body.vel.x = 0.0f;
			enemy->entity.body.vel.y = 0.0f;
		}
	}
}

void Ranger_GetBounds(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance

	float playerDistance = getDistance(*enemy->playerPos, enemy->entity.body.pos);
	float distanceTraveled;

	if (enemy->animInfo[enemy->animState].actualImage > 2 && enemy->animInfo[enemy->animState].actualImage < 9)
	{
		if (enemy->entity.moveDirActive)
		{
			enemy->entity.body.vel.x = enemy->speedMove * (enemy->moveDir * enemy->entity.moveDir.x);
			enemy->entity.body.vel.y = enemy->speedMove * enemy->moveDir * enemy->entity.moveDir.y;
		}
		else
		{
			enemy->entity.body.vel.x = enemy->speedMove * enemy->moveDir;
		}
	}
	else
	{
		enemy->entity.body.vel.x = 0.f;
		if (enemy->entity.isOnGround)
		{
			enemy->entity.body.vel.y = 0.f;
		}
	}

	enemy->entity.dir = enemy->moveDir;

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (enemy->entity.blocID > -1)
	{
		if (enemy->moveDir > 0 && enemy->rightBound.x == -1)
		{
			distanceTraveled = getDistance(enemy->entity.body.pos, enemy->savedPos); // distance traveled by the enemy from his other bound

			if (distanceTraveled >= enemy->patrolDistance) // if the distance traveled is higher or equal to the patrol distance, then set the bound
			{
				SetBound(enemy);
			}
			else if (enemy->entity.blocPos.x + 1 == instanceMaps.maps[indexMap].width)
			{
				SetBound(enemy);
			}
			else if (instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x + 1][enemy->entity.blocPos.y] == -1)
			{
				if (!IsChainedBlock(*enemy, map, instanceMaps, enemy->moveDir))
				{
					SetBound(enemy);
				}
			}
			else if (enemy->entity.hasHitWall)
			{
				enemy->rightBound = enemy->entity.body.pos;
				enemy->entity.body.pos = enemy->rightBound;
				enemy->moveDir = -abs(enemy->moveDir);
			}
		}
		else if (enemy->moveDir < 0 && enemy->leftBound.x == -1)
		{
			distanceTraveled = getDistance(enemy->entity.body.pos, enemy->savedPos); // distance traveled by the enemy from his other bound

			if (distanceTraveled >= enemy->patrolDistance)
			{
				SetBound(enemy);
			}
			else if (enemy->entity.blocPos.x == 0)
			{
				SetBound(enemy);
			}
			else if (instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x - 1][enemy->entity.blocPos.y] == -1)
			{
				if (!IsChainedBlock(*enemy, map, instanceMaps, enemy->moveDir))
				{
					SetBound(enemy);
				}
			}
			else if (enemy->entity.hasHitWall)
			{
				enemy->leftBound = enemy->entity.body.pos;
				enemy->entity.body.pos = enemy->entity.body.pos;
				enemy->moveDir = abs(enemy->moveDir);
			}
		}
	}

	if (playerDistance <= enemy->detectionRange	&& PlayerIsVisible(enemy, map, instanceMaps, indexMap))
	{
		int testDir = enemy->playerPos->x - enemy->entity.body.pos.x;

		if (IsSameSign((float)testDir, (float)enemy->entity.dir))
		{
			enemy->isBoundsSet = sfFalse;
			enemy->leftBound.x = -1;
			enemy->rightBound.x = -1;

			enemy->animInfo[enemy->animState].timer = 0.0f;
			enemy->animInfo[enemy->animState].actualImage = 0;

			enemy->previousState = enemy->currentState;
			enemy->currentState = E_LookAtTarget;
			enemy->animState = E_Anim_Idle;
			enemy->entity.body.vel.y = 0.0f;
			enemy->entity.body.vel.x = 0.0f;
		}
		else if (playerDistance <= (enemy->detectionRange - SIZE_TILE))
		{
			enemy->isBoundsSet = sfFalse;
			enemy->leftBound.x = -1;
			enemy->rightBound.x = -1;

			enemy->animInfo[enemy->animState].timer = 0.0f;
			enemy->animInfo[enemy->animState].actualImage = 0;

			enemy->previousState = enemy->currentState;
			enemy->currentState = E_LookAtTarget;
			enemy->animState = E_Anim_Idle;
			enemy->entity.body.vel.x = 0.0f;
			enemy->entity.body.vel.y = 0.0f;
		}
	}

	if (enemy->leftBound.x > -1 && enemy->rightBound.x > -1)
	{
		enemy->isBoundsSet = sfTrue;
	}
}

void Ranger_Patrol(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance

	if (!enemy->hasSpawn)
	{
		UpdateEntity(&enemy->entity.body, dt);
		Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

		if (enemy->entity.isOnGround)
		{
			enemy->hasSpawn = sfTrue;
		}
	}
	else
	{
		if (enemy->entity.isOnGround)
		{
			if (enemy->patrolID == 1)
			{
				Ranger_Patrol_1(enemy, map, instanceMaps, dt);
			}
			else
			{
				if (!enemy->isBoundsSet)
				{
					Ranger_GetBounds(enemy, map, instanceMaps, dt);
				}
				else if (enemy->entity.blocID > -1)
				{
					Ranger_Patrol_2(enemy, map, instanceMaps, dt);
				}
			}
		}
		else
		{
			UpdateEntity(&enemy->entity.body, dt);
			Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);
		}
	}
}

void Ranger_Escape(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance

	float playerDistance = getDistance(*enemy->playerPos, enemy->entity.body.pos);
	int testDir = enemy->playerPos->x - enemy->entity.body.pos.x;

	if (IsSameSign((float)testDir, (float)enemy->moveDir))
	{
		enemy->moveDir *= -1;
		enemy->entity.dir = enemy->moveDir;
	}

	// -- ENEMY DEPLACEMENT -- \\

	if (enemy->animInfo[enemy->animState].actualImage > 2 && enemy->animInfo[enemy->animState].actualImage < 9)
	{
		if (enemy->entity.moveDirActive)
		{
			enemy->entity.body.vel.x = enemy->speedMove * (enemy->moveDir * enemy->entity.moveDir.x);
			enemy->entity.body.vel.y = enemy->speedMove * enemy->moveDir * enemy->entity.moveDir.y;
		}
		else
		{
			enemy->entity.body.vel.x = enemy->speedMove * enemy->moveDir;
		}
	}
	else
	{
		enemy->entity.body.vel.x = 0.f;
		if (enemy->entity.isOnGround)
		{
			enemy->entity.body.vel.y = 0.f;
		}
	}

	// --  -- \\

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (playerDistance <= enemy->attackArray[1].atksDatas[0].attackRange && enemy->attackArray[1].canBeUse)
	{
		enemy->moveDir *= -1;
		enemy->entity.dir = enemy->moveDir;

		enemy->animInfo[enemy->animState].timer = 0.0f;
		enemy->animInfo[enemy->animState].actualImage = 0;

		enemy->entity.body.vel.x = 0;
		enemy->previousState = enemy->currentState;
		enemy->currentState = E_Attack;
		enemy->animState = E_Anim_Attack_2;

		enemy->entity.body.vel.x = 0.0f;
		enemy->entity.body.vel.y = 0.0f;

		enemy->attackActiveID = 1;

		enemy->isBlocked_L = sfFalse;
		enemy->isBlocked_R = sfFalse;

		return;
	}

	if (enemy->entity.blocID > -1)
	{
		if (enemy->moveDir > 0)
		{
			if (enemy->entity.blocPos.x + 1 == instanceMaps.maps[indexMap].width)
			{
				enemy->isBlocked_R = sfTrue;
			}
			else if (enemy->entity.hasHitWall)
			{
				enemy->isBlocked_R = sfTrue;
			}
			else if (enemy->entity.blocPos.x + 1 < instanceMaps.maps[indexMap].width)
			{
				if (enemy->entity.blocPos.y >= 0 && enemy->entity.blocPos.y < instanceMaps.maps[indexMap].height)
				{
					if (instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x + 1][enemy->entity.blocPos.y] == -1)
					{
						if (!IsChainedBlock(*enemy, map, instanceMaps, enemy->moveDir))
						{
							enemy->isBlocked_R = sfTrue;
						}
					}
				}
			}
		}
		else if (enemy->moveDir < 0)
		{
			if (enemy->entity.blocPos.x == 0)
			{
				enemy->isBlocked_L = sfTrue;
			}
			else if (enemy->entity.hasHitWall)
			{
				enemy->isBlocked_L = sfTrue;
			}
			else if (enemy->entity.blocPos.x - 1 >= 0)
			{
				if (enemy->entity.blocPos.y >= 0 && enemy->entity.blocPos.y < instanceMaps.maps[indexMap].height)
				{
					if (instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x - 1][enemy->entity.blocPos.y] == -1)
					{
						if (!IsChainedBlock(*enemy, map, instanceMaps, enemy->moveDir))
						{
							enemy->isBlocked_L = sfTrue;
						}
					}
				}
			}
		}
	}

	if (enemy->isBoundsSet && (enemy->entity.body.pos.x > enemy->rightBound.x + 5 || enemy->entity.body.pos.x < enemy->leftBound.x - 5))
	{
		enemy->isBoundsSet = sfFalse;
		enemy->leftBound.x = -1;
		enemy->rightBound.x = -1;
		enemy->savedPos = enemy->entity.body.pos;
	}

	if (playerDistance >= (enemy->attackArray[0].atksDatas[0].attackRange - 50) || enemy->isBlocked_L || enemy->isBlocked_R)
	{
		printf("Bloc\n");
		enemy->entity.body.vel.x = 0;
		enemy->previousState = enemy->currentState;

		enemy->animInfo[enemy->animState].timer = 0.0f;
		enemy->animInfo[enemy->animState].actualImage = 0;

		enemy->currentState = E_Attack;
		enemy->animState = E_Anim_Idle;

		enemy->entity.body.vel.x = 0.0f;
		enemy->entity.body.vel.y = 0.0f;
	}
}

void Ranger_LookAt(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance

	float playerDistance = getDistance(*enemy->playerPos, enemy->entity.body.pos);

	// Look at timer update
	if (enemy->currentLookTime > 0.0f)
	{
		enemy->currentLookTime -= dt;
	}

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	// Update the look dir of the enemy according to the pos of the player
	if (enemy->playerPos->x < enemy->entity.body.pos.x)
	{
		enemy->entity.dir = -1;
		enemy->moveDir = -1;
	}
	else if (enemy->playerPos->x > enemy->entity.body.pos.x)
	{
		enemy->entity.dir = 1;
		enemy->moveDir = 1;
	}

	// Exit the state if the player is to for or if he is not visible. Switch to PATROL state
	if (playerDistance > enemy->detectionRange + 50 || !PlayerIsVisible(enemy, map, instanceMaps))
	{
		enemy->animInfo[enemy->animState].timer = 0.0f;
		enemy->animInfo[enemy->animState].actualImage = 0;

		enemy->previousState = enemy->currentState;
		enemy->currentState = E_Patrol;
		enemy->animState = E_Anim_Walk;

		enemy->animInfo[enemy->animState].timer = 0.0f;
		enemy->animInfo[enemy->animState].actualImage = 0;

		enemy->entity.body.vel.x = 0.0f;
		enemy->entity.body.vel.y = 0.0f;
		enemy->currentLookTime = enemy->lookAtTimer;
	}
	else if (playerDistance <= enemy->attackArray[1].atksDatas[0].attackRange && enemy->attackArray[1].canBeUse)
	{
		enemy->entity.body.vel.x = 0;

		enemy->animInfo[enemy->animState].timer = 0.0f;
		enemy->animInfo[enemy->animState].actualImage = 0;

		enemy->previousState = enemy->currentState;
		enemy->currentState = E_Attack;
		enemy->animState = E_Anim_Attack_2;

		enemy->entity.body.vel.x = 0.0f;
		enemy->entity.body.vel.y = 0.0f;
		enemy->currentLookTime = enemy->lookAtTimer;
		enemy->attackActiveID = 1;
	}
	else if (playerDistance <= enemy->attackArray[0].atksDatas[0].attackRange || enemy->currentLookTime <= 0.0f)
	{
		enemy->entity.body.vel.x = 0;

		enemy->animInfo[enemy->animState].timer = 0.0f;
		enemy->animInfo[enemy->animState].actualImage = 0;

		enemy->previousState = enemy->currentState;
		enemy->currentState = E_Attack;
		enemy->animState = E_Anim_Idle;

		enemy->animInfo[enemy->animState].timer = 0.0f;
		enemy->animInfo[enemy->animState].actualImage = 0;

		enemy->entity.body.vel.x = 0.0f;
		enemy->entity.body.vel.y = 0.0f;
		enemy->currentLookTime = enemy->lookAtTimer;
		enemy->attackActiveID = 0;
	}

}

void Ranger_Idle(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance

	float playerDistance = getDistance(*enemy->playerPos, enemy->entity.body.pos);

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (playerDistance <= enemy->detectionRange && PlayerIsVisible(enemy, map, instanceMaps, indexMap))
	{
		int testDir = enemy->playerPos->x - enemy->entity.body.pos.x;

		if (IsSameSign((float)testDir, (float)enemy->entity.dir))
		{
			enemy->currentIdleTime = enemy->idleTime;

			enemy->previousState = enemy->currentState;
			enemy->currentState = E_LookAtTarget;
			enemy->animState = E_Anim_Idle;
			return;
		}
	}

	enemy->currentIdleTime -= dt;

	if (enemy->currentIdleTime <= 0.0f)
	{
		enemy->currentIdleTime = enemy->idleTime;
		enemy->currentState = E_Patrol;
		enemy->previousState = E_Idle;
		enemy->animState = E_Anim_Walk;
	}
}

//void Ranger_MoveTo(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
//{
//	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance
//	float playerDistance = getDistance(*enemy->playerPos, enemy->entity.body.pos);
//
//	if (enemy->playerPos->x > enemy->entity.body.pos.x)
//	{
//		enemy->moveDir = abs(enemy->moveDir);
//	}
//	else
//	{
//		enemy->moveDir = -abs(enemy->moveDir);
//	}
//
//	if (enemy->entity.moveDirActive)
//	{
//		enemy->entity.body.vel.x = enemy->speedMove * (enemy->moveDir * enemy->entity.moveDir.x);
//		enemy->entity.body.vel.y = enemy->speedMove * enemy->moveDir * enemy->entity.moveDir.y;
//	}
//	else
//	{
//		enemy->entity.body.vel.x = enemy->speedMove * enemy->moveDir;
//	}
//
//	enemy->entity.dir = enemy->moveDir;
//
//	//----------------------------------------------------------------------------------------------------- JUMP ------------------
//	EnemyJump(enemy, map, instanceMaps);
//	//----------------------------------------------------------------------------------------------------- END JUMP ------------------
//
//	UpdateEntity(&enemy->entity.body, dt);
//	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);
//
//	if (enemy->isBoundsSet && (enemy->entity.body.pos.x > enemy->rightBound.x || enemy->entity.body.pos.x < enemy->leftBound.x))
//	{
//		enemy->hasSpawn = sfFalse;
//		enemy->isBoundsSet = sfFalse;
//		enemy->leftBound.x = -1;
//		enemy->rightBound.x = -1;
//	}
//
//	if (playerDistance > enemy->detectionRange || !PlayerIsVisible(enemy, map, instanceMaps, indexMap))
//	{
//		enemy->previousState = enemy->currentState;
//		enemy->currentState = E_Patrol;
//		enemy->entity.body.vel.x = 0.0f;
//		enemy->entity.body.vel.y = 0.0f;
//	}
//	else if (playerDistance <= enemy->attackRange)
//	{
//		enemy->previousState = enemy->currentState;
//		enemy->currentState = E_Attack;
//		enemy->entity.body.vel.x = 0.0f;
//		enemy->entity.body.vel.y = 0.0f;
//	}
//}

void Ranger_Attack_Dist(Enemy* enemy, struct Projectile** firstProj, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance

	float playerDistance = getDistance(*enemy->playerPos, enemy->entity.body.pos);

	if (enemy->playerPos->x < enemy->entity.body.pos.x)
	{
		enemy->entity.dir = -1;
		enemy->moveDir = -1;
	}
	else if (enemy->playerPos->x > enemy->entity.body.pos.x)
	{
		enemy->entity.dir = 1;
		enemy->moveDir = 1;
	}

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (enemy->attackArray[0].canBeUse)
	{
		enemy->animState = E_Anim_Attack_1;
		if (enemy->animInfo[enemy->animState].timer == 0.00f && enemy->animInfo[enemy->animState].actualImage == 3)
		{
			sfVector2f firePos = (sfVector2f) { enemy->entity.middlePos.x, enemy->entity.middlePos.y - 50.0f };
			AddProjectile_Target(Proj_Ranger, Proj_Enemy, firePos, *enemy->playerPos, enemy->attackArray[0].atksDatas[0].damage, firstProj);
			enemy->attackArray[0].atksDatas[0].hasThrown = sfTrue;
		}
		if (enemy->attackArray[0].atksDatas[0].hasThrown && enemy->animInfo[enemy->animState].timer == 0.00f && enemy->animInfo[enemy->animState].actualImage == 0)
		{
			enemy->animState = E_Anim_Idle;
			enemy->attackArray[0].canBeUse = sfFalse;
			enemy->attackArray[0].atksDatas[0].hasThrown = sfFalse;
		}
	}

	if (playerDistance <= enemy->attackArray[1].atksDatas[0].attackRange && enemy->attackArray[1].canBeUse && !enemy->attackArray[0].canBeUse)
	{
		enemy->animInfo[enemy->animState].timer = 0.0f;
		enemy->animInfo[enemy->animState].actualImage = 0;

		enemy->entity.body.vel.x = 0;
		enemy->previousState = enemy->currentState;
		enemy->currentState = E_Attack;
		enemy->animState = E_Anim_Attack_2;

		enemy->entity.body.vel.x = 0.0f;
		enemy->entity.body.vel.y = 0.0f;

		enemy->attackActiveID = 1;

		enemy->isBlocked_L = sfFalse;
		enemy->isBlocked_R = sfFalse;
	}
	else if ((playerDistance > enemy->attackArray[0].atksDatas[0].attackRange || !PlayerIsVisible(enemy, map, instanceMaps, indexMap)) && enemy->animState != E_Attack)
	{
		enemy->isBlocked_R = sfFalse;
		enemy->isBlocked_L = sfFalse;

		enemy->animInfo[enemy->animState].timer = 0.0f;
		enemy->animInfo[enemy->animState].actualImage = 0;

		enemy->currentState = E_Patrol;
		enemy->animState = E_Anim_Walk;
		enemy->entity.body.vel.x = 0.0f;
		enemy->entity.body.vel.y = 0.0f;
	}
	else if (playerDistance <= enemy->escapeRange)
	{
		if (((enemy->playerPos->x < enemy->entity.body.pos.x) && !enemy->isBlocked_R) || ((enemy->playerPos->x > enemy->entity.body.pos.x) && !enemy->isBlocked_L))
		{
			enemy->isBlocked_L = sfFalse;
			enemy->isBlocked_R = sfFalse;

			enemy->animInfo[enemy->animState].timer = 0.0f;
			enemy->animInfo[enemy->animState].actualImage = 0;

			enemy->currentState = E_Escape;
			enemy->animState = E_Anim_Walk;
		}
	}

}

void Ranger_Attack_Repuls(Enemy* enemy, struct Projectile** firstProj, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance

	float playerDistance = getDistance(*enemy->playerPos, enemy->entity.body.pos);

	// Allows the gravity calculation
	enemy->entity.isOnGround = sfFalse;
	enemy->entity.body.Enablegravity = sfTrue;

	if (!enemy->attackArray[1].atksDatas[0].hasHit && playerDistance > 150.f && !enemy->entity.hasHitWall)
	{
		if (enemy->animInfo[enemy->animState].actualImage > 1 && enemy->animInfo[enemy->animState].actualImage < 9)
		{
			enemy->entity.body.vel.x = enemy->attackArray[1].atksDatas[0].chargeMoveSpeed * enemy->moveDir;
		}
	}
	else if (playerDistance <= 150.f)
	{
		enemy->entity.body.vel.x = 0.f;
	}

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	enemy->attackArray[1].atksDatas[0].update(&enemy->attackArray[1].atksDatas[0], enemy->entity.body.pos, enemy->type, enemy->moveDir, dt);

	if (enemy->isBoundsSet && (enemy->entity.body.pos.x > enemy->rightBound.x + 5 || enemy->entity.body.pos.x < enemy->leftBound.x - 5))
	{
		enemy->isBoundsSet = sfFalse;
		enemy->leftBound.x = -1;
		enemy->rightBound.x = -1;
		enemy->savedPos = enemy->entity.body.pos;
	}

	if (enemy->animInfo[enemy->animState].timer == 0.00f && enemy->animInfo[enemy->animState].actualImage == 8)
	{
		enemy->attackArray[1].atksDatas[0].hasThrown = sfTrue;
	}
	if (enemy->attackArray[1].atksDatas[0].hasThrown && enemy->animInfo[enemy->animState].timer == 0.00f && enemy->animInfo[enemy->animState].actualImage == 0)
	{
		enemy->animInfo[enemy->animState].timer = 0.0f;
		enemy->animInfo[enemy->animState].actualImage = 0;

		enemy->currentState = E_LookAtTarget;
		enemy->animState = E_Anim_Idle;

		enemy->animInfo[enemy->animState].timer = 0.0f;
		enemy->animInfo[enemy->animState].actualImage = 0;

		enemy->attackArray[1].canBeUse = sfFalse;
		enemy->attackArray[1].atksDatas[0].hasThrown = sfFalse;
		enemy->attackArray[1].atksDatas[0].hasHit = sfFalse;

		enemy->entity.body.vel.x = 0.0f;
	}
}

void Ranger_Jump(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance
	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (enemy->entity.isOnGround)
	{
		enemy->previousState = enemy->currentState;
		enemy->currentState = E_Patrol;
		enemy->animState = E_Anim_Walk;
	}
}

void Ranger_Stun(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance
	enemy->entity.body.vel.x = 0.0f;

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	enemy->currentStunTime -= dt;

	if (enemy->currentStunTime <= 0.0f)
	{
		enemy->currentStunTime = enemy->stunTime;
		enemy->currentState = enemy->previousState;
		enemy->previousState = E_Stun;
		enemy->animState = E_Anim_Walk;
	}
}

void Ranger_Invincible(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance
	if (enemy->invincibilityTime - enemy->currentInvicibleTime < enemy->repulseTime)
	{
		if (enemy->entity.moveDirActive)
		{
			enemy->entity.body.vel.x = enemy->repulseSpeed * (enemy->moveDir * -1 * enemy->entity.moveDir.x);
			enemy->entity.body.vel.y = enemy->repulseSpeed * (enemy->moveDir * -1 * enemy->entity.moveDir.y);
		}
		else
		{
			enemy->entity.body.vel.x = enemy->repulseSpeed * enemy->moveDir * -1;
		}
	}
	else
	{
		enemy->entity.body.vel.x = 0.f;
		if (enemy->entity.isOnGround)
		{
			enemy->entity.body.vel.y = 0.f;
		}
	}
	enemy->currentInvicibleTime -= dt;

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (enemy->currentInvicibleTime <= 0)
	{
		enemy->currentInvicibleTime = enemy->invincibilityTime;
		enemy->currentState = E_Patrol;
		enemy->previousState = E_Invincible;
		enemy->animState = E_Anim_Walk;
		enemy->entity.body.vel.x = 0.f;
		enemy->entity.body.vel.y = 0.f;
	}
}

void Ranger_Death(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance
	enemy->entity.body.vel.x = 0.00f;
	enemy->currentInvicibleTime -= dt;

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (enemy->currentInvicibleTime <= 0)
	{
		enemy->entity.body.vel.y = -1000.f;
		enemy->entity.body.Enablegravity = sfTrue;
		enemy->entity.isOnGround = sfFalse;
		enemy->currentState = E_Dead;
		// Spawn Item;
	}
}
#pragma endregion

#pragma region HEAVY
void Heavy_JumpChecks(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance

	if (enemy->entity.isOnGround)
	{
		if (enemy->moveDir < 0)
		{
			sfBool canPass = enemy->entity.blocPos.x - 1 >= 0;

			if (canPass && instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x - 1][enemy->entity.blocPos.y] < 0)
			{
				canPass = enemy->entity.blocPos.y + 1 < instanceMaps.maps[indexMap].height;
				if (canPass && instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x - 1][enemy->entity.blocPos.y + 1] < 0)
				{
					canPass = enemy->entity.blocPos.x - 3 >= 0;
					if (canPass)
					{
						if (instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x - 3][enemy->entity.blocPos.y] >= 0)
						{
							canPass = enemy->entity.blocPos.y - 1 >= 0;
							if (canPass && instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x - 3][enemy->entity.blocPos.y - 1] >= 0)
							{
								canPass = enemy->entity.blocPos.y - 2 >= 0;
								if (canPass && instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x - 3][enemy->entity.blocPos.y - 2] < 0)
								{
									canPass = enemy->entity.blocPos.y - 3 >= 0;
									if (canPass && instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x - 3][enemy->entity.blocPos.y - 3] < 0)
									{
										enemy->entity.body.vel.y = -(enemy->jumpForce + 200.f);
										enemy->entity.isOnGround = sfFalse;
										enemy->entity.body.Enablegravity = sfTrue;
									}
									else if (!canPass)
									{
										enemy->entity.body.vel.y = -(enemy->jumpForce + 200.f);
										enemy->entity.isOnGround = sfFalse;
										enemy->entity.body.Enablegravity = sfTrue;
									}
								}
								else if (!canPass)
								{
									enemy->entity.body.vel.y = -(enemy->jumpForce + 200.f);
									enemy->entity.isOnGround = sfFalse;
									enemy->entity.body.Enablegravity = sfTrue;
								}
							}
							else
							{
								enemy->entity.body.vel.y = -enemy->jumpForce;
								enemy->entity.isOnGround = sfFalse;
								enemy->entity.body.Enablegravity = sfTrue;
							}
						}
						else if(instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x - 3][enemy->entity.blocPos.y + 1] >= 0)
						{
							enemy->entity.body.vel.y = -enemy->jumpForce;
							enemy->entity.isOnGround = sfFalse;
							enemy->entity.body.Enablegravity = sfTrue;
						}
					}
				}
			}
		}
		else if (enemy->moveDir > 0)
		{
			sfBool canPass = enemy->entity.blocPos.x + 1 < instanceMaps.maps[indexMap].width;
			if (canPass && instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x + 1][enemy->entity.blocPos.y] < 0)
			{
				canPass = enemy->entity.blocPos.y + 1 < instanceMaps.maps[indexMap].height;
				if (canPass && instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x + 1][enemy->entity.blocPos.y + 1] < 0)
				{
					canPass = enemy->entity.blocPos.x + 3 < instanceMaps.maps[indexMap].width;
					if (canPass)
					{
						if (instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x + 1][enemy->entity.blocPos.y] >= 0)
						{
							canPass = enemy->entity.blocPos.y - 1 >= 0;
							if (canPass && instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x + 3][enemy->entity.blocPos.y - 1] >= 0)
							{
								canPass = enemy->entity.blocPos.y - 2 >= 0;
								if (canPass && instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x + 3][enemy->entity.blocPos.y - 2] < 0)
								{
									canPass = enemy->entity.blocPos.y - 3 >= 0;
									if (canPass && instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x + 3][enemy->entity.blocPos.y - 3] < 0)
									{
										enemy->entity.body.vel.y = -(enemy->jumpForce + 200.f);
										enemy->entity.isOnGround = sfFalse;
										enemy->entity.body.Enablegravity = sfTrue;
									}
									else if (!canPass)
									{
										enemy->entity.body.vel.y = -(enemy->jumpForce + 200.f);
										enemy->entity.isOnGround = sfFalse;
										enemy->entity.body.Enablegravity = sfTrue;
									}
								}
								else if (!canPass)
								{
									enemy->entity.body.vel.y = -(enemy->jumpForce + 200.f);
									enemy->entity.isOnGround = sfFalse;
									enemy->entity.body.Enablegravity = sfTrue;
								}
							}
							else
							{
								enemy->entity.body.vel.y = -enemy->jumpForce;
								enemy->entity.isOnGround = sfFalse;
								enemy->entity.body.Enablegravity = sfTrue;
							}
						}
						else if (instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x + 3][enemy->entity.blocPos.y + 1] >= 0)
						{
							enemy->entity.body.vel.y = -enemy->jumpForce;
							enemy->entity.isOnGround = sfFalse;
							enemy->entity.body.Enablegravity = sfTrue;
						}
					}
				}
			}
		}
	}
}

void Heavy_Movements(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance

	float playerDistance = getDistance(*enemy->playerPos, enemy->entity.body.pos);

	if (enemy->entity.moveDirActive)
	{
		enemy->entity.body.vel.x = enemy->speedMove * (enemy->moveDir * enemy->entity.moveDir.x);
		enemy->entity.body.vel.y = enemy->speedMove * enemy->moveDir * enemy->entity.moveDir.y;
	}
	else
	{
		enemy->entity.body.vel.x = enemy->speedMove * enemy->moveDir;
	}

	enemy->entity.dir = enemy->moveDir;

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (enemy->isBoundsSet && (enemy->entity.body.pos.x > enemy->rightBound.x + 5 || enemy->entity.body.pos.x < enemy->leftBound.x - 5))
	{
		enemy->hasSpawn = sfFalse;
		enemy->isBoundsSet = sfFalse;
		enemy->leftBound.x = -1;
		enemy->rightBound.x = -1;
		return;
	}
	if (enemy->moveDir > 0)
	{
		if (enemy->entity.body.pos.x >= enemy->rightBound.x)
		{
			enemy->entity.body.pos.x = enemy->rightBound.x;
			enemy->moveDir = -abs(enemy->moveDir);

			enemy->idleTime = 1.f + (float)(rand() % 21) / 10.0f; // set the idle state timer ( time of the idle State )
			enemy->currentIdleTime = enemy->idleTime;

			enemy->previousState = enemy->currentState;
			enemy->currentState = E_Idle;
			enemy->animState = E_Anim_Idle;
			enemy->entity.body.vel.x = 0.0f;
			enemy->entity.body.vel.y = 0.0f;
		}
		else if (enemy->entity.hasHitWall)
		{
			enemy->rightBound = enemy->entity.body.pos;
			enemy->entity.body.pos = enemy->rightBound;
			enemy->moveDir = -abs(enemy->moveDir);
			enemy->leftBound.x = -1;
			enemy->isBoundsSet = sfFalse;
		}
	}
	else if (enemy->moveDir < 0)
	{
		if (enemy->entity.body.pos.x <= enemy->leftBound.x)
		{
			enemy->entity.body.pos.x = enemy->leftBound.x;
			enemy->moveDir = abs(enemy->moveDir);

			enemy->idleTime = 1.f + (float)(rand() % 21) / 10.0f; // set the idle state timer ( time of the idle State )
			enemy->currentIdleTime = enemy->idleTime;

			enemy->previousState = enemy->currentState;
			enemy->currentState = E_Idle;
			enemy->animState = E_Anim_Idle;
			enemy->entity.body.vel.x = 0.0f;
			enemy->entity.body.vel.y = 0.0f;
		}
		else if (enemy->entity.hasHitWall)
		{
			enemy->leftBound = enemy->entity.body.pos;
			enemy->entity.body.pos = enemy->entity.body.pos;
			enemy->moveDir = abs(enemy->moveDir);
			enemy->rightBound.x = -1;
			enemy->isBoundsSet = sfFalse;
		}
	}

	if (playerDistance <= enemy->detectionRange	&& PlayerIsVisible(enemy, map, instanceMaps, indexMap))
	{
		int testDir = enemy->playerPos->x - enemy->entity.body.pos.x;

		if (IsSameSign((float)testDir, (float)enemy->entity.dir))
		{
			enemy->previousState = enemy->currentState;
			enemy->currentState = E_LookAtTarget;
			enemy->animState = E_Anim_Idle;
			enemy->entity.body.vel.x = 0.0f;
			enemy->entity.body.vel.y = 0.0f;
		}
	}
}

void Heavy_GetBounds(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance

	float playerDistance = getDistance(*enemy->playerPos, enemy->entity.body.pos);

	if (enemy->entity.moveDirActive)
	{
		enemy->entity.body.vel.x = enemy->speedMove * (enemy->moveDir * enemy->entity.moveDir.x);
		enemy->entity.body.vel.y = enemy->speedMove * enemy->moveDir * enemy->entity.moveDir.y;
	}
	else
	{
		enemy->entity.body.vel.x = enemy->speedMove * enemy->moveDir;
	}

	enemy->entity.dir = enemy->moveDir;

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (enemy->entity.blocID > -1)
	{
		if (enemy->moveDir > 0 && enemy->rightBound.x == -1)
		{
			if (enemy->entity.blocPos.x + 1 == instanceMaps.maps[indexMap].width)
			{
				SetBound(enemy);
			}
			else if (instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x + 1][enemy->entity.blocPos.y] == -1)
			{
				if (!IsChainedBlock(*enemy, map, instanceMaps, enemy->moveDir))
				{
					SetBound(enemy);
				}
			}
			else if (enemy->entity.hasHitWall)
			{
				enemy->rightBound = enemy->entity.body.pos;
				enemy->entity.body.pos = enemy->rightBound;
				enemy->moveDir = -abs(enemy->moveDir);
			}
		}
		else if (enemy->moveDir < 0 && enemy->leftBound.x == -1)
		{
			if (enemy->entity.blocPos.x == 0)
			{
				SetBound(enemy);
			}
			else if (instanceMaps.maps[indexMap].colliderID[enemy->entity.blocPos.x - 1][enemy->entity.blocPos.y] == -1)
			{
				if (!IsChainedBlock(*enemy, map, instanceMaps, enemy->moveDir))
				{
					SetBound(enemy);
				}
			}
			else if (enemy->entity.hasHitWall)
			{
				enemy->leftBound = enemy->entity.body.pos;
				enemy->entity.body.pos = enemy->entity.body.pos;
				enemy->moveDir = abs(enemy->moveDir);
			}
		}
	}

	if (enemy->leftBound.x > -1 && enemy->rightBound.x > -1)
	{
		enemy->isBoundsSet = sfTrue;
	}

	if (playerDistance <= enemy->detectionRange	&& PlayerIsVisible(enemy, map, instanceMaps, indexMap))
	{
		int testDir = enemy->playerPos->x - enemy->entity.body.pos.x;

		if (IsSameSign((float)testDir, (float)enemy->entity.dir))
		{
			enemy->isBoundsSet = sfFalse;
			enemy->leftBound.x = -1;
			enemy->rightBound.x = -1;

			enemy->animInfo[enemy->animState].timer = 0.0f;
			enemy->animInfo[enemy->animState].actualImage = 0;

			enemy->previousState = enemy->currentState;
			enemy->currentState = E_LookAtTarget;
			enemy->animState = E_Anim_Idle;
			enemy->entity.body.vel.y = 0.0f;
			enemy->entity.body.vel.x = 0.0f;
		}
		else if (playerDistance <= (enemy->detectionRange - SIZE_TILE))
		{
			enemy->isBoundsSet = sfFalse;
			enemy->leftBound.x = -1;
			enemy->rightBound.x = -1;

			enemy->animInfo[enemy->animState].timer = 0.0f;
			enemy->animInfo[enemy->animState].actualImage = 0;

			enemy->previousState = enemy->currentState;
			enemy->currentState = E_LookAtTarget;
			enemy->animState = E_Anim_Idle;
			enemy->entity.body.vel.x = 0.0f;
			enemy->entity.body.vel.y = 0.0f;
		}
	}
}

void Heavy_Patrol(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance
	if (!enemy->hasSpawn)
	{
		UpdateEntity(&enemy->entity.body, dt);
		Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

		if (enemy->entity.isOnGround)
		{
			enemy->hasSpawn = sfTrue;
		}
	}
	else
	{
		if (!enemy->isBoundsSet)
		{
			Heavy_GetBounds(enemy, map, instanceMaps, dt);
		}
		else if (enemy->entity.blocID > -1)
		{
			Heavy_Movements(enemy, map, instanceMaps, dt);
		}
	}
}

void Heavy_LookAt(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance

	float playerDistance = getDistance(*enemy->playerPos, enemy->entity.body.pos);
	float playerDistY = GetAxeDistance(enemy->playerPos->y, enemy->entity.body.pos.y);

	// Look at timer update
	if (enemy->currentLookTime > 0.0f)
	{
		enemy->currentLookTime -= dt;
	}

	// Update the look dir of the enemy according to the pos of the player
	if (enemy->playerPos->x < enemy->entity.body.pos.x)
	{
		enemy->entity.dir = -1;
		enemy->moveDir = -1;
	}
	else if (enemy->playerPos->x > enemy->entity.body.pos.x)
	{
		enemy->entity.dir = 1;
		enemy->moveDir = 1;
	}

	if (playerDistance > enemy->detectionRange || !PlayerIsVisible(enemy, map, instanceMaps, indexMap))
	{
		enemy->previousState = enemy->currentState;

		enemy->animInfo[enemy->animState].timer = 0.0f;
		enemy->animInfo[enemy->animState].actualImage = 0;

		enemy->currentState = E_Patrol;
		enemy->animState = E_Anim_Walk;
		enemy->currentLookTime = enemy->lookAtTimer;
	}
	else if (enemy->currentLookTime <= 0.0f && (playerDistY <= (SIZE_TILE * 3)))
	{
		enemy->attackArray[0].atksDatas[0].startPos = enemy->entity.body.pos;
		enemy->previousState = enemy->currentState;

		enemy->animInfo[enemy->animState].timer = 0.0f;
		enemy->animInfo[enemy->animState].actualImage = 0;

		enemy->currentState = E_MoveToTarget;
		enemy->animState = E_Anim_Walk;
		enemy->currentLookTime = enemy->lookAtTimer;
	}
}

void Heavy_Idle(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance

	float playerDistance = getDistance(*enemy->playerPos, enemy->entity.body.pos);

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (playerDistance <= enemy->detectionRange && PlayerIsVisible(enemy, map, instanceMaps, indexMap))
	{
		int testDir = enemy->playerPos->x - enemy->entity.body.pos.x;

		if (IsSameSign((float)testDir, (float)enemy->entity.dir))
		{
			enemy->currentIdleTime = enemy->idleTime;

			enemy->previousState = enemy->currentState;
			enemy->currentState = E_LookAtTarget;
			enemy->animState = E_Anim_Idle;
			return;
		}
	}

	enemy->currentIdleTime -= dt;

	if (enemy->currentIdleTime <= 0.0f)
	{
		enemy->currentIdleTime = enemy->idleTime;
		enemy->currentState = enemy->previousState;
		enemy->previousState = E_Idle;
		enemy->animState = enemy->currentState;
	}
}

void Heavy_MoveTo(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance

	float playerDistance = getDistance(*enemy->playerPos, enemy->entity.body.pos);
	float playerDistY = GetAxeDistance(enemy->playerPos->y, enemy->entity.body.pos.y);

	if (playerDistance > enemy->attackArray[0].atksDatas[0].attackRange)
	{
		if (enemy->entity.isOnGround)
		{
			if (enemy->playerPos->x > enemy->entity.body.pos.x)
			{
				enemy->moveDir = abs(enemy->moveDir);
			}
			else
			{
				enemy->moveDir = -abs(enemy->moveDir);
			}

			enemy->entity.dir = enemy->moveDir;

			if (enemy->entity.moveDirActive)
			{
				enemy->entity.body.vel.x = enemy->moveToSpeed * (enemy->moveDir * enemy->entity.moveDir.x);
				enemy->entity.body.vel.y = enemy->moveToSpeed * (enemy->moveDir * enemy->entity.moveDir.y);
			}
			else
			{
				enemy->entity.body.vel.x = enemy->moveToSpeed * enemy->moveDir;
			}

			//----------------------------------------------------------------------------------------------------- JUMP ------------------
			Heavy_JumpChecks(enemy, map, instanceMaps);
			if (!enemy->entity.isOnGround)
			{
				enemy->currentState = E_Jump;
				return;
			}
			//----------------------------------------------------------------------------------------------------- END JUMP ------------------

		}
	}
	else
	{
		enemy->entity.body.vel.x = 0.0f;
	}

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (enemy->isBoundsSet && (enemy->entity.body.pos.x > enemy->rightBound.x || enemy->entity.body.pos.x < enemy->leftBound.x))
	{
		enemy->hasSpawn = sfFalse;
		enemy->isBoundsSet = sfFalse;
		enemy->leftBound.x = -1;
		enemy->rightBound.x = -1;
		enemy->entity.body.vel.x = 0.0f;
	}

	if (playerDistance > enemy->detectionRange || !PlayerIsVisible(enemy, map, instanceMaps, indexMap) && playerDistY > SIZE_TILE * 2.5f)
	{
		enemy->previousState = enemy->currentState;
		enemy->currentState = E_Patrol;
		enemy->animState = E_Anim_Walk;
	}
	else if (playerDistance <= enemy->attackArray[0].atksDatas[0].attackRange)
	{
		enemy->previousState = enemy->currentState;
		enemy->entity.body.vel.x = 0.0f;
		enemy->entity.body.vel.y = 0.0f;
		enemy->currentState = E_Attack;
		enemy->animState = E_Anim_Idle;
		enemy->attackActiveID = 0;
	}
}

void Heavy_Attack_1(Enemy* enemy, void** null, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance
	float playerDistance = getDistance(*enemy->playerPos, enemy->entity.body.pos);

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (enemy->animState == E_Anim_Attack_1)
	{
		if ((enemy->animInfo[enemy->animState].timer == 0.f) && (enemy->animInfo[enemy->animState].actualImage == enemy->animInfo[enemy->animState].nbImage / 2))
		{
			enemy->attackArray[0].atksDatas[enemy->attackArray[0].atkID].hasHit = sfFalse;
			enemy->attackArray[0].atkID++;
			printf("Enter\n");
		}

		if (enemy->animInfo[enemy->animState].timer == 0.0f && enemy->animInfo[enemy->animState].actualImage == 0)
		{
			printf("Enter 3\n");
			printf("atk ID : %d\n", enemy->attackArray[0].atkID);
			if (enemy->attackArray[0].atkID == enemy->attackArray[0].atkNB -1)
			{
				enemy->attackArray[0].atksDatas[enemy->attackArray[0].atkID].hasHit = sfFalse;
				enemy->attackArray[0].atkID = 0;
				enemy->attackArray[0].canBeUse = sfFalse;

				enemy->animInfo[enemy->animState].timer = 0.0f;
				enemy->animInfo[enemy->animState].actualImage = 0;

				enemy->animState = E_Anim_Idle;
			}
		}
	}
	else
	{
		if (enemy->attackArray[0].canBeUse)
		{
			enemy->animState = E_Anim_Attack_1;
		}
	}

	if (playerDistance > enemy->attackArray[0].atksDatas[enemy->attackArray[0].atkID].attackRange)
	{
		enemy->attackArray[0].atksDatas[enemy->attackArray[0].atkID].hasHit = sfFalse;
		enemy->attackArray[0].atkID = 0;
		enemy->attackArray[0].canBeUse = sfFalse;

		enemy->animInfo[enemy->animState].timer = 0.0f;
		enemy->animInfo[enemy->animState].actualImage = 0;

		enemy->currentState = E_MoveToTarget;
		enemy->animState = E_Anim_Walk;
	}
}

void Heavy_Attack_2(Enemy* enemy, struct Enemy** firstEnemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance
	if (enemy->entity.moveDirActive)
	{
		enemy->entity.body.vel.x = enemy->attackArray[0].atksDatas[0].chargeMoveSpeed * (enemy->moveDir * enemy->entity.moveDir.x);
		enemy->entity.body.vel.y = enemy->attackArray[0].atksDatas[0].chargeMoveSpeed * (enemy->moveDir * enemy->entity.moveDir.y);
	}
	else
	{
		enemy->entity.body.vel.x = enemy->attackArray[0].atksDatas[0].chargeMoveSpeed * enemy->moveDir;
	}

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	Update_Charge_Attack(&enemy->attackArray[0], enemy->entity.body.pos, enemy->type, enemy->moveDir, dt); // ATTACK update

	if (enemy->isBoundsSet && (enemy->entity.body.pos.x > enemy->rightBound.x || enemy->entity.body.pos.x < enemy->leftBound.x))
	{
		enemy->hasSpawn = sfFalse;
		enemy->isBoundsSet = sfFalse;
		enemy->leftBound.x = -1;
		enemy->rightBound.x = -1;
	}

	RunnerJump(enemy, map, instanceMaps);

	if (enemy->entity.hasHitWall || enemy->attackArray[0].atksDatas[0].hasHit)
	{
		if (enemy->attackArray[0].atksDatas[0].hasHit)
		{
			enemy->attackArray[0].atksDatas[0].hasHit = sfFalse;
		}
		enemy->attackArray[0].canBeUse = sfFalse;
		enemy->attackArray[0].atksDatas[0].damage = 0;

		enemy->stunTime = 1.5f;
		enemy->currentStunTime = enemy->stunTime;

		enemy->previousState = E_Patrol;
		enemy->currentState = E_Stun;
		enemy->animState = E_Anim_HitSomething;
		enemy->entity.body.vel.x = 0.0f;
		enemy->entity.body.vel.y = 0.0f;
	}
	else if (!enemy->entity.isOnGround)
	{
		enemy->previousState = E_Patrol;
		enemy->currentState = E_Jump;
	}
}

void Heavy_Jump(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (enemy->entity.isOnGround)
	{
		enemy->previousState = enemy->currentState;
		enemy->currentState = E_MoveToTarget;
		enemy->animState = E_Anim_Walk;
	}
	
}

void Heavy_Stun(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance
	enemy->entity.body.vel.x = 0.0f;

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	enemy->currentStunTime -= dt;

	if (enemy->currentStunTime <= 0.0f)
	{
		enemy->currentStunTime = enemy->stunTime;
		enemy->currentState = enemy->previousState;
		enemy->previousState = E_Stun;
		enemy->animState = E_Anim_Walk;
	}
}

void Heavy_Invincible(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance
	if (enemy->invincibilityTime - enemy->currentInvicibleTime < enemy->repulseTime)
	{
		if (enemy->entity.moveDirActive)
		{
			enemy->entity.body.vel.x = enemy->repulseSpeed * (enemy->moveDir * -1 * enemy->entity.moveDir.x);
			enemy->entity.body.vel.y = enemy->repulseSpeed * (enemy->moveDir * -1 * enemy->entity.moveDir.y);
		}
		else
		{
			enemy->entity.body.vel.x = enemy->repulseSpeed * enemy->moveDir * -1;
		}
	}
	else
	{
		enemy->entity.body.vel.x = 0.f;
		if (enemy->entity.isOnGround)
		{
			enemy->entity.body.vel.y = 0.f;
		}
	}

	enemy->currentInvicibleTime -= dt;

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (enemy->currentInvicibleTime <= 0)
	{
		enemy->currentState = E_Patrol;
		enemy->animState = E_Anim_Walk;
	}
}

void Heavy_Death(Enemy* enemy, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	int indexMap = instanceMaps.actualMap; // ID of the current map in the instance
	enemy->entity.body.vel.x = 0.00f;
	enemy->currentInvicibleTime -= dt;

	UpdateEntity(&enemy->entity.body, dt);
	Entity2DMapCollision(&enemy->entity, map, instanceMaps, indexMap);

	if (enemy->currentInvicibleTime <= 0)
	{
		enemy->entity.body.vel.y = -1000.f;
		enemy->entity.body.Enablegravity = sfTrue;
		enemy->entity.isOnGround = sfFalse;
		enemy->currentState = E_Dead;
		// Spawn Item;
	}
}
#pragma endregion

#pragma endregion

// -------------------------------- ENEMIES CREATIONS --------------------------------
#pragma region ENEMIES CREATIONS

void CreateRunner(struct Enemy** first, sfVector2f position, sfVector2f* playerPos, float difficultyMultiplier)
{
	int tempRand;
	Enemy tempEnemy;

	tempEnemy.entity.body = RigidbodyLoader(position, 1, sfTrue);
	tempEnemy.entity.isOnGround = sfFalse;
	tempEnemy.entity.hasHitWall = sfFalse;
	tempEnemy.entity.moveDirActive = sfFalse;
	tempEnemy.entity.blocID = -1;

	tempEnemy.entity.size.x = 80;
	tempEnemy.entity.size.y = 150;

	float offsetX = tempEnemy.entity.size.x / 2;

	tempEnemy.entity.bodyCollider.topC.center.x = tempEnemy.entity.body.pos.x;
	tempEnemy.entity.bodyCollider.topC.center.y = tempEnemy.entity.body.pos.y + tempEnemy.entity.size.y - offsetX;
	tempEnemy.entity.bodyCollider.topC.radius = offsetX;

	tempEnemy.entity.bodyCollider.rect.top = tempEnemy.entity.bodyCollider.topC.center.y;
	tempEnemy.entity.bodyCollider.rect.left = tempEnemy.entity.body.pos.x - (offsetX);
	tempEnemy.entity.bodyCollider.rect.height = tempEnemy.entity.size.y - tempEnemy.entity.size.x;
	tempEnemy.entity.bodyCollider.rect.width = (offsetX * 2);

	int index = 0;
	for (int i = -1; i < 2; i++)
	{
		tempEnemy.entity.bodyCollider.bottomPoints[index].x = tempEnemy.entity.body.pos.x + offsetX*i - 5 * i;
		tempEnemy.entity.bodyCollider.bottomPoints[index].y = tempEnemy.entity.body.pos.y;
		index++;
	}

	tempEnemy.entity.moveDir.x = 1;
	tempEnemy.entity.moveDir.y = 0;

	tempEnemy.entity.middlePos.x = tempEnemy.entity.body.pos.x;
	tempEnemy.entity.middlePos.y = tempEnemy.entity.body.pos.y - tempEnemy.entity.size.y / 2;

	tempEnemy.type = E_Runner;
	tempEnemy.previousState = E_Patrol;
	tempEnemy.currentState = E_Patrol;
	tempEnemy.animState = E_Anim_Walk;

	tempEnemy.rightBound.x = -1;
	tempEnemy.leftBound.x = -1;

	tempEnemy.playerPos = playerPos;

	tempEnemy.speedMove = 200.0f;
	tempEnemy.jumpForce = 800.0f;

	tempEnemy.detectionRange = 900.0f + rand() % 200;

	// -- TIMER -- \\

	tempEnemy.lookAtTimer = 2.0f;
	tempEnemy.currentLookTime = tempEnemy.lookAtTimer;

	tempEnemy.idleTime = 1.0f;
	tempEnemy.currentIdleTime = tempEnemy.idleTime;

	// --       -- \\

	tempRand = rand() % 2;
	tempEnemy.moveDir = tempRand == 0 ? -1 : tempRand;
	tempEnemy.entity.dir = tempEnemy.moveDir;

	tempEnemy.life = E_RUNNER_LIFE_DIFF1 * difficultyMultiplier;

	//  -- Init attacks -- 
	float atkRange = 700.0f + rand() % 100;
	int damage = 30 * difficultyMultiplier;
	sfVector2f baseVelToAdd = {250.f, 0.f};
	tempEnemy.attacksNB = 1;
	tempEnemy.attackArray = (EnemyAttack*)malloc(sizeof(EnemyAttack) * tempEnemy.attacksNB);
	tempEnemy.attackArray[0] = CreateAttack(1, 3.f);
	tempEnemy.attackArray[0].atksDatas[0] = Create_Charge_Attack(atkRange, 1300.f, baseVelToAdd, damage);

	tempEnemy.attackActiveID = 0;

	//  -- Init Booleans -- 
	tempEnemy.isBoundsSet = sfFalse;
	tempEnemy.hasSpawn = sfFalse;
	tempEnemy.hasLoot = sfFalse;

	//  -- Init function pointer -- 
	tempEnemy.takeDamages = &TakeDamages; // Take Damages Function
	tempEnemy.patrol = &Runner_Patrol;
	tempEnemy.lookAt = &Runner_LookAt;
	tempEnemy.idle = &Runner_Idle;
	tempEnemy.moveTo = NULL;
	tempEnemy.attack_1 = &Runner_Attack;
	tempEnemy.attack_2 = NULL;
	tempEnemy.jump = &Runner_Jump;
	tempEnemy.stun = &Runner_Stun;
	tempEnemy.invincible = &Runner_Invincible;
	tempEnemy.escape = NULL;
	tempEnemy.death = &Runner_Death;

	AddEnemy(tempEnemy, first);
}

void CreateRanger(struct Enemy** first, sfVector2f position, sfVector2f* playerPos, int type, float difficultyMultiplier)
{
	int tempRand;
	Enemy tempEnemy;

	tempEnemy.entity.body = RigidbodyLoader(position, 10, sfTrue);
	tempEnemy.entity.isOnGround = sfFalse;
	tempEnemy.entity.hasHitWall = sfFalse;
	tempEnemy.entity.moveDirActive = sfFalse;
	tempEnemy.entity.blocID = -1;

	tempEnemy.entity.size.x = 80;
	tempEnemy.entity.size.y = 200;

	float offsetX = tempEnemy.entity.size.x / 2;

	tempEnemy.entity.bodyCollider.topC.center.x = tempEnemy.entity.body.pos.x;
	tempEnemy.entity.bodyCollider.topC.center.y = tempEnemy.entity.body.pos.y + tempEnemy.entity.size.y - offsetX;
	tempEnemy.entity.bodyCollider.topC.radius = offsetX;

	tempEnemy.entity.bodyCollider.rect.top = tempEnemy.entity.bodyCollider.topC.center.y;
	tempEnemy.entity.bodyCollider.rect.left = tempEnemy.entity.body.pos.x - (offsetX);
	tempEnemy.entity.bodyCollider.rect.height = tempEnemy.entity.size.y - tempEnemy.entity.size.x;
	tempEnemy.entity.bodyCollider.rect.width = (offsetX * 2);

	int index = 0;
	for (int i = -1; i < 2; i++)
	{
		tempEnemy.entity.bodyCollider.bottomPoints[index].x = tempEnemy.entity.body.pos.x + offsetX*i - 5 * i;
		tempEnemy.entity.bodyCollider.bottomPoints[index].y = tempEnemy.entity.body.pos.y;
		index++;
	}

	tempEnemy.entity.moveDir.x = 1;
	tempEnemy.entity.moveDir.y = 0;

	tempEnemy.entity.middlePos.x = tempEnemy.entity.body.pos.x;
	tempEnemy.entity.middlePos.y = tempEnemy.entity.body.pos.y - tempEnemy.entity.size.y / 2;

	tempEnemy.type = E_Ranger;
	tempEnemy.previousState = E_Patrol;
	tempEnemy.currentState = E_Patrol;
	tempEnemy.animState = E_Anim_Walk;

	tempEnemy.rightBound.x = -1;
	tempEnemy.leftBound.x = -1;

	tempEnemy.playerPos = playerPos;

	tempEnemy.speedMove = 450.0f + rand() % 50;
	tempEnemy.jumpForce = 1500.0f;

	tempEnemy.patrolDistance = 450.0f + rand() % 200;

	tempEnemy.detectionRange = 1400.0f + rand() % 100;
	tempEnemy.escapeRange = 500.0f + rand() % 100;

	// -- TIMER -- \\

	tempEnemy.lookAtTimer = 2.0f;
	tempEnemy.currentLookTime = tempEnemy.lookAtTimer;

	tempEnemy.idleTime = 1.0f;
	tempEnemy.currentIdleTime = tempEnemy.idleTime;

	// --       -- \\

	tempRand = rand() % 2;
	tempEnemy.moveDir = tempRand == 0 ? -1 : tempRand;
	tempEnemy.entity.dir = tempEnemy.moveDir;

	tempEnemy.life = E_RANGER_LIFE_DIFF1 * difficultyMultiplier;

	tempEnemy.patrolStateNB = 2;
	tempEnemy.patrolID = type;

	//  -- Init attacks -- \\

	float atk1Range = 1000.f + rand() % 151;
	sfVector2f baseVelToAdd = { 800.f, -1000.f };
	int damage1 = 15 * difficultyMultiplier;
	int damage2 = 20 * difficultyMultiplier;

	tempEnemy.attacksNB = 2;
	tempEnemy.attackArray = (EnemyAttack*)malloc(sizeof(EnemyAttack) * tempEnemy.attacksNB);

	tempEnemy.attackArray[0] = CreateAttack(1, 2.f);
	tempEnemy.attackArray[0].atksDatas[0] = Create_Distance_Attack(atk1Range, damage1);

	tempEnemy.attackArray[1] = CreateAttack(1, 3.5f);
	tempEnemy.attackArray[1].atksDatas[0] = Create_Charge_Attack(500.f, 580.f, baseVelToAdd, damage2);
	tempEnemy.attackArray[1].atksDatas[0].stunTime = 0.5f;

	tempEnemy.attackActiveID = 0;

	//  -- Init Booleans -- \\

	tempEnemy.isBoundsSet = sfFalse;
	tempEnemy.hasSpawn = sfFalse;
	tempEnemy.isBlocked_R = sfFalse;
	tempEnemy.isBlocked_L = sfFalse;
	tempEnemy.hasLoot = sfFalse;

	//  -- Init function pointer -- \\

	tempEnemy.takeDamages = &TakeDamages; // Take Damages Function
	tempEnemy.patrol = &Ranger_Patrol;
	tempEnemy.lookAt = &Ranger_LookAt;
	tempEnemy.idle = &Ranger_Idle;
	tempEnemy.moveTo = NULL;
	tempEnemy.attack_1 = &Ranger_Attack_Dist;
	tempEnemy.attack_2 = &Ranger_Attack_Repuls;
	tempEnemy.jump = &Ranger_Jump;
	tempEnemy.stun = &Ranger_Stun;
	tempEnemy.invincible = &Ranger_Invincible;
	tempEnemy.escape = &Ranger_Escape;
	tempEnemy.death = &Ranger_Death;

	AddEnemy(tempEnemy, first);
}

void CreateHeavy(struct Enemy** first, sfVector2f position, sfVector2f* playerPos, float difficultyMultiplier)
{
	int tempRand;
	Enemy tempEnemy;

	tempEnemy.entity.body = RigidbodyLoader(position, 10, sfTrue);
	tempEnemy.entity.isOnGround = sfFalse;
	tempEnemy.entity.hasHitWall = sfFalse;
	tempEnemy.entity.moveDirActive = sfFalse;
	tempEnemy.entity.blocID = -1;

	tempEnemy.entity.size.x = 80;
	tempEnemy.entity.size.y = 200;

	float offsetX = tempEnemy.entity.size.x / 2;

	tempEnemy.entity.bodyCollider.topC.center.x = tempEnemy.entity.body.pos.x;
	tempEnemy.entity.bodyCollider.topC.center.y = tempEnemy.entity.body.pos.y + tempEnemy.entity.size.y - offsetX;
	tempEnemy.entity.bodyCollider.topC.radius = offsetX;

	tempEnemy.entity.bodyCollider.rect.top = tempEnemy.entity.bodyCollider.topC.center.y;
	tempEnemy.entity.bodyCollider.rect.left = tempEnemy.entity.body.pos.x - (offsetX);
	tempEnemy.entity.bodyCollider.rect.height = tempEnemy.entity.size.y - tempEnemy.entity.size.x;
	tempEnemy.entity.bodyCollider.rect.width = (offsetX * 2);

	int index = 0;
	for (int i = -1; i < 2; i++)
	{
		tempEnemy.entity.bodyCollider.bottomPoints[index].x = tempEnemy.entity.body.pos.x + offsetX * i - 5 * i;
		tempEnemy.entity.bodyCollider.bottomPoints[index].y = tempEnemy.entity.body.pos.y;
		index++;
	}

	tempEnemy.entity.moveDir.x = 1;
	tempEnemy.entity.moveDir.y = 0;

	tempEnemy.entity.middlePos.x = tempEnemy.entity.body.pos.x;
	tempEnemy.entity.middlePos.y = tempEnemy.entity.body.pos.y - tempEnemy.entity.size.y / 2;

	tempEnemy.type = E_Heavy;
	tempEnemy.previousState = E_Patrol;
	tempEnemy.currentState = E_Patrol;
	tempEnemy.animState = E_Anim_Walk;

	tempEnemy.rightBound.x = -1;
	tempEnemy.leftBound.x = -1;

	tempEnemy.playerPos = playerPos;

	tempEnemy.speedMove = 450.0f + rand() % 50;
	tempEnemy.moveToSpeed = 800.0f + rand() % 51;
	tempEnemy.jumpForce = 1500.0f;

	tempEnemy.detectionRange = 1400.0f + rand() % 100;
	tempEnemy.escapeRange = 500.0f + rand() % 100;

	// -- TIMER -- \\

	tempEnemy.lookAtTimer = 1.5f;
	tempEnemy.currentLookTime = tempEnemy.lookAtTimer;

	tempEnemy.idleTime = 1.0f;
	tempEnemy.currentIdleTime = tempEnemy.idleTime;

	// --       -- \\

	tempRand = rand() % 2;
	tempEnemy.moveDir = tempRand == 0 ? -1 : tempRand;
	tempEnemy.entity.dir = tempEnemy.moveDir;

	tempEnemy.life = E_HEAVY_LIFE_DIFF1 * difficultyMultiplier;

	tempEnemy.patrolStateNB = 1;

	//  -- Init attacks -- \\

	float atk1Range = 180.f + rand() % 50;
	sfVector2f baseVelToAdd = { 800.f, -1000.f };
	
	tempEnemy.attacksNB = 2;
	tempEnemy.attackArray = (EnemyAttack*)malloc(sizeof(EnemyAttack) * tempEnemy.attacksNB);

	tempEnemy.attackArray[0] = CreateAttack(2, 2.0f);
	tempEnemy.attackArray[0].atksDatas[0] = Create_CaC_Attack(atk1Range, sfFalse, 15 * difficultyMultiplier);
	tempEnemy.attackArray[0].atksDatas[1] = Create_CaC_Attack(atk1Range, sfFalse, 15 * difficultyMultiplier);

	tempEnemy.attackArray[1] = CreateAttack(1, 3.5f);
	tempEnemy.attackArray[1].atksDatas[0] = Create_Distance_Attack(500.f, 20 * difficultyMultiplier);
	tempEnemy.attackArray[1].atksDatas[0].stunTime = 0.5f;

	tempEnemy.attackActiveID = 0;

	//  -- Init Booleans -- \\

	tempEnemy.isBoundsSet = sfFalse;
	tempEnemy.hasSpawn = sfFalse;

	tempEnemy.isBlocked_R = sfFalse;
	tempEnemy.isBlocked_L = sfFalse;
	tempEnemy.hasLoot = sfFalse;

	//  -- Init function pointer -- \\

	tempEnemy.takeDamages = &TakeDamages; // Take Damages Function
	tempEnemy.patrol = &Heavy_Patrol;
	tempEnemy.lookAt = &Heavy_LookAt;
	tempEnemy.idle = &Heavy_Idle;
	tempEnemy.moveTo = &Heavy_MoveTo;
	tempEnemy.attack_1 = &Heavy_Attack_1;
	tempEnemy.attack_2 = &Heavy_Attack_2;
	tempEnemy.jump = &Heavy_Jump;
	tempEnemy.stun = &Heavy_Stun;
	tempEnemy.invincible = &Heavy_Invincible;
	tempEnemy.escape = NULL;
	tempEnemy.death = &Heavy_Death;

	AddEnemy(tempEnemy, first);
}

#pragma endregion

// -------------------------------- COMMONS FUNCTIONS --------------------------------
#pragma region COMMONS FUNCTIONS

void SetEnemyAnimInfo(Enemy* enemy, AnimInfo* animInfo)
{
	//  -- Init animations -- \\

	enemy->animInfo = malloc(sizeof(AnimInfo) * E_Anim_NUMBER);
	enemy->animInfo[E_Anim_Walk] = animInfo[E_Anim_Walk];
	enemy->animInfo[E_Anim_Idle] = animInfo[E_Anim_Idle];
	enemy->animInfo[E_Anim_Attack_1] = animInfo[E_Anim_Attack_1];
	enemy->animInfo[E_Anim_Attack_2] = animInfo[E_Anim_Attack_2];
	enemy->animInfo[E_Anim_Stun] = animInfo[E_Anim_Stun];
	enemy->animInfo[E_Anim_HitSomething] = animInfo[E_Anim_HitSomething];
	enemy->animInfo[E_Anim_TakeDamage] = animInfo[E_Anim_TakeDamage];
}
void EnemyDead(Enemy* enemy, float dt)
{
	UpdateEntity(&enemy->entity.body, dt);
}
void UpdateEnemies(struct Enemy** first, struct Projectile** firstProj, Loot_Item* itemList, CollisionMap map, InstanceMaps instanceMaps, float dt)
{
	if (*first != NULL)
	{
		struct Enemy* enemy = *first;
		struct Enemy* nextEnemy = enemy->ptrNext;

		while (enemy != NULL)
		{
			sfBool outOfMap = sfFalse;
			sfVector2f enemyPos = enemy->entity.body.pos;

			if (enemy->currentState != E_Dead)
			{
				Update_AttackTimer(enemy->attackArray, enemy->attacksNB, dt);

				switch (enemy->currentState)
				{
				case E_Patrol:
					if (enemy->patrol != NULL)
					{
						enemy->patrol(enemy, map, instanceMaps, dt);
					}
					break;
				case E_LookAtTarget:
					if (enemy->lookAt != NULL)
					{
						enemy->lookAt(enemy, map, instanceMaps, dt);
					}
					break;
				case E_Idle:
					if (enemy->idle != NULL)
					{
						enemy->idle(enemy, map, instanceMaps, dt);
					}
					break;
				case E_MoveToTarget:
					if (enemy->moveTo != NULL)
					{
						enemy->moveTo(enemy, map, instanceMaps, dt);
					}
					break;
				case E_Attack:

					if (enemy->type == E_Ranger)
					{
						if (enemy->attackActiveID == 0 && enemy->attack_1 != NULL)
						{
							enemy->attack_1(enemy, firstProj, map, instanceMaps, dt);
						}
						else if (enemy->attackActiveID == 1 && enemy->attack_2 != NULL)
						{
							enemy->attack_2(enemy, NULL, map, instanceMaps, dt);
						}
					}
					else if (enemy->type == E_Runner)
					{
						if (enemy->attack_1 != NULL)
						{
							enemy->attack_1(enemy, NULL, map, instanceMaps, dt);
						}
					}
					else if (enemy->type == E_Heavy)
					{
						if (enemy->attackActiveID == 0 && enemy->attack_1 != NULL)
						{
							enemy->attack_1(enemy, NULL, map, instanceMaps, dt);
						}
						else if (enemy->attackActiveID == 1 && enemy->attack_2 != NULL)
						{
							enemy->attack_2(enemy, NULL, map, instanceMaps, dt);
						}
					}
					break;
				case E_Jump:
					if (enemy->jump != NULL)
					{
						enemy->jump(enemy, map, instanceMaps, dt);
					}
					break;
				case E_Stun:
					if (enemy->stun != NULL)
					{
						enemy->stun(enemy, map, instanceMaps, dt);
					}
					break;
				case E_Invincible:
					if (enemy->invincible != NULL)
					{
						enemy->invincible(enemy, map, instanceMaps, dt);
					}
					break;
				case E_Death:
					if (enemy->death != NULL)
					{
						enemy->death(enemy, map, instanceMaps, dt);
					}
					break;
				case E_Escape:
					if (enemy->escape != NULL)
					{
						enemy->escape(enemy, map, instanceMaps, dt);
					}
					break;
				default:
					break;
				}
				UpdateAnimation(&enemy->animInfo[enemy->animState], NULL, dt);
			}
			else
			{
				EnemyDead(enemy, dt);
				if (enemy != NULL && !enemy->hasLoot)
				{
					int lootRand = 1 + rand() % 100;
					if (lootRand <= 20 && !outOfMap)
					{
						Spawn_Loot(itemList, enemy->entity.middlePos);
					}

					enemy->hasLoot = sfTrue;
				}
			}

			if (enemyPos.y > instanceMaps.maps[instanceMaps.actualMap].height * SIZE_TILE + 1000.f)
			{

				RemoveEnemy(enemy, first);
				enemy = nextEnemy;
			}

			if (enemy != NULL)
			{
				nextEnemy = enemy;
				enemy = enemy->ptrNext;
			}
		}
	}
}

void RenderEnemies(SpritePack sp, struct Enemy** first, Layers* layer)
{
	for (Enemy* enemy = *first;
		enemy != NULL; enemy = enemy->ptrNext)
	{
		int layerNB = 25;

		if (enemy->entity.isOnGround)
		{
			addToLayer(&layer[LAYER_ENEMY].premier, &sp.shadow, enemy->entity.body.pos, DEFAULT_INFOBLIT);
		}

		sfVector2f newScale = { enemy->entity.dir, 1 };
		InfoBlitSprite infoBlitSprite = DEFAULT_INFOBLIT;
		infoBlitSprite.active = sfTrue;
		infoBlitSprite.scale = newScale;
		if (enemy->currentState == E_Dead)
		{
			layerNB = 10;
			infoBlitSprite.color.a = 200;
		}
		if (enemy->type == E_Heavy)
		{
		infoBlitSprite.color = sfRed;
		}

		//addToLayer(&layer[8].premier, &sp.enemiesSprites[enemy->type][enemy->animID].sprite[enemy->animInfo[enemy->animID].actualImage], enemy->entity.body.pos, infoBlitSprite);
		addToLayer(&layer[layerNB].premier, &sp.enemiesSprites[enemy->type][enemy->animState].sprite[enemy->animInfo[enemy->animState].actualImage], enemy->entity.body.pos, infoBlitSprite);
	}
}

#pragma endregion

// -------------------------------- ENEMIES INSTANTIATION --------------------------------
void LoadTiledEnemy(TiledMap map, char area, struct Enemy** first, sfVector2f* playerPos, EnemiesAnimsInfos animsInfos)
{
	float difficulty = 1.0f; // temporary variable for the difficulty of the game
	int num = -1;

	for (int i = 0; i < map.height; i++)
	{
		for (int j = 0; j < map.width; j++)
		{
			int num = map.enemyTile[j][i];

			if (num == 0)
			{
					CreateRunner(first, (sfVector2f) { j*SIZE_TILE, i*SIZE_TILE }, playerPos, difficulty); //  - Runner creation
					SetEnemyAnimInfo(*first, animsInfos.runner);
			}
			else if (num == 1)
			{
				CreateRanger(first, (sfVector2f) { j*SIZE_TILE, i*SIZE_TILE }, playerPos, 1 + rand() % 2, difficulty); //  - Ranger creation ( the type set if the enemy is static or not )
				SetEnemyAnimInfo(*first, animsInfos.ranger);
			}
		}
	}
}
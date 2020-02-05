/*
----------------- TO DO -----------------

Refaire la fonction Jump des ennemis pour qu'elle soit spécifique à chaque ennemi

*/
#include "fightBox.h"

int GetNbFile(char* directory, char* extention)
{
	int nbFile = 0;
	int index = 1;
	char buffer[500] = { 0 };
	FILE* file;
	do
	{
		sprintf(buffer, "%s%d%s", directory, index, extention);
		file = fopen(buffer, "r");
		if (file != NULL)
		{
			nbFile++;
			fclose(file);
		}
		index++;
	} while (file != NULL);

	return nbFile;
}

float GetFloatValue(char* buffer, char* name)
{
	int strOffset = 2 + strlen(name);
	char* tempPtr = strstr(buffer, name) + strOffset;
	char* ptrEnd;
	char bufferValue[50] = { 0 };
	int index = 0;
	while (tempPtr[index] != '"')
	{
		bufferValue[index] = tempPtr[index];
		index++;
	}

	return strtof(bufferValue, &ptrEnd);
}

FightBox LoadFileFightBox(char* Directory, int NbFile)
{
	char fileName[255] = { 0 };
	FightBox fightBox;
	FILE* file;
	char buffer[1000] = { 0 };
	sfVector2f tmpPos = { 0,0 };
	sprintf(fileName, "%s%d.svg", Directory, NbFile);
	int NbIteration = 0;
	file = fopen(fileName, "r");

	if (!file)
	{
		exit(0);
	}


	fightBox.bodyHitBox.type = noBox;
	fightBox.damageHitBox.type = noBox;

	while (fgets(buffer, 1000, file) != NULL)
	{

		// DAMAGE HIT BOX
		if (strstr(buffer, "HitBox") != NULL)
		{
			do
			{
				fgets(buffer, 1000, file);
				NbIteration++;
				if (NbIteration > 15)
				{
					exit(0);
				}
			} while (strstr(buffer, "rect") == NULL && strstr(buffer, "circle") == NULL);
			NbIteration = 0;
			if (strstr(buffer, "rect") != NULL)
			{
				fightBox.bodyHitBox.type = rect;

				fightBox.bodyHitBox.rect.left = GetFloatValue(buffer, "x");
				fightBox.bodyHitBox.rect.top = GetFloatValue(buffer, "y");
				fightBox.bodyHitBox.rect.width = GetFloatValue(buffer, "width");
				fightBox.bodyHitBox.rect.height = GetFloatValue(buffer, "height");
			}
			if (strstr(buffer, "circle") != NULL)
			{
				fightBox.bodyHitBox.type = circle;

				fightBox.bodyHitBox.circle.center.x = GetFloatValue(buffer, "cx");
				fightBox.bodyHitBox.circle.center.y = GetFloatValue(buffer, "cy");
				fightBox.bodyHitBox.circle.radius = GetFloatValue(buffer, "r");
			}
		}

		// DAMAGE HIT BOX
		if (strstr(buffer, "DamageBox") != NULL)
		{
			do
			{
				fgets(buffer, 1000, file);
				NbIteration++;
				if (NbIteration > 15)
				{
					exit(0);
				}
			} while (strstr(buffer, "rect") == NULL && strstr(buffer, "circle") == NULL);
			NbIteration = 0;

			if (strstr(buffer, "rect") != NULL)
			{
				fightBox.damageHitBox.type = rect;

				fightBox.damageHitBox.rect.left = GetFloatValue(buffer, "x");
				fightBox.damageHitBox.rect.top = GetFloatValue(buffer, "y");
				fightBox.damageHitBox.rect.width = GetFloatValue(buffer, "width");
				fightBox.damageHitBox.rect.height = GetFloatValue(buffer, "height");
			}
			if (strstr(buffer, "circle") != NULL)
			{
				fightBox.damageHitBox.type = circle;

				fightBox.damageHitBox.circle.center.x = GetFloatValue(buffer, "cx");
				fightBox.damageHitBox.circle.center.y = GetFloatValue(buffer, "cy");
				fightBox.damageHitBox.circle.radius = GetFloatValue(buffer, "r");
			}
		}
	}
	fclose(file);
	return fightBox;
}

FightBox** LoadPlayerFightBox()
{
	FightBox ** fightBox;
	fightBox = calloc(nbEnumPlayerState, sizeof(FightBox*));
	int nbFile = 0;

	//idle

	nbFile = GetNbFile("Data/Player/Idle/FightBox", ".svg");
	fightBox[pla_idle] = calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[pla_idle][i] = LoadFileFightBox("Data/Player/Idle/FightBox", i + 1);
	}

	//walking
	nbFile = GetNbFile("Data/Player/Walking/FightBox", ".svg");
	fightBox[pla_walking] = calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[pla_walking][i] = LoadFileFightBox("Data/Player/Walking/FightBox", i + 1);
	}

	//crouch
	nbFile = GetNbFile("Data/Player/Walking/FightBox", ".svg"); // PAS LES BONNES FIGHTBOX DE LOAD !!
	fightBox[pla_crouch] = calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[pla_crouch][i] = LoadFileFightBox("Data/Player/Walking/FightBox", i + 1);
	}

	//leaningOnAWall
	nbFile = GetNbFile("Data/Player/Walking/FightBox", ".svg"); // PAS LES BONNES FIGHTBOX DE LOAD !!
	fightBox[pla_leaningOnAWall] = calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[pla_leaningOnAWall][i] = LoadFileFightBox("Data/Player/Walking/FightBox", i + 1);
	}

	//climbing
	nbFile = GetNbFile("Data/Player/Walking/FightBox", ".svg"); // PAS LES BONNES FIGHTBOX DE LOAD !!
	fightBox[pla_climbing] = calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[pla_climbing][i] = LoadFileFightBox("Data/Player/Walking/FightBox", i + 1);
	}

	//impulsion
	nbFile = GetNbFile("Data/Player/Impulsion/FightBox", ".svg");
	fightBox[pla_impulsion] = calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[pla_impulsion][i] = LoadFileFightBox("Data/Player/Impulsion/FightBox", i + 1);
	}

	//rising
	nbFile = GetNbFile("Data/Player/Rising/FightBox", ".svg");
	fightBox[pla_rising] = calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[pla_rising][i] = LoadFileFightBox("Data/Player/Rising/FightBox", i + 1);
	}

	//extremum
	nbFile = GetNbFile("Data/Player/Extremum/FightBox", ".svg");
	fightBox[pla_extremum] = calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[pla_extremum][i] = LoadFileFightBox("Data/Player/Extremum/FightBox", i + 1);
	}

	//falling
	nbFile = GetNbFile("Data/Player/Fall/FightBox", ".svg");
	fightBox[pla_falling] = calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[pla_falling][i] = LoadFileFightBox("Data/Player/Fall/FightBox", i + 1);
	}

	//gliding
	nbFile = GetNbFile("Data/Player/Gliding/FightBox", ".svg");
	fightBox[pla_gliding] = calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[pla_gliding][i] = LoadFileFightBox("Data/Player/Gliding/FightBox", i + 1);
	}

	//landing
	nbFile = GetNbFile("Data/Player/Landing/FightBox", ".svg");
	fightBox[pla_landing] = calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[pla_landing][i] = LoadFileFightBox("Data/Player/Landing/FightBox", i + 1);
	}

	//rolling
	nbFile = GetNbFile("Data/Player/Rolling/FightBox", ".svg");
	fightBox[pla_rolling] = calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[pla_rolling][i] = LoadFileFightBox("Data/Player/Rolling/FightBox", i + 1);
	}

	//stunned
	nbFile = GetNbFile("Data/Player/Idle/FightBox", ".svg");
	fightBox[pla_stunned] = calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[pla_stunned][i] = LoadFileFightBox("Data/Player/Idle/FightBox", i + 1);
	}

	//attack
	nbFile = GetNbFile("Data/Player/Attack/FightBox", ".svg");
	fightBox[pla_attack] = calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[pla_attack][i] = LoadFileFightBox("Data/Player/Attack/FightBox", i + 1);
	}

	return fightBox;
}

FightBox** LoadEnemyFightBox_Runner()
{
	FightBox ** fightBox;
	fightBox = (FightBox**)calloc(E_Anim_NUMBER, sizeof(FightBox*));
	int nbFile = 0;

	// Walk
	nbFile = GetNbFile("Data/Enemies/Runner/Patrol/FightBox", ".svg");
	fightBox[E_Anim_Walk] = calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[E_Anim_Walk][i] = LoadFileFightBox("Data/Enemies/Runner/Patrol/FightBox", i + 1);
	}

	// Idle
	nbFile = GetNbFile("Data/Enemies/Runner/Idle/FightBox", ".svg");
	fightBox[E_Anim_Idle] = calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[E_Anim_Idle][i] = LoadFileFightBox("Data/Enemies/Runner/Idle/FightBox", i + 1);
	}

	// Attack_1
	nbFile = GetNbFile("Data/Enemies/Runner/Attack/FightBox", ".svg");
	fightBox[E_Anim_Attack_1] = calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[E_Anim_Attack_1][i] = LoadFileFightBox("Data/Enemies/Runner/Attack/FightBox", i + 1);
	}

	// Attack_2
	nbFile = GetNbFile("Data/Enemies/Runner/Attack/FightBox", ".svg");
	fightBox[E_Anim_Attack_2] = calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[E_Anim_Attack_2][i] = LoadFileFightBox("Data/Enemies/Runner/Attack/FightBox", i + 1);
	}

	// Stun
	nbFile = GetNbFile("Data/Enemies/Runner/Stun/FightBox", ".svg");
	fightBox[E_Anim_Stun] = calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[E_Anim_Stun][i] = LoadFileFightBox("Data/Enemies/Runner/Stun/FightBox", i + 1);
	}

	// Hit Something
	nbFile = GetNbFile("Data/Enemies/Runner/Stun/FightBox", ".svg");
	fightBox[E_Anim_HitSomething] = calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[E_Anim_HitSomething][i] = LoadFileFightBox("Data/Enemies/Runner/Stun/FightBox", i + 1);
	}

	// Take Damage
	nbFile = GetNbFile("Data/Enemies/Runner/Stun/FightBox", ".svg");
	fightBox[E_Anim_TakeDamage] = calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[E_Anim_TakeDamage][i] = LoadFileFightBox("Data/Enemies/Runner/Stun/FightBox", i + 1);
	}

	return fightBox;
}

FightBox** LoadEnemyFightBox_Ranger()
{
	FightBox ** fightBox;
	fightBox = (FightBox**)calloc(E_Anim_NUMBER, sizeof(FightBox*));
	int nbFile = 0;

	// Patrol
	nbFile = GetNbFile("Data/Enemies/Ranger/Patrol/FightBox", ".svg");
	fightBox[E_Anim_Walk] = (FightBox*)calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[E_Anim_Walk][i] = LoadFileFightBox("Data/Enemies/Ranger/Patrol/FightBox", i + 1);
	}

	// Idle
	nbFile = GetNbFile("Data/Enemies/Ranger/Idle/FightBox", ".svg");
	fightBox[E_Anim_Idle] = (FightBox*)calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[E_Anim_Idle][i] = LoadFileFightBox("Data/Enemies/Ranger/Idle/FightBox", i + 1);
	}

	// Attack_1 Distance
	nbFile = GetNbFile("Data/Enemies/Ranger/Attacks/Distance/FightBox", ".svg");
	fightBox[E_Anim_Attack_1] = (FightBox*)calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[E_Anim_Attack_1][i] = LoadFileFightBox("Data/Enemies/Ranger/Attacks/Distance/FightBox", i + 1);
	}

	// Attack_2 Repulsive
	nbFile = GetNbFile("Data/Enemies/Ranger/Attacks/Repulsive/FightBox", ".svg");
	fightBox[E_Anim_Attack_2] = (FightBox*)calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[E_Anim_Attack_2][i] = LoadFileFightBox("Data/Enemies/Ranger/Attacks/Repulsive/FightBox", i + 1);
	}

	// Stun
	nbFile = GetNbFile("Data/Enemies/Ranger/Stun/FightBox", ".svg");
	fightBox[E_Anim_Stun] = (FightBox*)calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[E_Anim_Stun][i] = LoadFileFightBox("Data/Enemies/Ranger/Stun/FightBox", i + 1);
	}

	// Stun
	nbFile = GetNbFile("Data/Enemies/Ranger/Stun/FightBox", ".svg");
	fightBox[E_Anim_HitSomething] = (FightBox*)calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[E_Anim_HitSomething][i] = LoadFileFightBox("Data/Enemies/Ranger/Stun/FightBox", i + 1);
	}

	// Take Damage
	nbFile = GetNbFile("Data/Enemies/Ranger/Stun/FightBox", ".svg");
	fightBox[E_Anim_TakeDamage] = (FightBox*)calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[E_Anim_TakeDamage][i] = LoadFileFightBox("Data/Enemies/Ranger/Stun/FightBox", i + 1);
	}

	return fightBox;
}

FightBox** LoadEnemyFightBox_Heavy()
{
	FightBox ** fightBox;
	fightBox = (FightBox**)calloc(E_Anim_NUMBER, sizeof(FightBox*));
	int nbFile = 0;

	// Patrol
	nbFile = GetNbFile("Data/Enemies/Cerfbourse/Patrol/FightBox", ".svg");
	fightBox[E_Anim_Walk] = (FightBox*)calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[E_Anim_Walk][i] = LoadFileFightBox("Data/Enemies/Cerfbourse/Patrol/FightBox", i + 1);
	}

	// Idle
	nbFile = GetNbFile("Data/Enemies/Cerfbourse/Idle/FightBox", ".svg");
	fightBox[E_Anim_Idle] = (FightBox*)calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[E_Anim_Idle][i] = LoadFileFightBox("Data/Enemies/Cerfbourse/Idle/FightBox", i + 1);
	}

	// Attack_1 Distance
	nbFile = GetNbFile("Data/Enemies/Cerfbourse/Attacks/Distance/FightBox", ".svg");
	fightBox[E_Anim_Attack_1] = (FightBox*)calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[E_Anim_Attack_1][i] = LoadFileFightBox("Data/Enemies/Cerfbourse/Attacks/Distance/FightBox", i + 1);
	}

	// Attack_2 Repulsive
	nbFile = GetNbFile("Data/Enemies/Cerfbourse/Attacks/Repulsive/FightBox", ".svg");
	fightBox[E_Anim_Attack_2] = (FightBox*)calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[E_Anim_Attack_2][i] = LoadFileFightBox("Data/Enemies/Cerfbourse/Attacks/Repulsive/FightBox", i + 1);
	}

	// Stun
	nbFile = GetNbFile("Data/Enemies/Cerfbourse/Stun/FightBox", ".svg");
	fightBox[E_Anim_Stun] = (FightBox*)calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[E_Anim_Stun][i] = LoadFileFightBox("Data/Enemies/Cerfbourse/Stun/FightBox", i + 1);
	}

	// Stun
	nbFile = GetNbFile("Data/Enemies/Cerfbourse/Stun/FightBox", ".svg");
	fightBox[E_Anim_HitSomething] = (FightBox*)calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[E_Anim_HitSomething][i] = LoadFileFightBox("Data/Enemies/Cerfbourse/Stun/FightBox", i + 1);
	}

	// Take Damage
	nbFile = GetNbFile("Data/Enemies/Cerfbourse/Stun/FightBox", ".svg");
	fightBox[E_Anim_TakeDamage] = (FightBox*)calloc(nbFile, sizeof(FightBox));
	for (int i = 0; i < nbFile; i++)
	{
		fightBox[E_Anim_TakeDamage][i] = LoadFileFightBox("Data/Enemies/Cerfbourse/Stun/FightBox", i + 1);
	}

	return fightBox;
}

FightBoxPack LoadFightBox()
{
	FightBoxPack fightBoxPack;

	fightBoxPack.player = LoadPlayerFightBox();
	fightBoxPack.enemies[E_Runner].fightBox = LoadEnemyFightBox_Runner();
	fightBoxPack.enemies[E_Ranger].fightBox = LoadEnemyFightBox_Ranger();
	fightBoxPack.enemies[E_Heavy].fightBox = LoadEnemyFightBox_Heavy();

	fightBoxPack.projectiles[Proj_Ranger] = LoadFileFightBox("Data/Projectiles/Enemies/HitBox", 1);
	fightBoxPack.projectiles[Proj_Player_FireBall] = LoadFileFightBox("Data/Projectiles/Enemies/HitBox", 1);

	return fightBoxPack;
}

HitBox projectRectWorld(HitBox hitBox, sfVector2f posEntity, sfVector2f OriginSprite, int dir)
{
	sfVector2f offSet;
	offSet.x = hitBox.rect.left - OriginSprite.x;
	offSet.y = hitBox.rect.top - OriginSprite.y;

	if (dir == -1)
	{
		hitBox.rect.left = posEntity.x + (offSet.x*dir) - hitBox.rect.width;
		hitBox.rect.top = posEntity.y + offSet.y;
	}
	else if (dir == 1)
	{
		hitBox.rect.left = posEntity.x + offSet.x;
		hitBox.rect.top = posEntity.y + offSet.y;
	}


	return hitBox;
}

void RenderFightBoxDebug(FightBox fightBox, sfVector2f posEntity, sfVector2f OriginSprite, int dir, sfRenderWindow* window)
{
	sfRectangleShape* rectShape;
	rectShape = sfRectangleShape_create();

	if (fightBox.bodyHitBox.type == rect)
	{

		sfVector2f rectPos;

		sfVector2f offSet;
		offSet.x = fightBox.bodyHitBox.rect.left - OriginSprite.x;
		offSet.y = fightBox.bodyHitBox.rect.top - OriginSprite.y;

		if (dir == -1)
		{
			rectPos.x = posEntity.x + (offSet.x*dir) - fightBox.bodyHitBox.rect.width;
			rectPos.y = posEntity.y + offSet.y;
		}
		else if (dir == 1)
		{
			rectPos.x = posEntity.x + offSet.x;
			rectPos.y = posEntity.y + offSet.y;
		}
		else
		{
			printf("I cant Render FightBox with wrong value\n");
			return;
		}


		//printf("pos : %f %f\n", rectPos.x, rectPos.y);

		sfRectangleShape_setSize(rectShape, (sfVector2f) { fightBox.bodyHitBox.rect.width, fightBox.bodyHitBox.rect.height });

		BlitRectShape(rectShape, rectPos, sfColor_fromRGBA(0, 255, 0, 150), window);
	}

	if (fightBox.damageHitBox.type == rect)
	{

		sfVector2f rectPos;

		sfVector2f offSet;
		offSet.x = fightBox.damageHitBox.rect.left - OriginSprite.x;
		offSet.y = fightBox.damageHitBox.rect.top - OriginSprite.y;

		if (dir == -1)
		{
			rectPos.x = posEntity.x + (offSet.x*dir) - fightBox.damageHitBox.rect.width;
			rectPos.y = posEntity.y + offSet.y;
		}
		else if (dir == 1)
		{
			rectPos.x = posEntity.x + offSet.x;
			rectPos.y = posEntity.y + offSet.y;
		}
		else
		{
			printf("I cant Render FightBox with wrong value\n");
			return;
		}

		sfRectangleShape_setSize(rectShape, (sfVector2f) { fightBox.damageHitBox.rect.width, fightBox.damageHitBox.rect.height });

		BlitRectShape(rectShape, rectPos, sfColor_fromRGBA(255, 0, 0, 150), window);
	}

	sfRectangleShape_destroy(rectShape);
}

//Test collision with (attack , body)
sfBool IsCollindingFightBox(HitBox attack, HitBox body)
{
	if (attack.type == rect && body.type == rect)
	{
		return isCollidingRectRect(attack.rect, body.rect);
	}
	return sfFalse;
}

void UpdateFightBox(FightBoxPack fightBoxPack, Player* player, Enemy** e_first, Projectile** proj_first, SpritePack spritePack)
{
	FightBox p_tmpFightBox;
	FightBox e_tmpFightBox;
	FightBox proj_tmpFightBox;

	sfVector2f OriginSprite;

	// ENEMIES - PLAYER
	for (struct Enemy* enemy = *e_first; enemy != NULL; enemy = enemy->ptrNext)
	{
		p_tmpFightBox = fightBoxPack.player[player->state][player->anim[player->state].actualImage];
		OriginSprite = sfSprite_getOrigin(spritePack.player[player->state].sprite[player->anim[player->state].actualImage]);

		HitBox p_AttackBox = projectRectWorld(p_tmpFightBox.damageHitBox, player->entity2D.body.pos, OriginSprite, player->entity2D.dir);
		HitBox p_HitBox = projectRectWorld(p_tmpFightBox.bodyHitBox, player->entity2D.body.pos, OriginSprite, player->entity2D.dir);

		e_tmpFightBox = fightBoxPack.enemies[enemy->type].fightBox[enemy->animState][enemy->animInfo[enemy->animState].actualImage];
		OriginSprite = sfSprite_getOrigin(spritePack.enemiesSprites[enemy->type][enemy->animState].sprite[enemy->animInfo[enemy->animState].actualImage]);

		HitBox e_HitBox = projectRectWorld(e_tmpFightBox.bodyHitBox, enemy->entity.body.pos, OriginSprite, enemy->entity.dir);

		// Boolean who check if the enemy can be hit or not
		sfBool checkState = enemy->currentState != E_Invincible && enemy->currentState != E_Death && enemy->currentState != E_Dead;

		// POUR PLUS TARD :  verifier si l'attack actuelle est differente de l'attaque precedente ( COMBO )
		// Utilisation de la structure PlayerHit de l'ennemi -> faire les verifications par rapport à ces données et celles du combo du player
		if (checkState && IsCollindingFightBox(p_AttackBox, e_HitBox))
		{
			enemy->currentState = E_Invincible;
			enemy->takeDamages(enemy, 100, 250.f, 0.2f);
		}

		if (e_tmpFightBox.damageHitBox.type != noBox)
		{
			int atkID = enemy->attackArray[enemy->attackActiveID].atkID; // Stock the ID of the current attack
			HitBox e_AttackBox = projectRectWorld(e_tmpFightBox.damageHitBox, enemy->entity.body.pos, OriginSprite, enemy->entity.dir);

			if (!enemy->attackArray[enemy->attackActiveID].atksDatas[atkID].hasHit && IsCollindingFightBox(e_AttackBox, p_HitBox))
			{
				enemy->attackArray[enemy->attackActiveID].atksDatas[atkID].hasHit = sfTrue;
				if (enemy->attackArray[enemy->attackActiveID].atksDatas[atkID].type == E_Atk_Charge)
				{
					enemy->entity.body.vel.x = 0.0f;
					enemy->entity.body.vel.y = 0.0f;
				}

				player->stats.hpActual -= enemy->attackArray[enemy->attackActiveID].atksDatas[atkID].damage; // Damage the player ( hp reduction ) !! NEED TO BY REPLACE BY A FUNCTION !! ( with a damage reduction according to the player armor )

				// Repulse if repulse active
				if (enemy->attackArray[enemy->attackActiveID].atksDatas[atkID].isRepulsive && player->state != pla_stunned)
				{
					StunThePlayer(player, enemy->attackArray[enemy->attackActiveID].atksDatas[atkID].velToAdd, enemy->attackArray[enemy->attackActiveID].atksDatas[atkID].stunTime);
				}
			}
		}
	}

	// PROJECTILES - PLAYER
	for (struct Projectile* proj = *proj_first; proj != NULL; proj = proj->ptrNext)
	{
		p_tmpFightBox = fightBoxPack.player[player->state][player->anim[player->state].actualImage];
		OriginSprite = sfSprite_getOrigin(spritePack.player[player->state].sprite[player->anim[player->state].actualImage]);

		HitBox p_AttackBox = projectRectWorld(p_tmpFightBox.damageHitBox, player->entity2D.body.pos, OriginSprite, player->entity2D.dir);
		HitBox p_HitBox = projectRectWorld(p_tmpFightBox.bodyHitBox, player->entity2D.body.pos, OriginSprite, player->entity2D.dir);

		proj_tmpFightBox = fightBoxPack.projectiles[proj->type];
		OriginSprite = sfSprite_getOrigin(spritePack.projectiles[proj->type]);

		HitBox proj_AttackBox = projectRectWorld(proj_tmpFightBox.damageHitBox, proj->body.pos, OriginSprite, proj->dir);
		HitBox proj_HitBox = projectRectWorld(proj_tmpFightBox.bodyHitBox, proj->body.pos, OriginSprite, proj->dir);

		if (proj->launcher == Proj_Enemy)
		{

			if (IsCollindingFightBox(p_AttackBox, proj_HitBox))
			{
				/*proj->hasHit = sfTrue;*/
				proj->launcher = Proj_Player;
				RevertProjectile(proj);
			}
			else if (IsCollindingFightBox(proj_AttackBox, p_HitBox))
			{
				proj->hasHit = sfTrue;
				player->stats.hpActual -= proj->damageDealed;
				StunThePlayer(player, (sfVector2f) { 500 * proj->dir, -200 }, 0.15);

			}
		}
		else if (proj->launcher == Proj_Player)
		{
			for (struct Enemy* enemy = *e_first; enemy != NULL; enemy = enemy->ptrNext)
			{
				e_tmpFightBox = fightBoxPack.enemies[enemy->type].fightBox[enemy->animState][enemy->animInfo[enemy->animState].actualImage];
				OriginSprite = sfSprite_getOrigin(spritePack.enemiesSprites[enemy->type][enemy->animState].sprite[enemy->animInfo[enemy->animState].actualImage]);

				HitBox e_HitBox = projectRectWorld(e_tmpFightBox.bodyHitBox, enemy->entity.body.pos, OriginSprite, enemy->entity.dir);

				sfBool checkState = enemy->currentState != E_Invincible && enemy->currentState != E_Death && enemy->currentState != E_Dead;

				if (checkState && IsCollindingFightBox(proj_AttackBox, e_HitBox))
				{
					enemy->currentState = E_Invincible;
					enemy->takeDamages(enemy, proj->damageDealed, 250.f, 0.2f);
					proj->hasHit = sfTrue;
				}
			}
		}
	}
}
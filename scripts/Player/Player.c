#include "Player.h"

#define ROLLINGTIME 0.2
#define ROLLINGCOST 50

#define GLIDINGCOST 80


// ------------- INIT PLAYER


StatPlayer InitPlayerStat()
{
	StatPlayer tmp;
	tmp.invincible = sfFalse;
	tmp.invisible = sfFalse;

	tmp.movespd = 1100.f;
	tmp.jump = 2100.f;

	tmp.jumpNbMax = 2;
	tmp.jumpNbActual = tmp.jumpNbMax;

	tmp.physicAtk = 100;
	tmp.spellPower = 2.f;
	tmp.critDmg = 1.50f;    // pourcentage
	tmp.critChance = 1.1f; // pourcentage
	tmp.cdReduct = 1.f;   // pourcentage

	tmp.hpMax = 1000;
	tmp.hpActual = tmp.hpMax;
	tmp.hpRegen = 1;
	tmp.armor = 10;

	tmp.staminaMax = 633;
	tmp.staminaActual = tmp.staminaMax;
	tmp.staminaRegen = 25;
	return tmp;
}

AnimInfo* InitPlayerAnim()
{
	AnimInfo* tmp;
	tmp = LoadAnimInfoByFile("Player/AnimPlayer");

	return tmp;
}

AnimSprite* LoadPlayerSprite(Player player)
{
	AnimSprite* tmp;
	tmp = LoadAnimSpriteByFile("Player/AnimPlayer");

	return tmp;
}

Entity2D InitPlayerEntity()
{
	Entity2D tmp;

	tmp.body.pos = (sfVector2f) { 900.f, 0 };
	tmp.dir = RIGHT;
	tmp.body = RigidbodyLoader(tmp.body.pos, 1, sfTrue);
	tmp.isOnGround = sfFalse;
	tmp.hasToCheckGround = sfTrue;
	tmp.hasHitWall = sfFalse;
	tmp.moveDirActive = sfFalse;
	tmp.blocID = -1;
	tmp.previousBlocID = -1;


	tmp.size.x = 242 / 2;
	tmp.size.y = 242;

	float offset = tmp.size.x / 2.f;


	tmp.bodyCollider.topC.center.x = tmp.body.pos.x;
	tmp.bodyCollider.topC.center.y = tmp.body.pos.y + tmp.size.y - offset;
	tmp.bodyCollider.topC.radius = offset;

	tmp.bodyCollider.rect.top = tmp.bodyCollider.topC.center.y;
	tmp.bodyCollider.rect.left = tmp.body.pos.x - (offset);
	tmp.bodyCollider.rect.height = (float)(tmp.size.y - tmp.size.x) + (tmp.size.x / 2) - 5.f;
	tmp.bodyCollider.rect.width = (offset * 2);

	int index = 0;
	for (int i = -1; i < 2; i++)
	{
		tmp.bodyCollider.bottomPoints[index].x = tmp.body.pos.x + offset*i;
		tmp.bodyCollider.bottomPoints[index].y = tmp.body.pos.y;
		index++;
	}

	tmp.moveDir = (sfVector2f) { 1, 0 };

	return tmp;
}

Player InitPlayer()
{
	Player tmp;

	tmp.stats = InitPlayerStat();
	tmp.anim = InitPlayerAnim();
	tmp.entity2D = InitPlayerEntity();
	tmp.state = pla_impulsion;

	tmp.timer.timerRolling = 0.f;
	tmp.timer.runningParticle = 0.f;
	tmp.timer.stunActual = 0.f;
	tmp.timer.stunMax = 0.f;

	tmp.spell[0] = EquipSpell(spl_fireBall);
	tmp.spell[1] = EquipSpell(spl_tripleJump);

	return tmp;
}

// --------------- Tools

float RegenStats(float actual, float max, float regen, float dt)
{
	//Regen
	if (actual < max)
	{
		actual += regen * dt;

		if (actual < 0)
		{
			actual = 0;
		}

	}
	else if (actual > max)
	{
		actual = max;
	}

	return actual;
}

void StunThePlayer(Player* player, sfVector2f vel, float duration)
{
	player->state = pla_stunned;
	player->timer.stunMax = duration;
	player->entity2D.body.vel = vel;

	player->entity2D.isOnGround = sfFalse;
	player->entity2D.body.Enablegravity = sfTrue;
}

sfBool UpdatingStun(Player* player, float dt)
{
	if (player->state == pla_stunned)
	{
		player->timer.stunActual += dt;


		if (player->timer.stunActual >= player->timer.stunMax)
		{
			player->timer.stunActual = 0.f;
			player->state = pla_idle;
		}
		return sfTrue;
	}
	return sfFalse;
}

// --------------- GAME LOOP


#pragma region ||------> State Machine

void Idle(Player* player)
{
	if (player->entity2D.isOnGround)
	{
		//pla_idle to pla_walking

		if (player->entity2D.body.vel.x <= -1.f || player->entity2D.body.vel.x >= 1.f)
		{
			player->state = pla_walking;
		}
	}
	else
	{
		// pla_idle to jumping
		if (player->entity2D.body.vel.y < -1.f)
		{
			player->state = pla_impulsion;
		}
		else if (player->entity2D.body.vel.y > 1.f)
		{
			player->state = pla_falling;
		}
	}

}

void Walking(Player* player)
{
	if (player->entity2D.isOnGround)
	{
		// pla_walking to pla_idle
		if (player->entity2D.body.vel.x == 0)
		{
			player->state = pla_idle;
		}
	}
	else
	{
		// pla_walking to jumping
		if (player->entity2D.body.vel.y < -1.f)
		{
			player->state = pla_impulsion;
		}
		// pla_walking to pla_falling
		else if (player->entity2D.body.vel.y > 1.f)
		{
			player->state = pla_falling;
		}
	}
}

void Crouch(Player* player)
{
	if (player->entity2D.isOnGround)
	{
		//idle to walking

		if (player->entity2D.body.vel.x <= -1.f || player->entity2D.body.vel.x >= 1.f)
		{
			player->state = pla_walking;
		}
	}
	else
	{
		// idle to jumping
		if (player->entity2D.body.vel.y < -1.f)
		{
			player->state = pla_impulsion;
		}
		else if (player->entity2D.body.vel.y > 1.f)
		{
			player->state = pla_falling;
		}
	}
}

void LeaningOnAWall(Player* player)
{
}

void Climbing(Player* player)
{
}

// jumping
void Impulsion(Player* player)
{
	if (!player->entity2D.isOnGround)
	{
		// jumping to pla_rising
		if (player->anim[pla_impulsion].timer == 0)
		{
			player->state = pla_rising;
		}
	}
	// jumping to pla_idle
	else if (player->entity2D.isOnGround)
	{
		player->state = pla_idle;
	}
}

void Rising(Player* player)
{
	// pla_rising to pla_extremum
	if (player->entity2D.body.vel.y > -500)
	{
		player->state = pla_extremum;
	}
}

void Extremum(Player* player)
{
	if (player->anim[pla_extremum].timer == 0)
	{
		player->state = pla_falling;
	}

}

void Falling(Player* player)
{
	if (player->entity2D.isOnGround)
	{
		player->state = pla_landing;
		player->stats.jumpNbActual = player->stats.jumpNbMax;
	}
	else if (player->entity2D.body.vel.y < -1.f)
	{
		player->state = pla_impulsion;
	}
}

void Gliding(Player* player)
{
	if (player->entity2D.isOnGround)
	{
		player->state = pla_idle;
		player->stats.jumpNbActual = player->stats.jumpNbMax;
	}

}

void Landing(Player* player)
{
	if (player->anim[pla_landing].timer == 0)
	{
		player->state = pla_idle;
	}
	else if (player->entity2D.body.vel.y < -1.f)
	{
		player->state = pla_impulsion;
	}
}


// pla_rolling

void Rolling(Player* player)
{
	if (player->timer.timerRolling >= ROLLINGTIME)
	{
		if (player->entity2D.isOnGround)
		{
			// pla_walking to pla_idle
			if (player->entity2D.body.vel.x > -1.f && player->entity2D.body.vel.x < 1.f)
			{
				player->state = pla_idle;
			}
		}
		else
		{
			// pla_walking to jumping
			if (player->entity2D.body.vel.y < -1.f)
			{
				player->state = pla_impulsion;
			}
			// pla_walking to pla_falling
			else if (player->entity2D.body.vel.y > 1.f)
			{
				player->state = pla_falling;
			}
		}
	}
}

// pla_stunned
void Stunned(Player* player)
{
	/*printf("STUNNED u noob\n");*/
}

// pla_attack

void Attack(Player* player)
{
	static sfBool endAttack = sfFalse;
	//printf("%d\n", player->anim[pla_attack].actualImage);


	if (player->anim[pla_attack].timer == 0 && endAttack)
	{
		if (player->entity2D.body.vel.x < -1.f || player->entity2D.body.vel.x > 1.f)
		{
			player->state = pla_walking;
			endAttack = sfFalse;
		}
		else
		{
			player->state = pla_idle;
			endAttack = sfFalse;
		}
	}
	if (player->anim[pla_attack].actualImage == 2)
	{
		endAttack = sfTrue;
	}
}
#pragma endregion

void MovementPlayerEvent(sfEvent event, Command command, Player *player, Sounds soundEffect)
{
	if (player->state != pla_stunned)
	{
		if (((event.key.code == sfKeySpace && event.type == sfEvtKeyPressed) || command.jumpNaction.pressed)
			&& player->state != pla_impulsion && player->state != pla_attack)
		{
			if (player->state == pla_falling  && player->state != pla_attack && player->stats.jumpNbActual == 0)
			{
				player->state = pla_gliding;
			}
			// jump
			if (player->stats.jumpNbActual > 0)
			{
				player->stats.jumpNbActual--;
				player->entity2D.body.vel.y = -player->stats.jump;
				player->entity2D.isOnGround = sfFalse;
				player->entity2D.moveDirActive = sfFalse;
			}

		}
		// ROLLING (A & E)
		if (player->state != pla_rolling && player->state != pla_attack  && player->stats.staminaActual >= ROLLINGCOST)
		{
			if ((event.key.code == sfKeyA && event.type == sfEvtKeyPressed) || command.dashLeft.pressed)
			{
				player->stats.staminaActual -= ROLLINGCOST;
				player->entity2D.dir = RIGHT;
				player->state = pla_rolling;
			}
			else if ((event.key.code == sfKeyE && event.type == sfEvtKeyPressed) || command.dashRight.pressed)
			{
				player->stats.staminaActual -= ROLLINGCOST;
				player->entity2D.dir = LEFT;
				player->state = pla_rolling;
			}

		}
		// ATTACK J 
		if (player->state == pla_idle || player->state == pla_walking)
		{
			if ((event.key.code == sfKeyJ  && event.type == sfEvtKeyPressed) || command.attack.pressed)
			{
				player->state = pla_attack;
				player->anim[pla_attack].timer = 0.000001f;
				playSound(soundEffect, 50.f);
				player->entity2D.body.vel.x = 0;
				player->entity2D.body.vel.y = 0;
			}
		}
	}
}

void MovementPlayer(Player *player, Command command)
{
	if (player->state != pla_stunned)
	{
		// SIDE
		if (sfKeyboard_isKeyPressed(sfKeyD) || command.right.hold)
		{
			if (player->state != pla_rolling && player->state != pla_attack)
			{
				player->entity2D.dir = RIGHT;

				if (player->entity2D.moveDirActive)
				{
					player->entity2D.body.vel.x = player->stats.movespd * player->entity2D.moveDir.x;
					player->entity2D.body.vel.y = player->stats.movespd * player->entity2D.moveDir.y;
				}
				else
				{
					player->entity2D.body.vel.x = player->stats.movespd;
				}
			}
		}
		else if (sfKeyboard_isKeyPressed(sfKeyQ) || command.left.hold)
		{
			if (player->state != pla_rolling && player->state != pla_attack)
			{
				player->entity2D.dir = LEFT;

				if (player->entity2D.moveDirActive)
				{
					player->entity2D.body.vel.x = -player->stats.movespd * player->entity2D.moveDir.x;
					player->entity2D.body.vel.y = -player->stats.movespd * player->entity2D.moveDir.y;
				}
				else
				{
					player->entity2D.body.vel.x = -player->stats.movespd;
				}
			}
		}
		else
		{
			if (player->entity2D.isOnGround)
			{
				player->entity2D.body.vel.x = 0;
				player->entity2D.body.vel.y = 0;
			}
			else
			{
				player->entity2D.body.vel.x = 0;
			}
		}

		// UP : Gliding

		if (sfKeyboard_isKeyPressed(sfKeySpace) || command.jumpNaction.hold)
		{
			if (player->stats.staminaActual <= 0)
			{
				if (player->state == pla_gliding)
				{
					player->state = pla_falling;
				}
			}
		}
		else if (player->state == pla_gliding)
		{
			player->state = pla_falling;
		}

		//DOWN
		if ((sfKeyboard_isKeyPressed(sfKeyS) || command.down.hold) && player->entity2D.body.vel.y == 0)
		{
			player->state = pla_crouch;
			player->entity2D.body.vel.x = 0;
		}
		else if (player->state == pla_crouch)
		{
			player->state = pla_idle;
		}
	}
}

void UpdatePlayer(Player *player, SpritePack *sprite, float dt)
{
	if (UpdatingStun(player, dt))
	{
	}
	else
	{
		// pla_rolling
		if (player->state == pla_rolling)
		{
			player->timer.timerRolling += dt;

			if (player->entity2D.dir == RIGHT)
			{
				player->entity2D.body.vel.x = -player->stats.movespd * 3;

			}
			if (player->entity2D.dir == LEFT)
			{
				player->entity2D.body.vel.x = player->stats.movespd * 3;

			}

			if (player->timer.timerRolling >= ROLLINGTIME)
			{
				player->timer.timerRolling = 0;
				player->state = pla_falling;
			}
		}

		if (player->state == pla_gliding)
		{
			player->entity2D.body.vel.y = 150;
			player->stats.staminaActual -= GLIDINGCOST*dt;
			switch (player->entity2D.dir)
			{
			case RIGHT:
				player->entity2D.body.vel.x = player->stats.movespd;
				break;
			case LEFT:
				player->entity2D.body.vel.x = -player->stats.movespd;
				break;
			}
		}
	}

	// regen
	player->stats.hpActual = RegenStats(player->stats.hpActual, player->stats.hpMax, player->stats.hpRegen, dt);
	player->stats.staminaActual = RegenStats(player->stats.staminaActual, player->stats.staminaMax, player->stats.staminaRegen, dt);

	//Anim
	UpdateAnimation(&player->anim[player->state], &sprite->player[player->state], dt);
}

void RenderPlayer(SpritePack sp, Player player, Layers *layer, sfRenderWindow* window)
{
	if (player.entity2D.isOnGround)
	{
		addToLayer(&layer[LAYER_PLAYER].premier, &sp.shadow, player.entity2D.body.pos, DEFAULT_INFOBLIT);
	}

	InfoBlitSprite infoBlitP = DEFAULT_INFOBLIT;
	infoBlitP.active = sfTrue;
	infoBlitP.scale = (sfVector2f) { (float)player.entity2D.dir, 1.f };

	addToLayer(&layer[LAYER_PLAYER].premier, &sp.player[player.state].sprite[player.anim[player.state].actualImage], player.entity2D.body.pos, infoBlitP);
}

// --- Debug


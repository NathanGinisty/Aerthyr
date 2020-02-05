#ifndef PLAYER_H
#define PLAYER_H
#include "../stdafx.h"
#include "../SpellManager/SpellManager.h"


typedef enum
{
	pla_idle,                                        // 0
	pla_walking,
	pla_crouch,
	pla_leaningOnAWall, // appuyé sur le mur
	pla_climbing, // acroché à un rebord        

	pla_impulsion, //jumping                         // 5
	pla_rising,
	pla_extremum,
	pla_falling,
	pla_gliding,
	pla_landing,                                     // 10

	pla_rolling, // roulage gauche comme droite

	pla_stunned, // perte de controle

	pla_attack,                                      // 12


	nbEnumPlayerState
} PlayerState;



typedef struct
{
	float timerRolling;
	float runningParticle;
	float stunActual;
	float stunMax;
} TimerCharacter;


typedef struct
{
	Entity2D entity2D;
	StatPlayer stats;
	PlayerState state;
	AnimInfo *anim;
	TimerCharacter timer;
	Spell spell[NBSPELL];
} Player;



Player InitPlayer();

StatPlayer InitPlayerStat();
AnimInfo* InitPlayerAnim();
Entity2D InitPlayerEntity();


AnimSprite* LoadPlayerSprite(Player player);

// ---
void Idle(Player* player);
void Walking(Player* player);
void Crouch(Player* player);
void LeaningOnAWall(Player* player);
void Climbing(Player* player);
void Impulsion(Player* player);
void Rising(Player* player);
void Extremum(Player* player);
void Falling(Player* player);
void Gliding(Player* player);
void Landing(Player* player);
void Rolling(Player* player);
void Stunned(Player* player);
void Attack(Player* player);


void MovementPlayer(Player *player, Command command);
float RegenStats(float actual, float max, float regen, float dt);
void StunThePlayer(Player* player, sfVector2f vel, float duration);
void MovementPlayerEvent(sfEvent event, Command command, Player *player, Sounds soundEffect);
void UpdatePlayer(Player *player, SpritePack *sprite, float dt);


void RenderPlayer(SpritePack sp, Player player, Layers *layer, sfRenderWindow* window);

#endif // !PLAYER_H
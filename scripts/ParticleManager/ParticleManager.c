#include "../ParticleManager/ParticleManager.h"


ParticleEffect InitParticle()
{
	ParticleEffect p;

	p.firstParticle = (Particle*)calloc(sizeof(Particle), 1);
	p.meteo = clear;
	p.timerSnow = 0;
	p.wind = 1;
	p.nbParticle = 0;
	return p;
}

ParticleSprite LoadParticleSprite()
{
	ParticleSprite tmp;
	tmp.smoke[0] = LoadSprite("Assets/Particle/test partivule poussiere2.png", sfTrue);
	tmp.star[0] = LoadSprite("Assets/Particle/particle_star.png", sfTrue);
	tmp.heal[0] = LoadSprite("Assets/Particle/particule heal croix.png", sfTrue);
	return tmp;
}

void CreateParticles(ParticleEffect* p, ParticleTypes type, int maxParticle, sfVector2f pos)
{
	for (int i = 0; i < maxParticle; i++)
	{
		Particle* tempParticle = (Particle*)calloc(sizeof(Particle), 1);
		tempParticle->type = type;
		tempParticle->alpha = 255;
		tempParticle->color = (sfColor) { 255, 255, 255, (int)tempParticle->alpha };
		tempParticle->angle = (float)(rand() % 360);
		float tempScale = (float)(rand() % 30 + 70) / 100;
		tempParticle->scale = (sfVector2f) { tempScale, tempScale };

		if (type == explosion)
		{
			tempParticle->lifetimeMax = (float)(rand() % 20 + 20) / 100;
			tempParticle->spd = (float)(rand() % 400 + 350);
			tempParticle->pos = (sfVector2f) { (float)pos.x, (float)pos.y };
			tempParticle->spriteUsed = rand() % 1;
		}
		if (type == quickplosion)
		{
			tempParticle->lifetimeMax = (float)(rand() % 5 + 10) / 100;
			tempParticle->spd = (float)(rand() % 100 + 300);
			tempParticle->pos = (sfVector2f) { (float)pos.x, (float)pos.y };
			tempParticle->spriteUsed = rand() % 1;
		}
		else if (type == exploWithAttenuation)
		{
			tempParticle->lifetimeMax = (float)(rand() % 100 + 50) / 100;
			tempParticle->spd = (float)(rand() % 400 + 350);
			tempParticle->pos = (sfVector2f) { (float)pos.x, (float)pos.y };
			tempParticle->spriteUsed = rand() % 1;
		}
		else if (type == dust)
		{
			tempParticle->lifetimeMax = (float)(rand() % 30 + 90) / 100;
			tempParticle->spd = (float)(rand() % 20 + 60)* (-1);
			tempParticle->pos = (sfVector2f) { (float)pos.x, (float)pos.y };
			tempParticle->spriteUsed = rand() % 1;
		}
		else if (type == snow)
		{
			tempParticle->lifetimeMax = (float)(rand() % 700 + 500) / 100;
			tempParticle->spd = (float)(rand() % 80 + 30);
			tempParticle->pos = (sfVector2f) { rand() % SCREENW, -20 };
			tempParticle->spriteUsed = rand() % 1;
			float tempScale = (float)(rand() % 20 + 20) / 100;
			tempParticle->scale = (sfVector2f) { tempScale, tempScale };
		}
		else
		{
			tempParticle->lifetimeMax = (float)(rand() % 100 + 50) / 100;
			tempParticle->spd = (float)(rand() % 200);
			tempParticle->pos = (sfVector2f) { (float)pos.x, (float)pos.y };
			tempParticle->spriteUsed = rand() % 1;
		}

		tempParticle->lifetimeActual = tempParticle->lifetimeMax;

		//Insertion de l'élément
		p->nbParticle++;

		if (p->nbParticle > 0)
		{
			tempParticle->next = p->firstParticle;
			p->firstParticle = tempParticle;
		}
		else
		{
			p->firstParticle = tempParticle;
		}
	}
}

void DeleteParticle(Particle* current, Particle** first)
{
	struct Particle* tempParticle = *first;

	if (tempParticle == current) //Cas particulier du premier élément
	{
		*first = current->next;
		free(current);
	}
	else //Si ce n'est pas le premier élément
	{
		while (tempParticle != NULL)
		{
			if (tempParticle->next == current)
			{
				tempParticle->next = tempParticle->next->next;
				free(current);
				return;
			}
			tempParticle = tempParticle->next;
		}
	}
}


void UpdateParticles(ParticleEffect *p, float dt)
{
	p->nbParticle = 0;
	for (Particle* tempParticle = p->firstParticle; tempParticle != NULL; tempParticle = tempParticle->next)
	{
		p->nbParticle++;
	}

	if (p->firstParticle != NULL)
	{
		Particle* tempParticle = p->firstParticle;
		Particle* savedParticle;
		while (tempParticle != NULL)
		{


			if (tempParticle->lifetimeActual > 0)
			{
				// ---- COMMON
				tempParticle->lifetimeActual -= dt;


				if (tempParticle->color.a > 0)
				{
					tempParticle->alpha = 255 * (tempParticle->lifetimeActual / tempParticle->lifetimeMax);
					tempParticle->color.a = (int)tempParticle->alpha;
				}

				// ---- EXPLOSION
				if (tempParticle->type == explosion)
				{
					tempParticle->pos.x += tempParticle->spd * cos(DegrToRad(tempParticle->angle)) * dt;
					tempParticle->pos.y += tempParticle->spd * sin(DegrToRad(tempParticle->angle)) * dt;
				}
				// ---- QUICKPLOSION
				if (tempParticle->type == quickplosion)
				{
					tempParticle->pos.x += tempParticle->spd * cos(DegrToRad(tempParticle->angle)) * dt;
					tempParticle->pos.y -= tempParticle->spd * dt;
				}

				// ---- EXPLOSION WITH ATTENUATION
				else if (tempParticle->type == exploWithAttenuation)
				{
					tempParticle->spd += -tempParticle->spd * 8 * dt;
					tempParticle->pos.x += tempParticle->spd * cos(DegrToRad(tempParticle->angle)) * dt;
					tempParticle->pos.y += tempParticle->spd * sin(DegrToRad(tempParticle->angle)) * dt;
				}

				// ---- SPIRAL
				else if (tempParticle->type == spiral)
				{
					tempParticle->angle += tempParticle->spd * dt;
					tempParticle->pos.x += tempParticle->spd * cos(DegrToRad(tempParticle->angle)) * dt;
					tempParticle->pos.y += tempParticle->spd * sin(DegrToRad(tempParticle->angle)) * dt;
				}

				// ---- DUST
				else if (tempParticle->type == dust)
				{
					tempParticle->pos.y += tempParticle->spd * dt;
				}

				// ---- SNOW
				else if (tempParticle->type == snow)
				{

					tempParticle->pos.x += tempParticle->spd / 10 * cos(DegrToRad(tempParticle->angle)) * dt;
					tempParticle->pos.y += tempParticle->spd * dt;
				}

				// DELETE

			}
			//savedParticle = tempParticle->next;
			if (tempParticle->lifetimeActual <= 0)
			{
				savedParticle = tempParticle->next;
				DeleteParticle(tempParticle, p->firstParticle);
				tempParticle = savedParticle;
			}

			if (tempParticle != NULL)
			{
				savedParticle = tempParticle;
				tempParticle = tempParticle->next;
			}
		}
	}
}

void RenderParticles(Layers* layer, ParticleEffect *p, SpritePack sprite)
{
	for (Particle* tempParticle = p->firstParticle; tempParticle != NULL; tempParticle = tempParticle->next)
	{
		if (tempParticle->lifetimeActual > 0)
		{
			InfoBlitSprite infoBlit = DEFAULT_INFOBLIT;
			infoBlit.active = sfTrue;
			infoBlit.rotation = tempParticle->angle;
			infoBlit.scale = tempParticle->scale;
			infoBlit.color = tempParticle->color;

			addToLayer(&layer[LAYER_PARTICLE].premier, &sprite.particle.smoke[tempParticle->spriteUsed], tempParticle->pos, infoBlit);
		}
	}
}
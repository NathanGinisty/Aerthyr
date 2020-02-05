#include "FadeManager.h"

Fade LoadFade()
{
	Fade tempFade = { 0 };
	tempFade.infos.timeWanted = 0.5;
	tempFade.infos.state = F_inactive;
	tempFade.infos.needChangeState = sfTrue;
	tempFade.infos.changeTo = jeu;

	sfImage* image = sfImage_create(SCREENW, SCREENH);
	sfTexture* texture = sfTexture_createFromImage(image, NULL);
	tempFade.sprite = sfSprite_create();
	sfSprite_setTexture(tempFade.sprite, texture, NULL);

	return tempFade;
}

void RenderFade(Fade fade, sfRenderWindow* window)
{
	BlitSprite(fade.sprite, VEC2F_NULL, window);
}

void UpdateFade(Fade* fade,GameState* gameState, float dt)
{
	int alpha;
	if (fade->infos.state == F_fadeIn || fade->infos.state == F_fadeOut)
	{
		fade->infos.timeActual += dt;
		if (fade->infos.timeActual > fade->infos.timeWanted)
		{
			if (fade->infos.state == F_fadeIn)
			{
				alpha = 255;
				if (fade->infos.needChangeState)
				{
					*gameState = fade->infos.changeTo;
					fade->infos.state = F_fadeOut;

				}
				else
				{
					fade->infos.state = F_active;
				}

			}
			else
			{
				alpha = 0;
				fade->infos.state = F_inactive;
			}
			fade->infos.timeActual = 0;
		}
		else
		{
			if (fade->infos.state == F_fadeIn)
			{
				alpha = (int)(fade->infos.timeActual / fade->infos.timeWanted * 255);
			}
			else
			{
				alpha = 255 - (int)(fade->infos.timeActual / fade->infos.timeWanted * 255);
			}
		}

		sfSprite_setColor(fade->sprite, sfColor_fromRGBA(0, 0, 0, alpha));
	}
}
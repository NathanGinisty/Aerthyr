#include "layerManager.h"

void addToLayer(Chaine** first, sfSprite** spriteAdress, sfVector2f pos, InfoBlitSprite infoBlitSprite)
{
	Chaine* provi = (Chaine*)calloc(sizeof(Chaine), 1);
	provi->sprite = spriteAdress;
	provi->pos = pos;
	provi->infoBlitSprite = infoBlitSprite;
	provi->suivant = *first;
	*first = provi;
}

void RenderLayers(Layers Liste[NB_LAYERS], sfRenderWindow* window)
{
	Chaine* current;

	for (int i = NB_LAYERS-1; i > -1; i--)
	{
		while (Liste[i].premier != NULL)
		{
			current = Liste[i].premier;

			if (current->infoBlitSprite.active)
			{
				sfSprite_setRotation(*(current->sprite), current->infoBlitSprite.rotation);
				sfSprite_setColor(*(current->sprite), current->infoBlitSprite.color);
				sfSprite_setScale(*(current->sprite), current->infoBlitSprite.scale);
			}

			BlitSprite(*(current->sprite), current->pos, window);
			Liste[i].premier = current->suivant;

			free(current);
			current = NULL;
		}
	}
	
}

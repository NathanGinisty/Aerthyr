#include "textManager.h"

void CreateText(TextInfo* message, char* police)
{
	message->police = sfFont_createFromFile(police);
	message->texte = sfText_create();

	sfText_setFont(message->texte, message->police);
}

void BlitText(TextInfo* message, sfVector2f pos, int taille, char* texte, sfColor couleur,sfRenderWindow* window)
{
	sfText_setColor(message->texte, couleur);
	sfText_setString(message->texte, texte);
	sfText_setCharacterSize(message->texte, taille);
	sfText_setPosition(message->texte, pos);
	sfRenderWindow_drawText(window, message->texte, NULL);
}

//renvoie 1 si la fonction doit boucler
//renvoie 0 si elle a fini
int TextBackspace(TextInfo* message, char* texte, int borne)
{
	int ret = 0;
	int index = 0;
	int lastSpace = 0;
	do
	{
		if (texte[index] == ' ')
		{
			sfVector2f test = sfText_findCharacterPos(message->texte, index);
			if (test.x > borne)
			{
				texte[lastSpace] = '\n';
			}
			else
			{
				lastSpace = index;
			}
		}
		index++;
	} while (texte[index] != '\0');

	if (sfText_findCharacterPos(message->texte, index).x > borne)
	{
		ret = 1;
	}
	return ret;
}

//affiche le texte sous forme de paragraphe
//borneD: pos a laquelle le texte fera retour
//a la ligne
void BlitTextBloc(TextInfo* message, sfVector2f pos, int taille, int borneD, char* texte, sfColor couleur,sfWindow* window)
{
	int a;
	do
	{
		sfText_setColor(message->texte, couleur);
		sfText_setString(message->texte, texte);
		sfText_setCharacterSize(message->texte, taille);
		sfText_setPosition(message->texte, pos);
		a = TextBackspace(message, texte, borneD);
		sfRenderWindow_drawText((sfRenderWindow*)window, message->texte, NULL);

	} while (a != 0);
}

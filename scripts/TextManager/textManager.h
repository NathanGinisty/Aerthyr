#ifndef TEXTMANAGER_H
#define TEXTMANAGER_H

#include "../stdafx.h"

typedef struct
{
	sfFont* police;
	sfText* texte;
	sfColor couleur;
}TextInfo;



void CreateText(TextInfo* message, char* police);
void BlitText(TextInfo* message, sfVector2f pos, int taille, char* texte, sfColor couleur, sfRenderWindow* window);

#endif // !TEXTMANAGER_H
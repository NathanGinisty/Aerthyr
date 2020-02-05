#ifndef MenuManager__H
#define MenuManager__H

#include "../stdafx.h"
#include "../FadeManager/FadeManager.h"

int TestButton(sfVector2f pos, sfVector2f size, sfVector2i mouse);

void Menu(sfVector2i mouse, Fade* fade, sfRenderWindow* window);

void Jeu(sfVector2i mouse, Fade* fade, sfRenderWindow* window);

void Options(sfVector2i mouse, Fade* fade, sfRenderWindow* window);

void Credits(sfVector2i mouse, Fade* fade, sfRenderWindow* window);


#endif 
#include "MenuManager.h"

int TestButton(sfVector2f pos, sfVector2f size, sfVector2i mouse)
{
	int test1 = mouse.x > pos.x && mouse.x < pos.x + size.x;
	int test2 = mouse.y > pos.y && mouse.y < pos.y + size.y;
	return test1 && test2;
}

void Menu(sfVector2i mouse, Fade* fade, sfRenderWindow* window)
{
	if (TestButton((sfVector2f) { 1550, 450 }, (sfVector2f) { 220, 100 }, mouse))
	{
		printf("passed hit button\n");
		fade->infos.changeTo = jeu;
		fade->infos.state = F_fadeIn;
	}
	else if (TestButton((sfVector2f) { 1550, 550 }, (sfVector2f) { 220, 100 }, mouse))
	{
		fade->infos.changeTo = options;
		fade->infos.state = F_fadeIn;
	}
	else if (TestButton((sfVector2f) { 1550, 650 }, (sfVector2f) { 220, 100 }, mouse))
	{
		fade->infos.changeTo = credits;
		fade->infos.state = F_fadeIn;
	}
	else if (TestButton((sfVector2f) { 1550, 750 }, (sfVector2f) { 220, 100 }, mouse))
	{
		exit(0);
	}
}

void Jeu(sfVector2i mouse, Fade* fade, sfRenderWindow* window)
{

}

void Options(sfVector2i mouse, Fade* fade, sfRenderWindow* window)
{

}

void Credits(sfVector2i mouse, Fade* fade, sfRenderWindow* window)
{

}
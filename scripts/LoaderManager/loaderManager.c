#include "loaderManager.h"

void LoaderDisplay(DataThread1* data, float deltaTime, sfClock* dtClock)
{
	sfSprite* logo = sfSprite_create();
	sfTexture* logoTexture = sfTexture_createFromFile("Assets/Load/Logo_Chargement.png", NULL);
	sfSprite_setTexture(logo, logoTexture, sfFalse);

	sfVector2u tmpSize = sfTexture_getSize(logoTexture);
	sfVideoMode videoMode = { tmpSize.x,tmpSize .y,32};
	sfRenderWindow* loadWindow = sfRenderWindow_create(videoMode, "", sfNone, NULL);
	MARGINS margins;
	margins.cxLeftWidth = -1;

	SetWindowLong(sfWindow_getSystemHandle((sfWindow*)loadWindow), GWL_STYLE, WS_POPUPWINDOW | WS_VISIBLE);
	DwmExtendFrameIntoClientArea(sfWindow_getSystemHandle((sfWindow*)loadWindow), &margins);

	sfEvent event;

	{
		float timer = 0.f;
		while (!*data->endThead)
		{
			while (sfWindow_pollEvent((sfWindow*)loadWindow, &event))
			{

			}

			deltaTime = sfTime_asSeconds(sfClock_restart(dtClock));
			timer += deltaTime;
			sfSprite_setColor(logo, (sfColor) { 255, 255, 255, 125 + (int)fabsf(sinf(timer) * 130) });

			sfRenderWindow_clear(loadWindow, sfTransparent);
			BlitSprite(logo, (sfVector2f) { 0, 0 }, loadWindow);
			sfRenderWindow_display(loadWindow);
		}
		sfRenderWindow_close(loadWindow);

		//Destroy logo and window to free memory
		sfWindow_destroy((sfWindow*)loadWindow);
		sfTexture_destroy(logoTexture);
		sfSprite_destroy(logo);
	}
}

void ChargementGameDataThread(DataThread1* data)
{
	sfClock* tmpClock = sfClock_create();
	srand((unsigned int)time(NULL));
	*data->gameData = LoadGameData();
	printf("Time to Load:%f\n", sfTime_asSeconds(sfClock_restart(tmpClock)));
	sfClock_destroy(tmpClock);
	

	*data->endThead = sfTrue;
}

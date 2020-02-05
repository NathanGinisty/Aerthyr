#include "soundManager.h"


Musique iniMusique(char* nom)
{
	Musique tempMusique;
	tempMusique.state = notPlaying;
	tempMusique.music = sfMusic_createFromFile(nom);
	tempMusique.duration = sfTime_asSeconds(sfMusic_getDuration(tempMusique.music));
	tempMusique.time = 0;
	return tempMusique;
}

float lerp(float ini, float end, float time, float timeNeeded)
{
	if (time < timeNeeded)
	{
		float temp = ini;
		float t = time / timeNeeded;
		temp += (end - ini) * t;
		return temp;
	}
	else
	{
		return end;
	}
}

void musicPlay(Musique* musique, float* volume, float Dt)
{
	float test = sfTime_asSeconds(sfMusic_getPlayingOffset(musique->music));

	if (musique->state == notPlaying && test < 1.f)
	{
		musique->state = playing;
		sfMusic_play(musique->music);
	}

	if (test < FADETIME && musique->state == playing)
	{
		musique->state = starting;
	}

	if (musique->state == starting)
	{
		musique->time += Dt;
		*volume = lerp(0, 100, musique->time, FADETIME);
		sfMusic_setVolume(musique->music, *volume);
		if (test > FADETIME)
		{
			musique->state = playing;
			musique->time = 0;
		}
	}

	if (musique->state == playing)
	{
		if (test + FADETIME > musique->duration)
		{
			musique->state = ending;
		}
	}

	if (musique->state == ending)
	{
		musique->time += Dt;
		*volume = lerp(100, 0, musique->time, FADETIME);
		sfMusic_setVolume(musique->music, *volume);
		if (*volume == 0)
		{
			musique->state = next;
			sfMusic_stop(musique->music);
		}
	}
}

void playlistPlay(PlayList* playlist, float Dt)
{
	musicPlay(&playlist->musiques[playlist->actual], &playlist->volume, Dt);
	if (playlist->musiques[playlist->actual].state == next)
	{
		playlist->actual++;
		if (playlist->actual == playlist->qtity)
		{
			playlist->actual = 0;
			for (int i = 0; i < playlist->qtity; i++)
			{
				playlist->musiques->state = notPlaying;
			}
		}
	}
}

void randomWithoutDouble(int quantite, int sortie[])
{
	int randMax = quantite;
	int* tab = (int*)malloc(quantite * sizeof(int));

	//init tab de random
	for (int i = 0; i < quantite; i++)
	{
		tab[i] = i;
	}

	//random entre x possibilitees, sans doublon
	int index = 0;
	for (int i = 0; i < quantite; i++)
	{
		int a = rand() % randMax;
		sortie[index] = tab[a];

		int temp = tab[randMax - 1];
		tab[randMax - 1] = tab[a];
		tab[a] = temp;
		randMax--;
		index++;
	}
}

Sounds initSoundTab(int qtity)
{
	Sounds temp;
	temp.son = (sfSound**)malloc(qtity * sizeof(sfSound*));
	temp.qtity = qtity;

	return temp;
}

sfSound* initSound(char* nom)
{
	sfSoundBuffer* buffer = sfSoundBuffer_createFromFile(nom);
	sfSound* sound = sfSound_create();
	sfSound_setBuffer(sound, buffer);

	return sound;
}

void playSound(Sounds son, float volume)
{
	int test = rand() % son.qtity;
	sfSoundStream_setVolume(son.son, volume);
	sfSound_play(son.son[test]);
}
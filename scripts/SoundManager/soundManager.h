#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include "../stdafx.h"

#define FADETIME 5

#pragma region music
typedef enum
{
	notPlaying,
	starting,
	playing,
	ending,
	next
}MusicState;

typedef struct
{
	sfMusic* music;
	MusicState state;
	float duration;
	float time;
}Musique;

typedef struct
{
	Musique* musiques;
	int qtity;
	int actual;
	float volume;
}PlayList;



Musique iniMusique(char* nom);

float lerp(float ini, float end, float time, float timeNeeded);

void musicPlay(Musique* musique, float* volume, float Dt);

void playlistPlay(PlayList* playlist, float Dt);

void randomWithoutDouble(int quantite, int sortie[]);

#pragma endregion

#pragma region sound
typedef struct
{
	sfSound** son;
	int qtity;
}Sounds;

Sounds initSoundTab(int qtity);

sfSound* initSound(char* nom);

void playSound(Sounds son, float volume);

#pragma endregion

typedef struct
{
	Musique menu;
	Musique inGame;
	Sounds soundEffect;
}GameMusics;

#endif
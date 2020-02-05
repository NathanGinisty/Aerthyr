#include "camera.h"



CameraViews LoadCamera(sfVector2f pos)
{
	CameraViews cam;
	cam.pos = pos;
	cam.scale = SCALE_CAMERA;
	cam.vel = (sfVector2f) { 0, 0 };
	cam.size.x = SCREENW * cam.scale;
	cam.size.y = SCREENH * cam.scale;
	cam.focusPlayer = sfTrue;
	cam.view = sfView_createFromRect((sfFloatRect) { 0, 0, SCREENW * cam.scale, SCREENH * cam.scale });
	sfView_setCenter(cam.view, cam.pos);
	return cam;
}

void MovePosCamera(CameraViews *camera, float moveX, float moveY)
{
	camera->pos.x += moveX;
	camera->pos.y += moveY;
	sfView_setCenter(camera->view, camera->pos);
}

sfVector2f ScreenToWorld(CameraViews camera, sfVector2f Vec2)
{
	sfVector2f tempPos;
	tempPos.x = (camera.pos.x - SCREENW / 2) + Vec2.x;
	tempPos.y = (camera.pos.y - SCREENH / 2) + Vec2.y;

	return tempPos;
}

sfVector2f GetDiffCameraPlayer(CameraViews camera, Player player)
{
	sfVector2f tmp;

	switch (player.state)
	{
	case pla_crouch:
		tmp.x = player.entity2D.body.pos.x - camera.pos.x;
		tmp.y = player.entity2D.bodyCollider.topC.center.y - camera.pos.y + (300 * camera.scale);
		break;
	case pla_gliding:
		tmp.x = player.entity2D.body.pos.x - camera.pos.x;
		tmp.y = player.entity2D.bodyCollider.topC.center.y - camera.pos.y + (50 * camera.scale);
		break;
	case pla_falling:
		tmp.x = player.entity2D.body.pos.x - camera.pos.x;
		tmp.y = player.entity2D.bodyCollider.topC.center.y - camera.pos.y + (50 * camera.scale);
		break;
	case pla_stunned:
		tmp.x = player.entity2D.body.pos.x - camera.pos.x;
		tmp.y = player.entity2D.bodyCollider.topC.center.y - camera.pos.y + (50 * camera.scale);
		break;
	default:
		tmp.x = player.entity2D.body.pos.x - camera.pos.x;
		tmp.y = player.entity2D.bodyCollider.topC.center.y - camera.pos.y - (80 * camera.scale);
		break;
	}


	return tmp;
}

void SetPosCamera(CameraViews *camera, Player player, sfVector2i mapSize, float dt)
{
	static float timer = 0;
	static int PrecedentDir = 0;
	static float decalCam = 0;

	sfVector2f diff = GetDiffCameraPlayer(*camera, player);

	// timer decalCam
	if (player.entity2D.dir == PrecedentDir && player.entity2D.body.vel.x != 0)
	{
		timer += dt;
	}
	else
	{
		timer = 0;
		decalCam = 0;
	}

	// reset decalCam
	//if (player.entity2D.dir != PrecedentDir)
	//{
	//	decalCam = 0;
	//}

	// precedentDir
	PrecedentDir = player.entity2D.dir;

	//timer
	if (timer > 0.1)
	{
		if (decalCam + camera->size.x / 2 < camera->size.x / 3 * 2 &&
			decalCam + camera->size.x / 2 > camera->size.x / 3 * 1)
		{
			decalCam += 200 * dt;
		}
	}

	diff.x += (decalCam * camera->scale * PrecedentDir);

	// t
	float t = 0.4 / (player.stats.movespd / 1000);

	//V=D/t
	float vX = diff.x / t;
	float vY = diff.y / t;

	//D = v*t
	camera->pos.x += vX * dt;
	camera->pos.y += vY * dt;

	if (camera->pos.x < 0)
	{
		camera->pos.x = 0;
	}
	if (camera->pos.x > mapSize.x * SIZE_TILE)
	{
		camera->pos.x = mapSize.x * SIZE_TILE;
	}
	if (camera->pos.y < 0)
	{
		camera->pos.y = 0;
	}
	if (camera->pos.y > mapSize.y * SIZE_TILE)
	{
		camera->pos.y = mapSize.y * SIZE_TILE;
	}
	sfVector2f tmpCamPos = (sfVector2f) { floorf(camera->pos.x), floorf(camera->pos.y)};
		
	sfView_setCenter(camera->view, tmpCamPos);
}



void SetZoomCamera(CameraViews *camera, float valueZoom, float dt)
{
	camera->scale += valueZoom * dt;

	if (camera->scale < 0.5)
	{
		camera->scale = 0.5;
	}
	if (camera->scale > 2.5)
	{
		camera->scale = 2.5;
	}

	camera->size.x = SCREENW * camera->scale;
	camera->size.y = SCREENH * camera->scale;

	sfView_setSize(camera->view, camera->size);
}

void VibrationCamera(CameraViews *camera, sfVector2f value)
{
	sfVector2f tempVec;
	sfVector2i screen;

	screen.x = (int)camera->size.x;
	screen.y = (int)camera->size.y;

	tempVec.x = camera->pos.x + (rand() % screen.x / 2 + screen.x / 2) * value.x;
	tempVec.y = camera->pos.y + (rand() % screen.y / 2 + screen.y / 2) * value.y;

	sfView_setCenter(camera->view, tempVec);
}

void ControlCamera(CameraViews *cam, float dt)
{
	if (sfKeyboard_isKeyPressed(sfKeyT))
	{
		SetZoomCamera(cam, -0.5f, dt);
	}
	else if (sfKeyboard_isKeyPressed(sfKeyG))
	{
		SetZoomCamera(cam, 0.5f, dt);
	}
	if (sfKeyboard_isKeyPressed(sfKeyY))
	{
		VibrationCamera(cam, (sfVector2f) { 0.05, 0.05 });
	}
}

void ReverseCamera(CameraViews *camera)
{
	camera->size.x = -camera->size.x;
	sfView_setSize(camera->view, camera->size);
}

// Bien pour faire un seul zoom précis, merdique le reste du temps, cela zoom sur la valeur actuel.
//void SetZoomCamera2(CameraViews *camera, float valueZoom, float dt)
//{
//	sfView_zoom(camera->view, valueZoom);
//}


//----------
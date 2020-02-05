#ifndef CAMERA_H
#define CAMERA_H
#include "../stdafx.h"
#include "../Player/Player.h"

#define SCALE_CAMERA 1.5f
#define DIFFXMIN 500


// ----- STRUCT -----//

typedef struct
{
	sfVector2f pos;
	sfVector2f size;
	sfVector2f vel;
	float scale;
	sfBool focusPlayer;
	sfView* view;
}CameraViews;

// ----- FUNCTION -----//

CameraViews LoadCamera(sfVector2f pos);

void MovePosCamera(CameraViews *camera, float moveX, float moveY);

void SetPosCamera(CameraViews *camera, Player player, sfVector2i mapSize, float dt);

sfVector2f ScreenToWorld(CameraViews camera, sfVector2f Vec2);

sfVector2f GetDiffCameraPlayer(CameraViews camera, Player player);

void SetZoomCamera(CameraViews *camera, float valueZoom, float dt);

void VibrationCamera(CameraViews *camera, sfVector2f value);

void ReverseCamera(CameraViews *camera);

void ControlCamera(CameraViews *cam, float dt);

#endif // !CAMERA_H
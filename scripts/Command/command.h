#ifndef COMMAND_H
#define COMMAND_H
#include "../stdafx.h"

#define TIME_RECOREVY 0.5f

typedef enum
{
	C_xBoxbutton,
	C_axe,
	C_switchButton,
	C_keyBoard,
}CommandType;

typedef enum
{
	C_KeyboardActive,
	C_GamepadActive
}ControllerType;

typedef struct
{
	int ActionButton[2];
	int factorValue;
	float RecoveryTime;
	CommandType type;
	sfBool pressed;
	sfBool hold;
}CommandAction;

typedef struct
{
	//PLAYER ACTION 
	CommandAction jumpNaction;
	CommandAction left;
	CommandAction right;
	CommandAction up;
	CommandAction down;
	CommandAction dashLeft;
	CommandAction dashRight;
	CommandAction attack;


	//INVENTORY
	CommandAction inventory;
	CommandAction inventoryG;
	CommandAction inventoryD;

	CommandAction comp1Nback;
	CommandAction comp2Ncatch;
	CommandAction atkNequip;

}Command;

Command InitCommand();
void UpdateCommand(Command* command, sfWindow* window, sfEvent event);
#endif // !COMMAND_H

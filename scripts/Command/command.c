#include "command.h"


CommandAction CreateCommandAction(CommandType commandType, int enumAction, int enumAction2, int factorValue)
{
	CommandAction commandAction;
	commandAction.pressed = sfFalse;
	commandAction.hold = sfFalse;
	commandAction.type = commandType;
	commandAction.ActionButton[0] = enumAction;
	commandAction.ActionButton[1] = enumAction2;
	commandAction.RecoveryTime = 0.f;
	commandAction.factorValue = factorValue;
	return commandAction;
}

Command InitCommand()
{
	Command tmpCommand;

	//PLAYER ACTION
	tmpCommand.jumpNaction = CreateCommandAction(C_xBoxbutton, 0, -1, 1);
	tmpCommand.left = CreateCommandAction(C_axe, sfJoystickX, sfJoystickPovX, -1);
	tmpCommand.right = CreateCommandAction(C_axe, sfJoystickX, sfJoystickPovX, 1);
	tmpCommand.up = CreateCommandAction(C_axe, sfJoystickY, sfJoystickPovY, -1);
	tmpCommand.down = CreateCommandAction(C_axe, sfJoystickY, sfJoystickPovY, 1);
	tmpCommand.dashLeft = CreateCommandAction(C_xBoxbutton, 4, -1, 1);
	tmpCommand.dashRight = CreateCommandAction(C_xBoxbutton, 5, -1, 1);
	tmpCommand.attack = CreateCommandAction(C_xBoxbutton, 2, -1, 1);


	//INVENTORY
	tmpCommand.inventory = CreateCommandAction(C_xBoxbutton, 7, -1, 1);
	tmpCommand.inventoryG = CreateCommandAction(C_xBoxbutton, 4, -1, 1);
	tmpCommand.inventoryD = CreateCommandAction(C_xBoxbutton, 5, -1, 1);

	tmpCommand.atkNequip = CreateCommandAction(C_xBoxbutton, 2, -1, 1);
	tmpCommand.comp1Nback = CreateCommandAction(C_xBoxbutton, 1, -1, 1);
	tmpCommand.comp2Ncatch = CreateCommandAction(C_xBoxbutton, 3, -1, 1);

	return tmpCommand;
}



void UpdateCommandAction(CommandAction* action, sfWindow* window, sfEvent event)
{
	if (action->hold)
	{
		action->pressed = sfFalse;
	}

	if (action->type == C_xBoxbutton)
	{
		action->pressed = sfFalse;

		if (event.type == sfEvtJoystickButtonPressed)
		{
			if ((action->ActionButton[0] != -1 && event.joystickButton.button == action->ActionButton[0]) || (action->ActionButton[1] != -1 && event.joystickButton.button == action->ActionButton[1]))
			{
				action->pressed = sfTrue;
				action->hold = sfTrue;
			}
		}
		else if (event.type == sfEvtJoystickButtonReleased)
		{
			if (event.joystickButton.button == action->ActionButton[0] && action->hold)
			{
				action->hold = sfFalse;
			}
		}
	}



	if (action->type == C_axe)
	{
		if (sfJoystick_getAxisPosition(0, action->ActionButton[0]) *action->factorValue > 70.f || sfJoystick_getAxisPosition(0, action->ActionButton[1]) *action->factorValue > 70.f)
		{
			if (!action->hold)
			{
				action->pressed = sfTrue;
			}
			action->hold = sfTrue;
		}
		else
		{
			action->hold = sfFalse;
		}
	}
	else if (action->type == C_switchButton)
	{

	}


}


void UpdateCommand(Command* command, sfWindow* window, sfEvent event)
{
	sfJoystick_update();

	//PLAYER ACTION
	UpdateCommandAction(&command->jumpNaction, window, event);
	UpdateCommandAction(&command->inventory, window, event);
	UpdateCommandAction(&command->inventoryD, window, event);
	UpdateCommandAction(&command->inventoryG, window, event);
	UpdateCommandAction(&command->dashLeft, window, event);
	UpdateCommandAction(&command->dashRight, window, event);
	UpdateCommandAction(&command->attack, window, event);

	//INVENTORY
	UpdateCommandAction(&command->left, window, event);
	UpdateCommandAction(&command->right, window, event);
	UpdateCommandAction(&command->up, window, event);
	UpdateCommandAction(&command->down, window, event);

	UpdateCommandAction(&command->atkNequip, window, event);
	UpdateCommandAction(&command->comp1Nback, window, event);
	UpdateCommandAction(&command->comp2Ncatch, window, event);
}
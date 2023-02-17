#include "Controller.h"

Controller* Controller::GetInstance()
{
	static Controller instance;

	return &instance;
}

bool Controller::Update()
{
	DWORD dwResult;
	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
	{
		statePre = state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		// î•ñŽæ“¾
		dwResult = XInputGetState(i, &state);
		
		if (dwResult == ERROR_SUCCESS)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}


bool Controller::GetPadState(State p_state, Type p_type)
{
	DWORD dwResult;	// ŠÖ”Œ‹‰Ê”»’è—p
	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
	{
		// î•ñŽæ“¾
		dwResult = XInputGetState(i, &state);

		if (dwResult == ERROR_SUCCESS)
		{
			CheckMode(p_state);
			switch (mode)
			{
			case BUTTON:
				if (GetButtonState(state, p_state, p_type))
				{
					return true;
				}
				break;
			case STICK:
				if (GetStickState(state, p_state))
				{
					return true;
				}
				break;
			}
		}
		else
		{
			return false;
		}
	}
}

bool Controller::GetButtonState(XINPUT_STATE state, State p_state, Type p_type)
{
	switch (p_type)
	{
	case Controller::NONE:
	case Controller::PUSH:
		if (PushButton(p_state) && state.Gamepad.wButtons & p_state)
		{
			return true;
		}
		break;
	case Controller::TRIGGER:
		if (TriggerButton(state, p_state) && state.Gamepad.wButtons & p_state)
		{
			return true;
		}
		break;
	default:
		break;
	}

	return false;
}

bool Controller::GetStickState(XINPUT_STATE state, State p_state)
{
	// ”ÍˆÍŽw’è—p
	int TRIGGER_DEADZONE = 100;
	int L_STICK_THUMB_DEAD = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
	int R_STICK_THUMB_DEAD = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
	switch (p_state)
	{
	case L_TRIGGER:
		if (statePre.Gamepad.bLeftTrigger < TRIGGER_DEADZONE && state.Gamepad.bLeftTrigger > TRIGGER_DEADZONE)
		{
			return true;
		}
		break;
	case R_TRIGGER:
		if (statePre.Gamepad.bRightTrigger < TRIGGER_DEADZONE && state.Gamepad.bRightTrigger > TRIGGER_DEADZONE)
		{
			return true;
		}
		break;
	case LEFT_U_STICK:
		if (state.Gamepad.sThumbLY > L_STICK_THUMB_DEAD)
		{
			return true;
		}
		break;
	case LEFT_D_STICK:
		if (state.Gamepad.sThumbLY < -L_STICK_THUMB_DEAD)
		{
			return true;
		}
		break;
	case LEFT_R_STICK:
		if (state.Gamepad.sThumbLX > L_STICK_THUMB_DEAD)
		{
			return true;
		}
		break;
	case LEFT_L_STICK:
		if (state.Gamepad.sThumbLX < -L_STICK_THUMB_DEAD)
		{
			return true;
		}
		break;
	case RIGHT_U_STICK:
		if (state.Gamepad.sThumbRY > R_STICK_THUMB_DEAD)
		{
			return true;
		}
		break;
	case RIGHT_D_STICK:
		if (state.Gamepad.sThumbRY < -R_STICK_THUMB_DEAD)
		{
			return true;
		}
		break;
	case RIGHT_R_STICK:
		if (state.Gamepad.sThumbRX > R_STICK_THUMB_DEAD)
		{
			return true;
		}
		break;
	case RIGHT_L_STICK:
		if (state.Gamepad.sThumbRX < -R_STICK_THUMB_DEAD)
		{
			return true;
		}
		break;
	}
	return false;
}

bool Controller::PushButton(State& p_state)
{
	switch (p_state)
	{
	case A:
		p_state = (State)XINPUT_GAMEPAD_A;
		break;
	case B:
		p_state = (State)XINPUT_GAMEPAD_B;
		break;
	case X:
		p_state = (State)XINPUT_GAMEPAD_X;
		break;
	case Y:
		p_state = (State)XINPUT_GAMEPAD_Y;
		break;
	case L_SHOULDER:
		p_state = (State)XINPUT_GAMEPAD_LEFT_SHOULDER;
		break;
	case R_SHOULDER:
		p_state = (State)XINPUT_GAMEPAD_RIGHT_SHOULDER;
		break;
	case BACK:
		p_state = (State)XINPUT_GAMEPAD_BACK;
		break;
	case START:
		p_state = (State)XINPUT_GAMEPAD_START;
		break;
	case UP:
		p_state = (State)XINPUT_GAMEPAD_DPAD_UP;
		break;
	case DOWN:
		p_state = (State)XINPUT_GAMEPAD_DPAD_DOWN;
		break;
	case LEFT:
		p_state = (State)XINPUT_GAMEPAD_DPAD_LEFT;
		break;
	case RIGHT:
		p_state = (State)XINPUT_GAMEPAD_DPAD_RIGHT;
		break;
	case LEFT_THUMB:
		p_state = (State)XINPUT_GAMEPAD_LEFT_THUMB;
		break;
	case RIGHT_THUMB:
		p_state = (State)XINPUT_GAMEPAD_RIGHT_THUMB;
		break;
	}

	return true;
}

bool Controller::TriggerButton(XINPUT_STATE state, State& p_state)
{
	switch (p_state)
	{
	case A:
		p_state = (State)XINPUT_GAMEPAD_A;
		break;
	case B:
		p_state = (State)XINPUT_GAMEPAD_B;
		break;
	case X:
		p_state = (State)XINPUT_GAMEPAD_X;
		break;
	case Y:
		p_state = (State)XINPUT_GAMEPAD_Y;
		break;
	case L_SHOULDER:
		p_state = (State)XINPUT_GAMEPAD_LEFT_SHOULDER;
		break;
	case R_SHOULDER:
		p_state = (State)XINPUT_GAMEPAD_RIGHT_SHOULDER;
		break;
	case BACK:
		p_state = (State)XINPUT_GAMEPAD_BACK;
		break;
	case START:
		p_state = (State)XINPUT_GAMEPAD_START;
		break;
	case UP:
		p_state = (State)XINPUT_GAMEPAD_DPAD_UP;
		break;
	case DOWN:
		p_state = (State)XINPUT_GAMEPAD_DPAD_DOWN;
		break;
	case LEFT:
		p_state = (State)XINPUT_GAMEPAD_DPAD_LEFT;
		break;
	case RIGHT:
		p_state = (State)XINPUT_GAMEPAD_DPAD_RIGHT;
		break;
	case LEFT_THUMB:
		p_state = (State)XINPUT_GAMEPAD_LEFT_THUMB;
		break;
	case RIGHT_THUMB:
		p_state = (State)XINPUT_GAMEPAD_RIGHT_THUMB;
		break;
	}

	if (!statePre.Gamepad.wButtons && state.Gamepad.wButtons)
	{
		return true;
	}
	return false;
}

void Controller::CheckMode(State p_state)
{
	stateNum = p_state;
	int LastDigitalNum = RIGHT_THUMB;

	if (stateNum <= LastDigitalNum)
	{
		mode = BUTTON;
	}
	else
	{
		mode = STICK;
	}
}
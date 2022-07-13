#include "Controller.h"
#include <cassert>

using namespace DirectX;

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

Controller* Controller::GetInstance()
{
	static Controller instance;

	return &instance;
}

bool Controller::Initialize(WinApp* win_app)
{
	HRESULT result = S_FALSE;

	// 初期化（一度だけ行う処理）
	result = DirectInput8Create
	(
		win_app->GetInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr
	);
	if (FAILED(result))
	{
		assert(0);
		return false;
	}

	//ゲームパッドの生成
	devController = nullptr;
	result = dinput->CreateDevice(GUID_Joystick, &devController, NULL);

	if (devController!=nullptr)
	{
		//入力データ形式のセット
		result = devController->SetDataFormat(&c_dfDIJoystick);
		if (FAILED(result))
		{
			assert(0);
		}

		// 軸モードを絶対値モードとして設定
		DIPROPDWORD diprop;
		ZeroMemory(&diprop, sizeof(diprop));
		diprop.diph.dwSize = sizeof(diprop);
		diprop.diph.dwHeaderSize = sizeof(diprop.diph);
		diprop.diph.dwHow = DIPH_DEVICE;
		diprop.diph.dwObj = 0;
		diprop.dwData = DIPROPAXISMODE_ABS;

		// 軸モードを変更
		devController->SetProperty(DIPROP_AXISMODE, &diprop.diph);

		// X軸の値の範囲設定
		DIPROPRANGE diprg;
		ZeroMemory(&diprg, sizeof(diprg));
		diprg.diph.dwSize = sizeof(diprg);
		diprg.diph.dwHeaderSize = sizeof(diprg.diph);
		diprg.diph.dwHow = DIPH_BYOFFSET;
		diprg.diph.dwObj = DIJOFS_X;
		diprg.lMin = -responsive_range;
		diprg.lMax = responsive_range;

		// X軸の値の範囲設定
		devController->SetProperty(DIPROP_RANGE, &diprg.diph);

		// Y軸の値の範囲設定
		diprg.diph.dwObj = DIJOFS_Y;
		devController->SetProperty(DIPROP_RANGE, &diprg.diph);

		//排他制御レベルセット
		result = devController->SetCooperativeLevel(win_app->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
		if (FAILED(result))
		{
			assert(0);
		}
	}
}

void Controller::Update()
{
	HRESULT result = S_FALSE;
	if (devController != nullptr)
	{
		//ゲームパッド情報の取得開始
		result = devController->Acquire();
		//前フレームの判定
		oldGamePadState = gamePadState;
		//全ボタンの情報を取得する
		result = devController->GetDeviceState(sizeof(DIJOYSTATE), &gamePadState);
		//リセット
		for (int i = 0; i < 32; i++)
		{
			is_push[i] = false;
		}
	}
}

bool Controller::TiltLeftStick(int stick)
{
	assert(0 <= stick && stick < 4);

	//左
	if (gamePadState.lX < -unresponsive_range && stick == S_Left)
	{
		return true;
	}
	//右
	else if (gamePadState.lX > unresponsive_range && stick == S_Right)
	{
		return true;
	}
	//後ろ
	if (gamePadState.lY > unresponsive_range && stick == S_Down)
	{
		return true;
	}
	//前
	else if (gamePadState.lY < -unresponsive_range && stick == S_Up)
	{
		return true;
	}

	return false;
}

bool Controller::TriggerLeftStick(int stick)
{
	assert(0 <= stick && stick < 4);

	//左
	if (gamePadState.lX < -unresponsive_range && !(oldGamePadState.lX < -unresponsive_range) && stick == S_Left)
	{
		return true;
	}
	//右
	else if (gamePadState.lX > unresponsive_range && !(oldGamePadState.lX > unresponsive_range) && stick == S_Right)
	{
		return true;
	}
	//後ろ
	if (gamePadState.lY > unresponsive_range && !(oldGamePadState.lY > unresponsive_range) && stick == S_Down)
	{
		return true;
	}
	//前
	else if (gamePadState.lY < -unresponsive_range && !(oldGamePadState.lY < -unresponsive_range) && stick == S_Up)
	{
		return true;
	}

	return false;
}

XMFLOAT2 Controller::LeftStickAngle()
{
	//スティックの方向判定
	float y_vec = static_cast<float>(-gamePadState.lY / responsive_range);
	float x_vec = static_cast<float>(gamePadState.lX / responsive_range);

	//横
	if (gamePadState.lX > -unresponsive_range && gamePadState.lX < unresponsive_range)
	{
		x_vec = 0.0f;
	}
	//縦
	if (gamePadState.lY < unresponsive_range && gamePadState.lY > -unresponsive_range)
	{
		y_vec = 0.0f;
	}

	return XMFLOAT2(x_vec, y_vec);
}

bool Controller::PushButton(int Button)
{
	assert(0 <= Button && Button < Cross_Up);

	for (int i = 0; i < 32; i++)
	{
		if (!(gamePadState.rgbButtons[i] & 0x80))
		{
			continue;
		}

		switch (i)
		{
		case 0:
			is_push[ButtonKind::Button_A] = true;
			break;
		case 1:
			is_push[ButtonKind::Button_B] = true;
			break;
		case 2:
			is_push[ButtonKind::Button_X] = true;
			break;
		case 3:
			is_push[ButtonKind::Button_Y] = true;
			break;
		case 4:
			is_push[ButtonKind::Button_LB] = true;
			break;
		case 5:
			is_push[ButtonKind::Button_RB] = true;
			break;
		case 6:
			is_push[ButtonKind::Select] = true;
			break;
		case 7:
			is_push[ButtonKind::Start] = true;
			break;
		case 8:
			is_push[ButtonKind::Button_LS] = true;
			break;
		case 9:
			is_push[ButtonKind::Button_RS] = true;
			break;
		default:
			break;
		}
	}

	for (int i = 0; i < Cross_Up; i++)
	{
		if (is_push[i] == true)
		{
			if (is_push[i] == is_push[Button])
			{
				return true;
			}
		}
	}

	return false;
}

bool Controller::TriggerButton(int Button)
{
	assert(0 <= Button && Button < Cross_Up);

	for (int i = 0; i < 32; i++)
	{
		if (!(gamePadState.rgbButtons[i] & 0x80))
		{
			continue;
		}
		if (oldGamePadState.rgbButtons[i] & 0x80)
		{
			continue;
		}

		switch (i)
		{
		case 0:
			is_push[ButtonKind::Button_A] = true;
			break;
		case 1:
			is_push[ButtonKind::Button_B] = true;
			break;
		case 2:
			is_push[ButtonKind::Button_X] = true;
			break;
		case 3:
			is_push[ButtonKind::Button_Y] = true;
			break;
			is_push[ButtonKind::Button_Y] = true;
			break;
		case 4:
			is_push[ButtonKind::Button_LB] = true;
			break;
		case 5:
			is_push[ButtonKind::Button_RB] = true;
			break;
		case 6:
			is_push[ButtonKind::Select] = true;
			break;
		case 7:
			is_push[ButtonKind::Start] = true;
			break;
		case 8:
			is_push[ButtonKind::Button_LS] = true;
			break;
		case 9:
			is_push[ButtonKind::Button_RS] = true;
			break;
		default:
			break;
		}
	}

	for (int i = 0; i < Cross_Up; i++)
	{
		if (is_push[i] == true)
		{
			if (is_push[i] == is_push[Button])
			{
				return true;
			}
		}
	}

	return false;
}

bool Controller::PushCrossKey(int CrossKey)
{
	assert(Cross_Up <= CrossKey && CrossKey < Cross_Left);

	if (gamePadState.rgdwPOV[0] != 0xFFFFFFFF)
	{
		switch (gamePadState.rgdwPOV[0])
		{
		case 0:
			is_push[ButtonKind::Cross_Up] = true;
			break;
		case 4500:
			is_push[ButtonKind::Cross_Up] = true;
			is_push[ButtonKind::Cross_Right] = true;
			break;
		case 9000:
			is_push[ButtonKind::Cross_Right] = true;
			break;
		case 13500:
			is_push[ButtonKind::Cross_Right] = true;
			is_push[ButtonKind::Cross_Down] = true;
			break;
		case 18000:
			is_push[ButtonKind::Cross_Down] = true;
			break;
		case 22500:
			is_push[ButtonKind::Cross_Down] = true;
			is_push[ButtonKind::Cross_Left] = true;
			break;
		case 27000:
			is_push[ButtonKind::Cross_Left] = true;
			break;
		case 31500:
			is_push[ButtonKind::Cross_Left] = true;
			is_push[ButtonKind::Cross_Up] = true;
			break;
		}

		for (int i = Cross_Up; i < ButtonMax; i++)
		{
			if (is_push[i] == true)
			{
				if (is_push[i] == is_push[CrossKey])
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool Controller::TriggerCrossKey(int CrossKey)
{
	assert(Cross_Up <= CrossKey && CrossKey < Cross_Left);

	if (gamePadState.rgdwPOV[0] != 0xFFFFFFFF && oldGamePadState.rgdwPOV[0] == 0xFFFFFFFF)
	{
		switch (gamePadState.rgdwPOV[0])
		{
		case 0:
			is_push[ButtonKind::Cross_Up] = true;
			break;
		case 4500:
			is_push[ButtonKind::Cross_Up] = true;
			is_push[ButtonKind::Cross_Right] = true;
			break;
		case 9000:
			is_push[ButtonKind::Cross_Right] = true;
			break;
		case 13500:
			is_push[ButtonKind::Cross_Right] = true;
			is_push[ButtonKind::Cross_Down] = true;
			break;
		case 18000:
			is_push[ButtonKind::Cross_Down] = true;
			break;
		case 22500:
			is_push[ButtonKind::Cross_Down] = true;
			is_push[ButtonKind::Cross_Left] = true;
			break;
		case 27000:
			is_push[ButtonKind::Cross_Left] = true;
			break;
		case 31500:
			is_push[ButtonKind::Cross_Left] = true;
			is_push[ButtonKind::Cross_Up] = true;
			break;
		}

		for (int i = Cross_Up; i < ButtonMax; i++)
		{
			if (is_push[i] == true)
			{
				if (is_push[i] == is_push[CrossKey])
				{
					return true;
				}
			}
		}
	}

	return false;
}


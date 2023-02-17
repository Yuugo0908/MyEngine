#pragma once
#pragma comment(lib,"XInput.lib")
#include <windows.h>
#include <Xinput.h>

class Controller
{
public:
	enum State
	{
		// デジタル
		A = 1,	//	Aボタン
		B,	//	Bボタン
		X,	//	Xボタン
		Y,	//	Yボタン
		L_SHOULDER,  //	LB
		R_SHOULDER,	//	RB
		BACK,	//	Back
		START,	//	Start
		UP,		//	十字キーの上
		DOWN,	//	十字キーの下
		LEFT,	//	十字キーの左
		RIGHT,	//	十字キーの右
		LEFT_THUMB,		//	左スティック
		RIGHT_THUMB,	//	右スティック
		// アナログ
		L_TRIGGER,	//	左トリガー
		R_TRIGGER,	//	右トリガー
		LEFT_U_STICK,	//	左スティックの上
		LEFT_D_STICK,	//	左スティックの下
		LEFT_L_STICK,	//	左スティックの左
		LEFT_R_STICK,	//	左スティックの右
		RIGHT_U_STICK,	//	右スティックの上
		RIGHT_D_STICK,	//	右スティックの下
		RIGHT_L_STICK,	//	右スティックの左
		RIGHT_R_STICK,	//	右スティックの右
	};
	enum Mode
	{
		BUTTON,
		STICK,
	};

	enum Type
	{
		NONE,
		PUSH,
		TRIGGER,
	};

public:
	static Controller* GetInstance();
	bool IsConnected();
	bool GetPadState(State p_state, Type p_type);

private:
	Mode mode;
	bool GetButtonState(XINPUT_STATE state, State p_state, Type p_type);
	bool GetStickState(XINPUT_STATE state, State p_state);
	bool PushButton(State& p_state);
	bool TriggerButton(XINPUT_STATE state, State& p_state);
	void CheckMode(State p_state);

	XINPUT_STATE state;
	XINPUT_STATE statePre;
	State stateNum;
};
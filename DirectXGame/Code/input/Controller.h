#pragma once
#include <Windows.h>
#include <wrl.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "WinApp.h"
#include "Operator.h"

class Controller
{
protected: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public: // サブクラス
	//ゲームパッド（ボタン）
	enum ButtonKind
	{
		Button_A,
		Button_B,
		Button_X,
		Button_Y,
		Button_LB,
		Button_RB,
		Select,
		Start,
		Button_LS,
		Button_RS,
		Cross_Up,
		Cross_Down,
		Cross_Right,
		Cross_Left,
		ButtonMax
	};

	//マウス
	enum MouseKind
	{
		M_Left, M_Right, M_Middle
	};

	//スティック
	enum StickKind
	{
		S_Up, S_Down, S_Right, S_Left
	};
public: //メンバ関数
	//初期化
	void Initialize(WinApp* win_app);
	//更新
	void Update();
	//ゲームパッドスティック
	bool TiltLeftStick(int stick);
	//ゲームパッドスティック（長押し不可）
	bool TriggerLeftStick(int stick);
	//ゲームパッドスティックを倒した比率
	XMFLOAT2 LeftStickAngle();
	//ゲームパッドボタン
	bool PushButton(int Button);
	//ゲームパッドボタン（長押し不可）
	bool TriggerButton(int Button);
	//ゲームパッド十字キー
	bool PushCrossKey(int CrossKey);
	//ゲームパッド十字キー（長押し不可）
	bool TriggerCrossKey(int CrossKey);
	// インスタンス
	static Controller* GetInstance();

private: //メンバ変数
	//DirectInputのインスタンス
	ComPtr<IDirectInput8> dinput;
	//ゲームコントローラー
	ComPtr<IDirectInputDevice8> devController;
	//ゲームパッドの判定
	DIJOYSTATE gamePadState = {};
	//前フレームのゲームパッドの判定
	DIJOYSTATE oldGamePadState = {};
	//ボタンデータ
	bool is_push[32] = {};
	//スティックの反応範囲
	LONG responsive_range = 100;
	//スティックの無反応範囲
	LONG unresponsive_range = 20;
};


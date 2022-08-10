#pragma once
#include <Windows.h>
#include <wrl.h>

#define DIRECTINPUT_VERSION     0x0800          // DirectInputのバージョン指定
#include <dinput.h>
class Mouse
{
public:
	// マウスの座標
	struct MouseMove {
		long MouseX;
		long MouseY;
		long MouseZ;
	};

private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public: // 静的メンバ変数
	static Mouse* GetInstance();

public: // メンバ関数

	// 初期化
	bool Initialize(HINSTANCE hInstance, HWND hwnd);

	// 更新
	void Update();

	// キーの左ボタン押下をチェック
	bool PushMouseLeft();

	// キーの中ボタン押下をチェック
	bool PushMouseMiddle();

	// キーの左ボタントリガーをチェック
	bool TriggerMouseLeft();

	// キーの中ボタントリガーをチェック
	bool TriggerMouseMiddle();

	// マウス移動量を取得
	MouseMove GetMouseMove();

private: // メンバ変数

	// デバイス関連
	ComPtr<IDirectInput8> dinput;
	ComPtr<IDirectInputDevice8> devMouse;
	DIMOUSESTATE2 mouseState = {};
	DIMOUSESTATE2 mouseStatePre = {};
};


#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Audio.h"
#include "GameScene.h"
#include "Controller.h"
#include "Light.h"
#include "Mouse.h"

// ゲーム全体
class MyGame
{
public:

	// 初期化
	void Initialize();
	// 終了
	void Finalize();
	// 毎フレーム更新
	void Update();
	// 描画
	void Draw();
	// 終了リクエスト
	bool IsEndRequest();

	//ポインタ置き場
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	Keyboard* keyboard = nullptr;
	Mouse* mouse = nullptr;
	Audio* audio = nullptr;
	GameScene* gameScene = nullptr;
	Camera* camera = nullptr;
	Image2d* image2d = nullptr;
	Controller* controller = nullptr;
	Light* light = nullptr;
};
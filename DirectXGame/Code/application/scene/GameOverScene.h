#pragma once

#include "DirectXCommon.h"
#include "Keyboard.h"
#include "Controller.h"
#include "Mouse.h"
#include "Image2d.h"
#include "DebugText.h"
#include "Object3d.h"
#include "Camera.h"
#include "Audio.h"
#include "Light.h"
#include "BaseScene.h"
#include "SafeDelete.h"
#include "FadeScene.h"

class GameOverScene : public BaseScene
{
public: // メンバ関数
	// 初期化
	void Initialize() override;
	// 終了
	void Finalize() override;
	// 毎フレーム処理
	void Update() override;
	// 描画
	void Draw() override;

private: // メンバ変数
	Keyboard* keyboard = Keyboard::GetInstance();
	Controller* controller = Controller::GetInstance();
	Mouse* mouse = Mouse::GetInstance();
	Audio* audio = Audio::GetInstance();
	Camera* camera = Camera::GetInstance();

	// 画像
	Image2d* GameOver = nullptr;
	Image2d* backGround = nullptr;

	bool retryFlag = false;
	bool titleFlag = false;
};


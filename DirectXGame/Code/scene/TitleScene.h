#pragma once

#include "DirectXCommon.h"
#include "Keyboard.h"
#include "Controller.h"
#include "Mouse.h"
#include "Image2d.h"
#include "DebugText.h"
#include "Object3d.h"
#include "Audio.h"
#include "BaseScene.h"
#include "SafeDelete.h"
#include "Easing.h"
#include "LevelLoader.h"
#include "FadeScene.h"

class TitleScene : public BaseScene
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

	// jsonオブジェクトの初期化
	void jsonObjectInit(const std::string sceneName);
	// jsonオブジェクトの更新
	void jsonObjectUpdate();

private: // メンバ変数

	Keyboard* keyboard = Keyboard::GetInstance();
	Controller* controller = Controller::GetInstance();
	Mouse* mouse = Mouse::GetInstance();
	Audio* audio = Audio::GetInstance();
	Camera* camera = Camera::GetInstance();

	Light* light = nullptr;

	enum Image2dNum
	{
		titleNum = 1, backNum
	};

	// レベルデータ
	LevelData* levelData = nullptr;
	// レベルデータで読み込んだオブジェクト
	std::vector<std::unique_ptr<Object3d>> jsonObject{};

	// 画像
	Image2d* title = nullptr;

	float alpha = 0.0f;
	bool fadeFlag = false;
};
#pragma once

#include "DirectXCommon.h"
#include "Keyboard.h"
#include "Controller.h"
#include "Mouse.h"
#include "Image2d.h"
#include "DebugText.h"
#include "Object3d.h"
#include "Model.h"
#include "Operator.h"

#include "Camera.h"
#include "Audio.h"
#include "Rope.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Item.h"
#include "Collision.h"
#include "Light.h"
#include "Easing.h"
#include "LevelLoader.h"

#include <SafeDelete.h>
#include <stdlib.h>
#include <time.h>

class TitleScene {
public: // メンバ関数

	// コンストクラタ
	TitleScene();
	// デストラクタ
	~TitleScene();
	// 初期化
	void Initialize();
	// 解放
	void Finalize();
	// 毎フレーム処理
	void Update();
	// 描画
	void Draw();

private: // メンバ変数
	DirectXCommon* dxCommon = nullptr;
	Keyboard* keyboard = Keyboard::GetInstance();
	Audio* audio = nullptr;

	// 画像
	Image2d* title = nullptr;
	Image2d* backGround = nullptr;
	float alpha = 0.0f;
	Image2d* explanation = nullptr;

	// シーン管理用
	bool fadeIn = false;
	bool fadeOut = false;
	bool expFlag = false;

	// シェイク用
	bool shakeFlag = false;

	bool attackFlag = false; // 突進開始フラグ
	float avoidTime = 0.0f;

	int check = 0;
};

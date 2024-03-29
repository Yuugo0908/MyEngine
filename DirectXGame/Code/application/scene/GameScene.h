﻿#pragma once

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
#include "Collision.h"
#include "Light.h"
#include "Easing.h"
#include "LevelLoader.h"
#include "Particle.h"
#include "BaseScene.h"
#include "SafeDelete.h"
#include "FadeScene.h"

#include <stdlib.h>
#include <time.h>
#include <list>

class GameScene : public BaseScene
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

	//pos初期化
	void Reset();
	// Imguiの設定
	void SetImgui();
	// ライトの更新
	void LightUpdate();
	// カメラの更新
	void CameraUpdate();
	// 敵の更新
	void EnemyUpdate();
	// ロープの更新
	void RopeUpdate();

	// jsonオブジェクトの初期化
	void jsonObjectInit(const std::string sceneName);
	// プレイヤーとエネミーのデータの初期化
	void playerAndEnemyInit(const std::string sceneName);
	// jsonオブジェクトの更新
	void jsonObjectUpdate();

	float GetLength(XMFLOAT3 posA, XMFLOAT3 posB)
	{
		XMFLOAT3 len = { posA.x - posB.x, posA.y - posB.y, posA.z - posB.z };
		return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
	}

private: // メンバ変数
	Keyboard* keyboard = Keyboard::GetInstance();
	Controller* controller = Controller::GetInstance();
	Mouse* mouse = Mouse::GetInstance();
	Camera* camera = Camera::GetInstance();
	Light* light = Light::GetInstance();

	Rope* rope = nullptr;
	Player* player = nullptr;
	Enemy* enemy = nullptr;
	// 敵の複数生成用リスト
	std::list<std::unique_ptr<Enemy>> enemies;

	enum Scene
	{
		title_, tutorial_, game_, clear_, failure_
	};

	// レベルデータ
	LevelData* levelData = nullptr;
	// レベルデータで読み込んだオブジェクト
	std::vector<std::unique_ptr<Object3d>> jsonObject{};
	// 全マップのデータ
	std::vector<std::vector<std::unique_ptr<Object3d>>> allMapData{};
	// ステージの数
	int stageCount = 0;
	// ステージの番号
	int stageNum = 0;

	// 画像
	Image2d* HPText = nullptr;
	Image2d* PlayerHPBar = nullptr;
	Image2d* PlayerHPGauge = nullptr;

	// 音声
	Audio* Bgm = Audio::GetInstance();

	// パーティクル
	Particle* effectKnockDown = nullptr;
	Particle* effectKnockDown2 = nullptr;
	Particle* effectTarget = nullptr;
	Particle* effectAvoid = nullptr;
	Particle* effectRush = nullptr;
	Particle* effectCollision = nullptr;
	int targetEffectCount = 0;

	// プレイヤー
	XMFLOAT3 pPos = {};//座標
	XMFLOAT3 pPosOld = {};
	XMFLOAT3 pScale = {};
	bool avoidFlag = false;//回避管理フラグ
	float playerHp = 360; 

	// エネミー
	XMFLOAT3 ePos = {};
	XMFLOAT3 ePosOld = {};
	XMFLOAT3 eScale = {};
	int enemyCount = 0;
	bool getEnemyAlive = false;

	// ロープ
	XMFLOAT3 catchPos = {};
	bool rFlag = false;
	bool rThrowFlag = false;
	bool rushFlag = false; // 突進開始フラグ
	float elapsedTime = 0.0f;

	// カメラ
	XMFLOAT3 cPos = {};
	XMFLOAT3 cTarget = {};
	const float trackLimit = -30.0f;

	// シーン管理用
	bool gameClearFlag = false;
	bool gameOverFlag = false;

	// シェイク用
	bool shakeFlag = false;

	// コントローラー
	// 振動用変数
	int vSpeedL = 0;
	int vSpeedR = 0;

	// ターゲット座標保存用
	XMFLOAT3 targetPos = {};
	// 過去にターゲットしたオブジェクトの座標
	XMFLOAT3 oldTargetPos = { FLT_MAX, FLT_MAX, FLT_MAX };
	// ターゲット距離保存用
	float targetLength = FLT_MAX;
	// ターゲット角度保存用
	float targetAngle = FLT_MAX;
	// ターゲットする距離の基準
	const float baseLength = 15.0f;
	// ターゲットする角度の基準
	const float baseAngle = 60.0f;

	// ポールの座標と距離と角度
	XMFLOAT3 targetPolePos = {};
	float targetPoleLength = 15.0f;
	float targetPoleAngle = 60.0f;

	// エネミーの座標と距離と角度
	XMFLOAT3 targetEnemyPos = {};
	float targetEnemyLength = 15.0f;
	float targetEnemyAngle = 60.0f;
};
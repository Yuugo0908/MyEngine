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
#include "Collision.h"
#include "Light.h"
#include "Easing.h"
#include "LevelLoader.h"
#include "Particle.h"
#include "BaseScene.h"
#include "SafeDelete.h"

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
	void reset();
	// Imguiの設定
	void SetImgui();
	// ライトの更新
	void LightUpdate();
	// カメラの更新
	void CameraUpdate();
	// 当たり判定
	void CollisionUpdate();
	// 遮蔽物があるかの判別
	bool RayCollision(const XMFLOAT3 startPos, const XMFLOAT3 endPos, const XMFLOAT3 boxPos, const XMFLOAT3 boxScale);

	// jsonオブジェクトの初期化
	void jsonObjectInit(const std::string sceneName);
	// jsonオブジェクトの更新
	void jsonObjectUpdate();

	void GameScene::CreateParticles(XMFLOAT3 setPos, float startScale, float endScale, XMFLOAT4 startColor, XMFLOAT4 endColor, int count);

	float GetLength(XMFLOAT3 posA, XMFLOAT3 posB)
	{
		XMFLOAT3 len = { posA.x - posB.x, posA.y - posB.y, posA.z - posB.z };
		return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
	}

private: // メンバ変数
	Keyboard* keyboard = Keyboard::GetInstance();
	Controller* controller = Controller::GetInstance();
	Mouse* mouse = Mouse::GetInstance();
	Audio* audio = Audio::GetInstance();
	Camera* camera = Camera::GetInstance();
	Light* light = nullptr;

	Rope* rope = nullptr;
	Player* player = nullptr;
	Enemy* enemy = nullptr;
	// 複数生成用リスト
	std::list<std::unique_ptr<Enemy>> enemys;
	LevelData* levelData = nullptr;

	enum Scene
	{
		title_, tutorial_, game_, clear_, failure_
	};

	enum ObjectType
	{
		sphere_, box_, stage_, wall_, pole_, skydome_,
	};

	enum Image2dNum
	{
		HPTextNum = 1, HPBarNum, HPGaugeNum, fadeNum,
	};

	// jsonオブジェクト
	std::vector<std::unique_ptr<Object3d>> jsonObject{};
	int objectType = 0;
	bool layHit = false;
	XMFLOAT3 boxPos = {};
	XMFLOAT3 boxScale = {};

	XMFLOAT3 polePos = {};
	XMFLOAT3 poleScale = {};
	float poleLength = 15.0f;

	// 画像
	Image2d* HPText = nullptr;
	Image2d* PlayerHPBar = nullptr;
	Image2d* PlayerHPGauge = nullptr;
	Image2d* fadeTex = nullptr;
	float alpha = 1.0f;

	// パーティクル
	Particle* box_effect = nullptr;

	// プレイヤー
	XMFLOAT3 pPos = {};//座標
	XMFLOAT3 pPosOld = {};
	XMFLOAT3 pScale = {};
	bool onGround = false;//自由落下のフラグ
	bool moveFlag = false;//移動管理フラグ
	bool avoidFlag = false;//回避管理フラグ
	float playerHp = 360;

	// エネミー
	int enemyCount = 0;
	float enemyHp = 360;
	bool eAlive = false;
	XMFLOAT3 ePos = {};
	XMFLOAT3 ePosOld = {};
	XMFLOAT3 eScale = {};

	// ロープ
	bool rFlag = false;
	int throwCount = 0;
	XMFLOAT3 catchPos = {};

	// カメラ
	XMFLOAT3 cPos = {};
	XMFLOAT3 cTarget = {};
	XMVECTOR cameraLength = {};
	const float trackLimit = -30.0f;

	// シーン管理用
	bool fadeFlag = false;
	bool gameClearFlag = false;
	bool gameOverFlag = false;

	// シェイク用
	bool shakeFlag = false;
	bool attackFlag = false; // 突進開始フラグ
	float avoidTime = 0.0f;

	// コントローラー
	// 振動用変数
	int vSpeedL = 0;
	int vSpeedR = 0;

	// 距離確認用
	XMFLOAT3 posSave = {};
	float minLength = 15.0f;
};
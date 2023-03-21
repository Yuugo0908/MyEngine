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
#include "Particle.h"

#include <SafeDelete.h>
#include <stdlib.h>
#include <time.h>

class GameScene {
private: // 静的メンバ変数
	static const int debugTextTexNumber = 0;

public: // メンバ関数

	// コンストクラタ
	GameScene();
	// デストラクタ
	~GameScene();
	// 初期化
	void Initialize(DirectXCommon* dxCommon, Audio* audio);
	// 毎フレーム処理
	void Update();
	//pos初期化
	void reset();
	// 描画
	void Draw();
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
	DirectXCommon* dxCommon = nullptr;
	Keyboard* keyboard = Keyboard::GetInstance();
	Controller* controller = Controller::GetInstance();
	Mouse* mouse = Mouse::GetInstance();
	Audio* audio = nullptr;
	Camera* camera = Camera::GetInstance();
	Collision* collision = nullptr;
	Light* light = nullptr;
	Easing* easing = nullptr;

	Rope* rope = nullptr;
	Player* player = nullptr;
	Enemy* enemy = nullptr;
	// 複数生成用リスト
	std::list<std::unique_ptr<Enemy>> enemys;
	Bullet* bullet = nullptr;
	Item* item = Item::GetInstance();
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
		titleNum = 1, resultNum, GameOverNum, HPTextNum, HPBarNum, HPGaugeNum, backNum, expNum, fadeNum,
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
	Image2d* title = nullptr;
	Image2d* result = nullptr;
	Image2d* GameOver = nullptr;
	Image2d* HPText = nullptr;
	Image2d* PlayerHPBar = nullptr;
	Image2d* PlayerHPGauge = nullptr;
	Image2d* backGround = nullptr;
	Image2d* fadeTex = nullptr;
	float alpha = 0.0f;
	Image2d* explanation = nullptr;

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
	int nowScene = title_;
	bool fadeIn = false;
	bool fadeOut = false;
	bool expFlag = false;

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
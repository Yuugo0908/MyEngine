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

class TutorialScene : public BaseScene
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
	// ロープの更新
	void RopeUpdate();

	// jsonオブジェクトの初期化
	void jsonObjectInit(const std::string sceneName);
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

	// 画像
	Image2d* HPText = nullptr;
	Image2d* PlayerHPBar = nullptr;
	Image2d* PlayerHPGauge = nullptr;
	Image2d* fadeTex = nullptr;
	float alpha = 1.0f;

	// 音声
	Audio* Bgm = Audio::GetInstance();

	// パーティクル
	Particle* effectBox = nullptr;
	Particle* effectCircle = nullptr;
	Particle* effectCircle2 = nullptr;
	Particle* effectTarget = nullptr;
	Particle* effectAvoid = nullptr;

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
	const float trackLimit = -30.0f;

	// シーン管理用
	bool fadeFlag = false;
	bool gameClearFlag = false;
	bool gameOverFlag = false;

	// シェイク用
	bool shakeFlag = false;
	bool rushFlag = false; // 突進開始フラグ
	float elapsedTime = 0.0f;

	// コントローラー
	// 振動用変数
	int vSpeedL = 0;
	int vSpeedR = 0;

	// 座標保存用
	XMFLOAT3 posSave = {};
	XMFLOAT3 oldPosSave = { 1000.0f, 1000.0f, 1000.0f };
	bool targetFlag = false;

	// ポールの座標と距離
	XMFLOAT3 posPoleSave = {};
	float minPoleLength = 15.0f;

	// エネミーの座標と距離
	XMFLOAT3 posEnemySave = {};
	float minEnemyLength = 15.0f;
};


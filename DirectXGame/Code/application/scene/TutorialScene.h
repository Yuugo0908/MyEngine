#pragma once

#include "DirectXCommon.h"
#include "Keyboard.h"
#include "Controller.h"
#include "Mouse.h"
#include "Image2d.h"
#include "DebugText.h"
#include "Object3d.h"
#include "PostEffect.h"
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

	enum TutorialState
	{
		None, Throw, Avoid, Attack
	};

	// レベルデータ
	LevelData* levelData = nullptr;
	// レベルデータで読み込んだオブジェクト
	std::vector<std::unique_ptr<Object3d>> jsonObject{};

	// 画像
	Image2d* HPText = nullptr;
	Image2d* PlayerHPBar = nullptr;
	Image2d* PlayerHPGauge = nullptr;
	Image2d* wasdKey = nullptr;
	Image2d* spaceKey = nullptr;
	Image2d* mouseImg = nullptr;
	Image2d* mouseLeftImg = nullptr;
	Image2d* mouseRightImg = nullptr;
	float alpha = 1.0f;

	// 音声
	Audio* Bgm = Audio::GetInstance();

	// パーティクル
	Particle* effectBox = nullptr;
	Particle* effectCircle = nullptr;
	Particle* effectCircle2 = nullptr;
	Particle* effectTarget = nullptr;
	Particle* effectAvoid = nullptr;
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
	XMFLOAT3 targetPos= {};
	// ターゲット距離保存用
	float targetLength = FLT_MAX;
	// 過去にターゲットしたオブジェクトの座標
	XMFLOAT3 oldTargetPos = { FLT_MAX, FLT_MAX, FLT_MAX };
	// ターゲットする距離の基準
	const float baseLength = 15.0f;

	// ポールの座標と距離
	XMFLOAT3 targetPolePos = {};
	float minPoleLength = 15.0f;

	// エネミーの座標と距離
	XMFLOAT3 targetEnemyPos = {};
	float minEnemyLength = 15.0f;

	// チュートリアル用
	TutorialState tutorialState = Throw; // チュートリアル
	bool tutorialThrow = false;
	bool tutorialAvoid = false;
	bool tutorialAttack = false;
	int imgShowCount = 0; // チュートリアル用画像の点滅カウント
};


#pragma once

#include "SafeDelete.h"
#include "DirectXCommon.h"
#include <DirectXMath.h>
#include "Input.h"
#include "Sprite.h"
#include "Object3d.h"
#include "DebugText.h"
#include "Audio.h"
#include "Model.h"
#include "stdlib.h"
#include "time.h"
#include "FbxLoader.h"

#define SHOT 100

using namespace DirectX;

class GameScene {
private: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;

private: // 静的メンバ変数
	static const int debugTextTexNumber = 0;

public: // メンバ関数
	// コンストクラタ
	GameScene();
	// デストラクタ
	~GameScene();
	// 初期化
	void Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio);
	// 毎フレーム処理
	void Update();
	//pos初期化
	void reset();
	// 描画
	void Draw();
	//移動
	void Move();
	//敵移動
	void enemyMove();
	//敵攻撃
	void enemyAttack();
	//当たり判定
	void collision();

private: // メンバ変数
	DirectXCommon* dxCommon = nullptr;
	Input* input = nullptr;
	Audio *playAudio = nullptr;
	Audio* shot = nullptr;
	Audio* hit = nullptr;
	DebugText debugText;

	// ゲームシーン用
	Sprite* title = nullptr;
	Sprite* HPText = nullptr;
	Sprite* PlayerHPBar = nullptr;
	Sprite* PlayerHPGauge = nullptr;
	Sprite* TimerBar = nullptr;
	Sprite* TimerGauge = nullptr;
	Sprite* GameClear = nullptr;
	Sprite* GameOver = nullptr;

	Model* playerModel = nullptr;
	Object3d* playerObj = nullptr;

	Model* enemyModel = nullptr;
	Object3d* enemyObj = nullptr;

	Model* skydomeModel = nullptr;
	Object3d* skydomeObj = nullptr;

	Model* enemyBulletModel = nullptr;
	Object3d* enemyBulletObj = nullptr;
	Object3d* enemyBulletObj2 = nullptr;
	Object3d* enemyBulletObj3 = nullptr;

	XMFLOAT3 playerPos = {};
	XMFLOAT3 enemyPos = {};
	XMFLOAT3 enemyBulletPos = {};
	XMFLOAT3 enemyBulletPos2 = {};
	XMFLOAT3 enemyBulletPos3 = {};

	int nowScene = 0;
	bool collisionFlag = false;
	bool collisionFlag2 = false;
	bool collisionFlag3 = false;
	bool enemyAttackFlag = false;
	bool BulletFlag = false;
	bool BulletFlag2 = false;
	bool BulletFlag3 = false;

	int enemyAttackCount = 0;

	const float moveAmount = 0.7f;

	float enemyMoveX = 0.5f;

	float collisionX = 0.0f;
	float collisionY = 0.0f;
	float collisionZ = 0.0f;
	float Collision = 0.0f;

	float collisionX2 = 0.0f;
	float collisionY2 = 0.0f;
	float collisionZ2 = 0.0f;
	float Collision2 = 0.0f;

	float collisionX3 = 0.0f;
	float collisionY3 = 0.0f;
	float collisionZ3 = 0.0f;
	float Collision3 = 0.0f;

	const float playerRadius = 2.0f;
	const float enemyBulletRadius = 2.0f;

	float PlayerHP = 360;

	float DodgeTimer = 0.0f;
};
#pragma once
#include "Object3d.h"
#include "Keyboard.h"
#include "Controller.h"
#include "Collision.h"
#include "Easing.h"
#include "Operator.h"
#include "Camera.h"
#include "Particle.h"
#include "DirectXCommon.h"
#include "Rope.h"

class Player
{
private:
	Player();

	~Player();

public:
	// コピーコンストラクタを無効化
	Player(const Player& obj) = delete;
	// 代入演算子を無効化
	Player& operator=(const Player& obj) = delete;
public: // メンバ関数

	static Player* GetInstance();

	bool Initialize(const XMFLOAT3 pos, const XMFLOAT3 scale);

	void Update();

	void Rush(XMFLOAT3 targetPos, bool& flag, float& avoidTime);

	void Avoid();

	void Jump();

	float Search(const XMFLOAT3& pos, const XMFLOAT3 cameraEye);

	bool Damage(const std::unique_ptr<Object3d>& object);

	void Reset();

	void ReSpawn();

	void TrackRot(const XMFLOAT3& startPos, const XMFLOAT3& endPos);

	void CameraTarget() { camera->SetTarget(pPos); }

	// マップ当たり判定
	bool MapCollide(XMFLOAT3 boxPos, XMFLOAT3 boxScale);

	bool StageCollide(XMFLOAT3 stagePos, XMFLOAT3 stageScale, bool& reverseFlag);

	bool PoleCollide(XMFLOAT3 polePos, XMFLOAT3 poleScale);

	// オブジェクト
	const std::unique_ptr<Object3d>& GetObj() { return playerObj; }
	// 回避
	const bool& GetAvoidFlag() { return avoidFlag; }
	void SetAvoidFlag(bool avoidFlag) { this->avoidFlag = avoidFlag; }
	// 着地
	const bool& GetOnGround() { return onGround; }
	void SetOnGround(bool onGround) { this->onGround = onGround; }

	// ジャンプ
	void SetJumpFlag(bool jumpFlag) { this->jumpFlag = jumpFlag; }

	float GetLength(XMFLOAT3 posA, XMFLOAT3 posB)
	{
		XMFLOAT3 len = { posA.x - posB.x, posA.y - posB.y, posA.z - posB.z };
		return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
	}

private: // メンバ変数

	Keyboard* keyboard = Keyboard::GetInstance();
	Mouse* mouse = Mouse::GetInstance();
	Controller* controller = Controller::GetInstance();
	Camera* camera = Camera::GetInstance();

	// モデル
	Model* playerModel = nullptr;
	std::unique_ptr<Object3d> playerObj = nullptr;

	// プレイヤー
	XMFLOAT3 pPos = {};//座標
	XMFLOAT3 pPosOld = {};
	XMFLOAT3 pScale = {};//大きさ
	XMFLOAT3 pRot = {};//回転
	XMFLOAT3 inertiaSaveJump = {}; // ジャンプ慣性
	XMFLOAT3 inertiaSaveMove = {};
	XMFLOAT3 reSpawnPos = {};

	float pSpeed = 0.35f;
	bool onGround = false; // 自由落下のフラグ
	bool jumpFlag = false; // ジャンプフラグ
	bool moveFlag = false; // 移動フラグ
	bool rushFlag = false; // 突進フラグ
	float pMove = 0.0f; // 移動量
	float pAcc = 0.2f; // 加速
	float pVel = 0.2f; // 速度
	float pDown = 0.0f; // 減速
	float pGra = 0.08f;//重力
	float rate = 1.0f; // 斜め移動時の制限
	int damageInterval = 0; // 攻撃を受けた際のインターバル

	// 突進用
	bool avoidFlag = false; // 回避開始フラグ

	// カメラ距離取得用
	XMFLOAT3 cameraTrack = {};
};


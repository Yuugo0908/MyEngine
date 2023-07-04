#pragma once
#include "Object3d.h"
#include "Collision.h"
#include "Operator.h"
#include "Player.h"
#include "Bullet.h"
#include "SafeDelete.h"

#include <stdlib.h>
#include <time.h>

class Enemy
{
public: // メンバ関数
	// 行動フェーズ
	enum class Phase
	{
		attack,
		move,
		stay,
	};

	bool ModelInit();

	bool Initialize(Player* player);

	void Update();

	void Draw();

	void Reset();

	bool BulletCreate();

	void ReactionDraw();

	void Attack();

	void Move();

	void Stay();

	void Jump();

	void TrackRot(const XMFLOAT3& startPos, const XMFLOAT3& endPos);

	void ReSpawn();

	void Search();

	bool ObstacleDetection(XMFLOAT3 boxPos, XMFLOAT3 boxScale);

	bool Damage(const std::unique_ptr<Object3d>& object);

	bool BulletCollision();

	bool Danger();

	// マップ当たり判定
	bool MapCollide(const XMFLOAT3 boxPos, const XMFLOAT3 boxScale);
	bool StageCollide(const XMFLOAT3 stagePos, const XMFLOAT3 stageScale);

	const std::unique_ptr<Object3d>& GetObj() { return enemyObj; }

	const bool& GetAlive() { return eAlive; }
	void SetAlive(bool eAlive) { this->eAlive = eAlive; }

	void SetAttackFlag(bool attackFlag) { this->attackFlag = attackFlag; }
	const bool& GetAttackFlag() { return attackFlag; }
	// 着地
	void SetOnGround(bool onGround) { this->onGround = onGround; }
	// 出現座標の設定
	void SetSpawnPos(XMFLOAT3 spawnPos) { this->spawnPos = spawnPos; }
	// 出現回転の設定
	void SetSpawnRot(XMFLOAT3 spawnRot) { this->spawnRot = spawnRot; }

	float GetLength(XMFLOAT3 posA, XMFLOAT3 posB)
	{
		XMFLOAT3 len = { posA.x - posB.x, posA.y - posB.y, posA.z - posB.z };
		return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
	}

private:
	Player* player = nullptr;

	static Model* enemyModel;
	std::unique_ptr<Object3d> enemyObj = nullptr;

	// パーティクル
	Particle* exclamation_mark = nullptr;
	Particle* question_mark = nullptr;

	// 弾複数生成用
	static Model* bulletModel;
	std::list<std::unique_ptr<Bullet>> bullets;

	// エネミー
	XMFLOAT3 ePos = {};
	XMFLOAT3 eOldPos = {};
	XMFLOAT3 eScale = {};
	XMFLOAT3 eRot = {};
	XMFLOAT3 spawnPos = {};
	XMFLOAT3 spawnRot = {};
	XMFLOAT3 preMovePos = {};

	// リスポーンするy座標(高さ)
	const float limitPos = -30.0f;

	// 敵の行動パターン
	Phase phase = Enemy::Phase::move;

	float eUp = 0.0f; // 上昇
	float eDown = 0.0f;// 下降
	float eGra = 0.1f; // 重力
	float PElength = 0.0f;
	float lengthOld = 0.0f;

	int attackCount = 10;

	bool eAlive = false;// 生きているかのフラグ
	bool attackFlag = false;// 攻撃がプレイヤーに当たるかのフラグ
	bool jumpFlag = false;
	bool onGround = false;
	bool hitFlag = false;

	bool visibleFlag = false;
	bool invisibleFlag = true;

	// プレイヤー
	XMFLOAT3 pPos = {};
};


#pragma once
#include "Object3d.h"
#include "Collision.h"
#include "Operator.h"
#include "Player.h"
#include "Bullet.h"

#include <stdlib.h>
#include <time.h>

class Enemy
{
public:
	// 行動フェーズ
	enum class Phase
	{
		attack,
		move,
		stay,
	};

	static bool StaticInit();

	bool Initialize(Player* player);

	bool BulletCreate();

	void Update();

	void Finalize();

	void Draw();

	void Attack();

	void Move();

	void Stay();

	void Spawn();

	void Reset();

	bool EnemyCollision();

	bool BulletCollision();

	void BulletRemove();

	// マップチップ当たり判定
	bool MapCollide(XMFLOAT3 boxPos, XMFLOAT3 boxScale);
	bool StageCollide(XMFLOAT3 stagePos, XMFLOAT3 stageScale);

	const std::unique_ptr<Object3d>& GetObj() { return enemyObj; }

	const bool& GetAlive() { return eAlive; }
	void SetAlive(bool eAlive) { this->eAlive = eAlive; }

	void SetAttackFlag(bool attackFlag) { this->attackFlag = attackFlag; }
	void SetCatchFlag(bool catchFlag) { this->catchFlag = catchFlag; }

	// 着地
	const bool& GetOnGround() { return onGround; }
	void SetOnGround(bool onGround) { this->onGround = onGround; }

	float GetLength(XMFLOAT3 posA, XMFLOAT3 posB)
	{
		XMFLOAT3 len = { posA.x - posB.x, posA.y - posB.y, posA.z - posB.z };
		return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
	}

private:
	Player* player = nullptr;

	static Model* enemyModel;
	std::unique_ptr<Object3d> enemyObj = nullptr;

	// 弾複数生成用
	static Model* bulletModel;
	std::list<std::unique_ptr<Bullet>> bullets;

	// エネミー
	XMFLOAT3 ePos = {};
	XMFLOAT3 eOldPos = {};
	XMFLOAT3 eScale = {};
	XMFLOAT3 spawnPos = {};
	XMFLOAT3 randPos = {}; // ランダムなスポーン位置

	Phase phase = Enemy::Phase::move;

	float eUp = 0.0f; // 上昇
	float eDown = 0.0f;// 下降
	float eGra = 0.1f; // 重力
	float PElength = 0.0f;
	float lengthOld = 0.0f;

	int attackCount = 0;

	bool eFlag = false; // 自由落下のフラグ
	bool eAlive = false;// 生きているかのフラグ
	bool attackFlag = false;// 攻撃がプレイヤーに当たるかのフラグ
	bool catchFlag = false;// ロープで捕まっているかの判定
	bool jumpFlag = false;
	bool onGround = false;
	bool hitFlag = false;

	// プレイヤー
	XMFLOAT3 pPos = {};
};


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
	typedef enum
	{
		attack,
		move,
		stay,
	} Phase;

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

	bool EnemyCollision();

	bool BulletCollision();

	// マップチップ当たり判定
	bool MapCollide(XMFLOAT3 boxPos, XMFLOAT3 boxRadius);
	bool StageCollide(XMFLOAT3 stagePos, XMFLOAT3 stageScale);
	bool RopeCollide(XMFLOAT3 rPos, XMFLOAT3 rRadius);

	const std::unique_ptr<Object3d>& GetObj() { return enemyObj; }

	const XMFLOAT3& GetPos() { return ePos; }
	void SetPos(XMFLOAT3 ePos) { this->ePos = ePos; }

	const bool& GetAlive() { return eAlive; }
	void SetAlive(bool eAlive) { this->eAlive = eAlive; }

	void SetAttackFlag(bool attackFlag) { this->attackFlag = attackFlag; }

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
	XMFLOAT3 eRadius = {};
	XMFLOAT3 spawnPos = {};
	XMFLOAT3 randPos = {}; // ランダムなスポーン位置

	Phase phase = move;

	float eUp = 0.0f; // 上昇
	float eDown = 0.0f;// 下降
	float eGra = 0.1f; // 重力
	float PElength = 0.0f;
	float lengthOld = 0.0f;

	int eAliveCount = 0;
	int enemyCount = 0; // 倒した数
	int attackCount = 0;

	bool eFlag = false; // 自由落下のフラグ
	bool eAlive = false;// 生きているかのフラグ
	bool attackFlag = false;
	bool jumpFlag = false;
	bool onGround = false;

	// バレット
	XMFLOAT3 bPos = {};

	// プレイヤー
	XMFLOAT3 pPos = {};
};


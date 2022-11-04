#pragma once
#include "Object3d.h"
#include "Collision.h"
#include "Player.h"
#include "Bullet.h"

#include <DirectXMath.h>
#include <stdlib.h>
#include <time.h>

class Enemy
{
private: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;
public:

	// 行動フェーズ
	enum class Phase
	{
		attack,
		move,
	};

	bool Initialize(Player* player, Bullet* bullet);

	void Update();

	void Finalize();

	void Draw();

	void Move();

	void Attack();

	XMFLOAT3 spawnRandom(XMFLOAT3& rnadSpawn);

	const std::unique_ptr<Object3d>& GetObj() { return enemy; }
	//const std::unique_ptr<Object3d>& GetBullet() { return bullet; }

	const XMFLOAT3& GetPos() { return ePos; }
	void SetPos(XMFLOAT3 ePos) { this->ePos = ePos; }

	const bool& GetAlive() { return eAlive; }
	void SetAlive(bool eAlive) { this->eAlive = eAlive; }

	//const bool& GetAttackFlag() { return attackFlag; }
	//void SetAttackFlag(bool attackFlag) { this->attackFlag = attackFlag; }

	float GetLength(XMFLOAT3 posA, XMFLOAT3 posB)
	{
		XMFLOAT3 len = { posA.x - posB.x, posA.y - posB.y, posA.z - posB.z };
		return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
	}

private:
	Player* player = nullptr;
	Bullet* bullet = nullptr;

	Model* enemyModel = nullptr;
	std::unique_ptr<Object3d> enemy = nullptr;
	/*Model* bulletModel = nullptr;
	std::unique_ptr<Object3d> bullet = nullptr;*/

	// エネミー
	XMFLOAT3 ePos = {};
	float PElength = {};
	bool eFlag = false; // 自由落下のフラグ
	float eVal = 0.2f; // 速度
	float eGra = 0.1f; // 重力
	bool eAlive = false;// 生きているかのフラグ
	int eAliveCount = 0;
	int enemyCount = 0; // 倒した数
	bool attackFlag = false;
	Phase phase = Enemy::Phase::move;
	XMFLOAT3 spawnPos = {}; // ランダムなスポーン位置

	// バレット
	XMFLOAT3 bPos = {};

	// プレイヤー
	XMFLOAT3 pPos = {};
};


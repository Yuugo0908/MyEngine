#pragma once
#include "Object3d.h"
#include "Collision.h"
#include "Operator.h"
#include "Camera.h"

class Bullet
{
public: // メンバ関数

	bool Initialize();

	void Update(const XMFLOAT3& pPos, const XMFLOAT3& ePos);

	void Attack();

	void Finalize();

	void Draw();

	void Collision();

	const std::unique_ptr<Object3d>& GetObj() { return bulletObj; }

	const XMFLOAT3& GetPos() { return bPos; }
	void SetPos(XMFLOAT3 bPos) { this->bPos = bPos; }

	const bool& GetAttackFlag() { return attackFlag; }
	void SetAttackFlag(bool attackFlag) { this->attackFlag = attackFlag; }

	float GetLength(XMFLOAT3 posA, XMFLOAT3 posB)
	{
		XMFLOAT3 len = { posA.x - posB.x, posA.y - posB.y, posA.z - posB.z };
		return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
	}

private: // メンバ変数
	Model* bulletModel = nullptr;
	std::unique_ptr<Object3d> bulletObj = nullptr;

	// バレット
	XMFLOAT3 bPos = {};
	XMFLOAT3 bSpeed = {};
	bool attackFlag = false;
	bool shakeFlag = false; // カメラのシェイク
	int attackCount = 0;

	// エネミー
	XMFLOAT3 oldePos = {};
};


#pragma once
#include "Object3d.h"
#include "Collision.h"
#include <DirectXMath.h>

class Bullet
{
private: // エイリアス
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;

public:

	bool Initialize();

	void Update(const XMFLOAT3& pPos, const XMFLOAT3& ePos);

	void Attack(const std::unique_ptr<Object3d>& object, const XMFLOAT3& ePos);

	void Finalize();

	void Draw();

	const std::unique_ptr<Object3d>& GetObj() { return bullet; }

	const XMFLOAT3& GetPos() { return bPos; }
	void SetPos(XMFLOAT3 bPos) { this->bPos = bPos; }

	const bool& GetAttackFlag() { return attackFlag; }
	void SetAttackFlag(bool attackFlag) { this->attackFlag = attackFlag; }

	float GetLength(XMFLOAT3 posA, XMFLOAT3 posB)
	{
		XMFLOAT3 len = { posA.x - posB.x, posA.y - posB.y, posA.z - posB.z };
		return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
	}

private:
	Model* bulletModel = nullptr;
	std::unique_ptr<Object3d> bullet = nullptr;

	// バレット
	XMFLOAT3 bPos = {};
	XMFLOAT3 bSpeed = {};
	bool attackFlag = false;
};


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
	// �s���t�F�[�Y
	enum class Phase
	{
		attack,
		move,
	};

	bool Initialize(Bullet* bullet, Player* player);

	void Update(const XMFLOAT3 pPos, const XMFLOAT3 bPos);

	void Finalize();

	void Draw();

	void Move();

	void Spawn(float oldrandPos);

	void Collision();

	const std::unique_ptr<Object3d>& GetObj() { return enemyObj; }

	const XMFLOAT3& GetPos() { return ePos; }
	void SetPos(XMFLOAT3 ePos) { this->ePos = ePos; }

	const bool& GetAlive() { return eAlive; }
	void SetAlive(bool eAlive) { this->eAlive = eAlive; }

	const float& GetrandPos() { return randPos.y; }

	float GetLength(XMFLOAT3 posA, XMFLOAT3 posB)
	{
		XMFLOAT3 len = { posA.x - posB.x, posA.y - posB.y, posA.z - posB.z };
		return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
	}

private:

	Bullet* bullet = nullptr;
	Player* player = nullptr;

	Model* enemyModel = nullptr;
	std::unique_ptr<Object3d> enemyObj = nullptr;

	// �G�l�~�[
	XMFLOAT3 ePos = {};
	float PElength = {};
	bool eFlag = false; // ���R�����̃t���O
	float eVal = 0.2f; // ���x
	float eGra = 0.1f; // �d��
	bool eAlive = false;// �����Ă��邩�̃t���O
	int eAliveCount = 0;
	int enemyCount = 0; // �|������
	bool attackFlag = false;
	Phase phase = Enemy::Phase::move;
	XMFLOAT3 randPos = {}; // �����_���ȃX�|�[���ʒu
	float oldrandPos = 0.0f;
	bool shakeFlag = false;
	bool randFlag = false;

	// �o���b�g
	XMFLOAT3 bPos = {};

	// �v���C���[
	XMFLOAT3 pPos = {};
};


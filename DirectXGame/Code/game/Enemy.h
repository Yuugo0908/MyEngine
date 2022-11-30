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
		stay,
	};

	bool Initialize(Player* player, Bullet* bullet);

	bool Create();

	void Update();

	void Finalize();

	void Draw();

	void Move();

	void Stay();

	void Spawn();

	const std::unique_ptr<Object3d>& GetObj() { return enemyObj; }

	const XMFLOAT3& GetPos() { return ePos; }
	void SetPos(XMFLOAT3 ePos) { this->ePos = ePos; }

	const bool& GetAlive() { return eAlive; }
	void SetAlive(bool eAlive) { this->eAlive = eAlive; }

	float GetLength(XMFLOAT3 posA, XMFLOAT3 posB)
	{
		XMFLOAT3 len = { posA.x - posB.x, posA.y - posB.y, posA.z - posB.z };
		return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
	}

private:
	Player* player = nullptr;
	Bullet* bullet = nullptr;

	Model* enemyModel = nullptr;
	std::unique_ptr<Object3d> enemyObj = nullptr;

	// �e���������p
	std::list<std::unique_ptr<Bullet>> bullets;

	// �G�l�~�[
	XMFLOAT3 ePos = {};
	XMFLOAT3 oldePos = {};
	float PElength = 0.0f;
	float lengthOld = 0.0f;
	bool eFlag = false; // ���R�����̃t���O
	float eVal = 0.2f; // ���x
	float eGra = 0.1f; // �d��
	bool eAlive = false;// �����Ă��邩�̃t���O
	int eAliveCount = 0;
	int enemyCount = 0; // �|������
	bool attackFlag = false;
	Phase phase = Enemy::Phase::move;
	XMFLOAT3 randPos = {}; // �����_���ȃX�|�[���ʒu
	int attackCount = 0;

	// �o���b�g
	XMFLOAT3 bPos = {};

	// �v���C���[
	XMFLOAT3 pPos = {};
};


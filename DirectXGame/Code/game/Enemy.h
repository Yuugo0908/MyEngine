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

	// �}�b�v�`�b�v�����蔻��
	bool MapCollide(XMFLOAT3 boxPos, XMFLOAT3 boxRadius);
	bool StageCollide(XMFLOAT3 stagePos, XMFLOAT3 stageScale);
	bool RopeCollide(XMFLOAT3 rPos, XMFLOAT3 rRadius);

	const std::unique_ptr<Object3d>& GetObj() { return enemyObj; }

	const XMFLOAT3& GetPos() { return ePos; }
	void SetPos(XMFLOAT3 ePos) { this->ePos = ePos; }

	const bool& GetAlive() { return eAlive; }
	void SetAlive(bool eAlive) { this->eAlive = eAlive; }

	void SetAttackFlag(bool attackFlag) { this->attackFlag = attackFlag; }

	// ���n
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

	// �e���������p
	static Model* bulletModel;
	std::list<std::unique_ptr<Bullet>> bullets;

	// �G�l�~�[
	XMFLOAT3 ePos = {};
	XMFLOAT3 eOldPos = {};
	XMFLOAT3 eRadius = {};
	XMFLOAT3 spawnPos = {};
	XMFLOAT3 randPos = {}; // �����_���ȃX�|�[���ʒu

	Phase phase = move;

	float eUp = 0.0f; // �㏸
	float eDown = 0.0f;// ���~
	float eGra = 0.1f; // �d��
	float PElength = 0.0f;
	float lengthOld = 0.0f;

	int eAliveCount = 0;
	int enemyCount = 0; // �|������
	int attackCount = 0;

	bool eFlag = false; // ���R�����̃t���O
	bool eAlive = false;// �����Ă��邩�̃t���O
	bool attackFlag = false;
	bool jumpFlag = false;
	bool onGround = false;

	// �o���b�g
	XMFLOAT3 bPos = {};

	// �v���C���[
	XMFLOAT3 pPos = {};
};


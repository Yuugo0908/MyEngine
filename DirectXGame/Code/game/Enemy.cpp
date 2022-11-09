#include "Enemy.h"

bool Enemy::Initialize(Player* player, Bullet* bullet)
{
	assert(player);
	assert(bullet);
	this->player = player;
	this->bullet = bullet;

	enemyModel = enemyModel->CreateFromObject("sphere");
	enemyObj = Object3d::Create();
	enemyObj->SetModel(enemyModel);

	enemyObj->SetPosition({ 0.0f, 100.0f, 0.0f });
	enemyObj->SetScale({ 1.0f, 1.0f, 1.0f });
	ePos = enemyObj->GetPosition();

	return true;
}

bool Enemy::Create()
{
	return false;
}

void Enemy::Update()
{
	bPos = bullet->GetPos();
	attackFlag = bullet->GetAttackFlag();

	if (!eAlive)
	{
		eAliveCount++;
		enemyObj->SetPosition({ 0.0f, 100.0f, 0.0f });

		if (eAliveCount == 60)
		{
			phase = Enemy::Phase::move;
			pPos = player->GetPos();
			spawnRandom(spawnPos);
			enemyObj->SetPosition({ spawnPos.x, 5.0f, spawnPos.z });
			eAlive = true;
			eAliveCount = 0;
		}

		attackFlag = false;
		bullet->SetAttackFlag(attackFlag);
		ePos = enemyObj->GetPosition();
		enemyObj->Update();
		bullet->SetPos(ePos);
		return;
	}

	pPos = player->GetPos();
	PElength = GetLength(pPos, ePos);

	if (ePos.y > 0.0f)
	{
		eVal -= eGra;
		ePos.y += eVal;
	}

	else if (ePos.y <= 0.0f)
	{
		ePos.y = 0.0f;
		eVal = 0.0f;

		switch (phase)
		{
		case Enemy::Phase::attack:
		default:
			if (!attackFlag && PElength >= 15.0f)
			{
				phase = Enemy::Phase::move;
			}
			bullet->Attack(player->GetObj(), ePos);
			break;
		case Enemy::Phase::move:
			if (PElength < 15.0f)
			{
				phase = Enemy::Phase::attack;
			}
			Move();
			break;
		}
	}

	enemyObj->SetPosition(ePos);
	enemyObj->Update();
}

void Enemy::Move()
{
	attackFlag = false;
	bullet->SetAttackFlag(attackFlag);
	XMVECTOR playerPos = { pPos.x, pPos.y, pPos.z, 1 };
	XMVECTOR enemyPos = { ePos.x, ePos.y, ePos.z, 1 };

	XMVECTOR subPlayerEnemy = XMVectorSubtract(playerPos, enemyPos);
	XMVECTOR NsubPlayerEnemy = XMVector3Normalize(subPlayerEnemy);

	XMFLOAT3 subPE = { NsubPlayerEnemy.m128_f32[0], NsubPlayerEnemy.m128_f32[1], NsubPlayerEnemy.m128_f32[2] };

	ePos.x += subPE.x / 5;
	ePos.y += subPE.y / 5;
	ePos.z += subPE.z / 5;
	bullet->SetPos(ePos);
}

XMFLOAT3 Enemy::spawnRandom(XMFLOAT3& randSpawn)
{
	srand((unsigned int)time(NULL));

	randSpawn.x = (float)(-40 + rand() % 80);
	randSpawn.y = (float)(-40 + rand() % 80);
	randSpawn.z = (float)(-40 + rand() % 80);

	return randSpawn;
}

void Enemy::Draw()
{
	enemyObj->Draw();
}

void Enemy::Finalize()
{
	delete enemyModel;
}

#include "Enemy.h"

bool Enemy::Initialize(Player* player, Bullet* bullet)
{
	assert(player);
	assert(bullet);
	this->player = player;
	this->bullet = bullet;

	enemyModel = enemyModel->CreateFromObject("sphere");
	enemy = Object3d::Create();
	enemy->SetModel(enemyModel);

	enemy->SetPosition({ 0.0f, 100.0f, 0.0f });
	enemy->SetScale({ 1.0f, 1.0f, 1.0f });
	ePos = enemy->GetPosition();

	return true;
}

void Enemy::Update()
{
	//if (!attackFlag)
	//{
	//	pPos = player->GetPos();
	//	bPos = bullet->GetPosition();
	//	bPos = ePos;
	//	bullet->SetPosition(bPos);
	//	bullet->Update();

	//	XMVECTOR playerPos = { pPos.x, pPos.y, pPos.z, 1 };
	//	XMVECTOR bulletPos = { bPos.x, bPos.y, bPos.z, 1 };

	//	XMVECTOR subPlayerEnemy = XMVectorSubtract(playerPos, bulletPos);
	//	XMVECTOR NsubPlayerEnemy = XMVector3Normalize(subPlayerEnemy);

	//	bSpeed = { NsubPlayerEnemy.m128_f32[0], NsubPlayerEnemy.m128_f32[1], NsubPlayerEnemy.m128_f32[2] };
	//}

	bPos = bullet->GetPos();
	attackFlag = bullet->GetAttackFlag();

	if (!eAlive)
	{
		eAliveCount++;
		enemy->SetPosition({ 0.0f, 100.0f, 0.0f });

		if (eAliveCount == 60)
		{
			phase = Enemy::Phase::move;
			pPos = player->GetPos();
			spawnRandom(spawnPos);
			enemy->SetPosition({ spawnPos.x, 5.0f, spawnPos.z });
			eAlive = true;
			eAliveCount = 0;
		}

		ePos = enemy->GetPosition();
		enemy->Update();
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

	enemy->SetPosition(ePos);
	enemy->Update();
	//bullet->SetPosition(bPos);
	//bullet->Update();
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

void Enemy::Attack()
{
	//attackFlag = true;
	//bPos.x += bSpeed.x / 3;
	//bPos.y += bSpeed.y / 3;
	//bPos.z += bSpeed.z / 3;

	//if (Collision::CollisionObject(player->GetObj(), bullet) || GetLength(ePos, bPos) >= 30.0f)
	//{
	//	attackFlag = false;
	//}
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
	enemy->Draw();
	//bullet->Draw();
}

void Enemy::Finalize()
{
	delete enemyModel;
	//delete bulletModel;
}

#include "Enemy.h"

bool Enemy::Initialize(Player* player)
{
	assert(player);
	this->player = player;

	enemyModel = enemyModel->CreateFromObject("sphere");
	bulletModel = bulletModel->CreateFromObject("sphere");
	enemy = Object3d::Create();
	bullet = Object3d::Create();
	enemy->SetModel(enemyModel);
	bullet->SetModel(bulletModel);

	enemy->SetPosition({ 0.0f, 5.0f, 5.0f });
	enemy->SetScale({ 1.0f, 1.0f, 1.0f });
	ePos = enemy->GetPosition();

	bullet->SetPosition(ePos);
	bullet->SetScale({ 0.5, 0.5, 0.5 });

	return true;
}

void Enemy::Update()
{
	if (!attackFlag)
	{
		pPos = player->GetPos();

		XMVECTOR playerPos = { pPos.x, pPos.y, pPos.z, 1 };
		XMVECTOR enemyPos = { bPos.x, bPos.y, bPos.z, 1 };

		XMVECTOR subPlayerEnemy = XMVectorSubtract(playerPos, enemyPos);
		XMVECTOR NsubPlayerEnemy = XMVector3Normalize(subPlayerEnemy);

		bSpeed = { NsubPlayerEnemy.m128_f32[0], NsubPlayerEnemy.m128_f32[1], NsubPlayerEnemy.m128_f32[2] };

		bPos = ePos;
		bullet->SetPosition(bPos);
		bullet->Update();
	}

	if (!eAlive)
	{
		eAliveCount++;
		enemy->SetPosition({ 0.0f, 100.0f, 0.0f });

		if (eAliveCount == 60)
		{
			enemy->SetPosition({ 0.0f, 5.0f, 5.0f });
			eAlive = true;
			eAliveCount = 0;
		}

		ePos = enemy->GetPosition();
		enemy->Update();
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
			Attack();
			if (!attackFlag && PElength >= 15.0f)
			{
				phase = Enemy::Phase::move;
			}
			break;
		case Enemy::Phase::move:
			Move();
			if (PElength < 15.0f)
			{
				phase = Enemy::Phase::attack;
			}
			break;
		}
	}

	enemy->SetPosition(ePos);
	enemy->Update();
	bullet->SetPosition(bPos);
	bullet->Update();
}

void Enemy::Move()
{
	XMVECTOR playerPos = { pPos.x, pPos.y, pPos.z, 1 };
	XMVECTOR enemyPos = { ePos.x, ePos.y, ePos.z, 1 };

	XMVECTOR subPlayerEnemy = XMVectorSubtract(playerPos, enemyPos);
	XMVECTOR NsubPlayerEnemy = XMVector3Normalize(subPlayerEnemy);

	XMFLOAT3 subPE = { NsubPlayerEnemy.m128_f32[0], NsubPlayerEnemy.m128_f32[1], NsubPlayerEnemy.m128_f32[2] };

	ePos.x += subPE.x / 5;
	ePos.y += subPE.y / 5;
	ePos.z += subPE.z / 5;
	bPos = ePos;
}

void Enemy::Attack()
{
	attackFlag = true;

	XMVECTOR playerPos = { pPos.x, pPos.y, pPos.z, 1 };
	XMVECTOR enemyPos = { bPos.x, bPos.y, bPos.z, 1 };

	XMVECTOR subPlayerEnemy = XMVectorSubtract(playerPos, enemyPos);
	XMVECTOR NsubPlayerEnemy = XMVector3Normalize(subPlayerEnemy);

	XMFLOAT3 subPE = { NsubPlayerEnemy.m128_f32[0], NsubPlayerEnemy.m128_f32[1], NsubPlayerEnemy.m128_f32[2] };

	bPos.x += bSpeed.x / 3;
	bPos.y += bSpeed.y / 3;
	bPos.z += bSpeed.z / 3;

	if (Collision::CollisionObject(player->GetObj(), bullet) || GetLength(ePos, bPos) >= 30.0f)
	{
		bPos = ePos;
		attackFlag = false;
	}
}

void Enemy::Draw()
{
	enemy->Draw();
	bullet->Draw();
}

void Enemy::Finalize()
{
	delete enemyModel;
	delete bulletModel;
}

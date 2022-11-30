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
	enemyObj->SetColor({ 0.0f, 0.8f, 0.0f, 1.0f});
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
	bullet->Update(pPos, ePos);

	if (!eAlive)
	{
		eAliveCount++;
		enemyObj->SetPosition({ 0.0f, 100.0f, 0.0f });

		if (eAliveCount == 60)
		{
			pPos = player->GetPos();
			Spawn();
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
	lengthOld = GetLength(ePos, oldePos);

	if (ePos.y > 0.0f)
	{
		eVal -= eGra;
		ePos.y += eVal;
	}

	else if (ePos.y <= 0.0f)
	{
		ePos.y = 0.0f;
		eVal = 0.0f;

		if (!attackFlag && lengthOld > 50.0f || PElength > 50.0f)
		{
			phase = Enemy::Phase::stay;
		}
		else if (!attackFlag && PElength > 20.0f && PElength <= 50.0f)
		{
			phase = Enemy::Phase::move;
		}
		else if (PElength <= 20.0f)
		{
			phase = Enemy::Phase::attack;
			attackFlag = true;
			bullet->SetAttackFlag(attackFlag);
		}

		switch (phase)
		{
		default:
		case Enemy::Phase::attack:
			bullet->Attack();
			break;
		case Enemy::Phase::move:
			Move();
			break;
		case Enemy::Phase::stay:
			Stay();
			break;
		}
	}

	enemyObj->SetPosition(ePos);
	enemyObj->Update();
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
}

void Enemy::Stay()
{
	XMVECTOR oldEnemyPos = { oldePos.x, oldePos.y, oldePos.z, 1 };
	XMVECTOR enemyPos = { ePos.x, ePos.y, ePos.z, 1 };

	XMVECTOR subOldEnemy = XMVectorSubtract(oldEnemyPos, enemyPos);
	XMVECTOR NsubOldEnemy = XMVector3Normalize(subOldEnemy);

	XMFLOAT3 subPE = { NsubOldEnemy.m128_f32[0], NsubOldEnemy.m128_f32[1], NsubOldEnemy.m128_f32[2] };

	if (lengthOld <= 0.3f)
	{
		ePos = oldePos;
	}
	else
	{
		ePos.x += subPE.x / 5;
		ePos.y += subPE.y / 5;
		ePos.z += subPE.z / 5;
	}
}

void Enemy::Spawn()
{
	randPos.x = (float)(-40 + rand() % 80);
	randPos.y = (float)(-3 + rand() % 3);
	randPos.z = (float)(-40 + rand() % 80);

	phase = Enemy::Phase::move;
	enemyObj->SetPosition({ randPos.x, 5.0f, randPos.z });
	enemyObj->SetScale({ 1.0f, 1.0f, 1.0f });
	enemyObj->SetColor({ 0.0f, 0.8f, 0.0f, 1.0f });
	ePos = enemyObj->GetPosition();
	oldePos = { ePos.x, 0.0f, ePos.z };
}

void Enemy::Draw()
{
	enemyObj->Draw();
}

void Enemy::Finalize()
{
	delete enemyModel;
}

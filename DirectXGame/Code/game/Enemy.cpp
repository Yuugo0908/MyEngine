#include "Enemy.h"

bool Enemy::Initialize(Bullet* bullet, Player* player)
{
	assert(bullet);
	assert(player);
	this->bullet = bullet;
	this->player = player;

	enemyModel = enemyModel->CreateFromObject("sphere");
	enemyObj = Object3d::Create();
	enemyObj->SetModel(enemyModel);

	return true;
}

void Enemy::Update(const XMFLOAT3 pPos, const XMFLOAT3 bPos)
{
	this->pPos = pPos;
	this->bPos = bPos;

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
			//if (!attackFlag && PElength >= 15.0f)
			//{
			//	phase = Enemy::Phase::move;
			//}
			//bullet->Attack(player->GetObj(), ePos);
			break;
		case Enemy::Phase::move:
			//if (PElength < 15.0f)
			//{
			//	phase = Enemy::Phase::attack;
			//}
			Move();
			//Collision();
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

void Enemy::Spawn(float oldrandPos)
{
	srand((unsigned int)time(NULL));
	while (1)
	{
		randPos.x = (float)(-40 + rand() % 80);
		randPos.y = (float)(-3 + rand() % 3);
		randPos.z = (float)(-40 + rand() % 80);

		if (ePos.y != randPos.y)
		{
			break;
		}
	}

	phase = Enemy::Phase::move;
	enemyObj->SetPosition({ randPos.x, 5.0f, randPos.z });
	enemyObj->SetScale({ 1.0f, 1.0f, 1.0f });
	ePos = enemyObj->GetPosition();
}

void Enemy::Collision()
{
	if (Collision::CollisionObject(player->GetObj(), enemyObj))
	{
		Camera::GetInstance()->CameraShake(shakeFlag);
		eAlive = false;
	}
}

void Enemy::Draw()
{
	enemyObj->Draw();
}

void Enemy::Finalize()
{
	delete enemyModel;
}

#include "Enemy.h"

Model* Enemy::bulletModel = nullptr;

bool Enemy::Initialize(Player* player)
{
	assert(player);
	this->player = player;

	bulletModel = bulletModel->CreateFromObject("sphere");
	enemyModel = enemyModel->CreateFromObject("sphere");
	enemyObj = Object3d::Create();
	enemyObj->SetModel(enemyModel);

	enemyObj->SetPosition({ 0.0f, 100.0f, 0.0f });
	enemyObj->SetScale({ 1.0f, 1.0f, 1.0f });
	enemyObj->SetColor({ 0.0f, 0.8f, 0.0f, 1.0f});
	ePos = enemyObj->GetPosition();

	return true;
}

bool Enemy::BulletCreate()
{
	std::unique_ptr<Bullet> newBullet = std::make_unique<Bullet>();
	newBullet->Initialize(bulletModel);
	newBullet->SetPos(ePos);
	attackFlag = false;
	newBullet->SetAttackFlag(attackFlag);
	bullets.push_back(std::move(newBullet));
	return true;
}

void Enemy::Update()
{
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

		ePos = enemyObj->GetPosition();
		enemyObj->Update();
		if (!LoadFlag)
		{
			BulletCreate();
			LoadFlag = true;
		}
		//LoadFlag = false;
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

		if (!attackFlag && (PElength > 10.0f && PElength <= 40.0f))
		{
			phase = Enemy::Phase::move;
		}
		else if(!attackFlag)
		{
			phase = Enemy::Phase::stay;
		}

		if (PElength <= 15.0f)
		{
			phase = Enemy::Phase::attack;
			for (std::unique_ptr<Bullet>& bullet : bullets)
			{
				attackFlag = true;
				bullet->SetAttackFlag(attackFlag);
			}
		}

		switch (phase)
		{
		default:
		case Enemy::Phase::attack:
			for (std::unique_ptr<Bullet>& bullet : bullets)
			{
				bullet->Attack();
			}
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
	//ePos.y += subPE.y / 3;
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
		//ePos.y += subPE.y / 5;
		ePos.z += subPE.z / 5;
	}
}

void Enemy::Spawn()
{
	randPos.x = (float)(-40 + rand() % 80);
	randPos.z = (float)(-40 + rand() % 80);

	phase = Enemy::Phase::move;
	enemyObj->SetPosition({ randPos.x, 5.0f, randPos.z });
	enemyObj->SetScale({ 1.0f, 1.0f, 1.0f });
	enemyObj->SetColor({ 0.0f, 0.8f, 0.0f, 1.0f });
	ePos = enemyObj->GetPosition();
	oldePos = { ePos.x, 0.0f, ePos.z };
}

bool Enemy::EnemyCollision()
{
	for (std::unique_ptr<Bullet>& bullet : bullets)
	{
		eAlive = false;
		bullets.remove(bullet);
		break;
	}
	return true;
}

bool Enemy::BulletCollision()
{
	for (std::unique_ptr<Bullet>& bullet : bullets)
	{
		attackFlag = bullet->GetAttackFlag();
		bullet->Update(pPos, ePos);
		if (Collision::CollisionObject(bullet->GetObj(), player->GetObj()))
		{
			bPos = ePos;
			bullet->SetPos(bPos);
			bullet->Collision();
			return true;
		}
	}
	return false;
}

void Enemy::Draw()
{
	enemyObj->Draw();

	for (std::unique_ptr<Bullet>& bullet : bullets)
	{
		if (bullet->GetAttackFlag())
		{
			bullet->Draw();
		}
	}
}

void Enemy::Finalize()
{
	delete enemyModel;
	delete bulletModel;
}

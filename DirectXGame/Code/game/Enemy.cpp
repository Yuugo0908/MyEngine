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
			onGround = false;
			attackFlag = false;
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

	// ジャンプ
	if (jumpFlag)
	{
		eUp -= eGra;
		ePos.y += eUp;
		if (onGround)
		{
			jumpFlag = false;
			eUp = 0.0f;
		}
	}
	else
	{
		eDown -= 0.2f;
		ePos.y += eDown;
		if (onGround)
		{
			eDown = 0.0f;
		}
	}


	if (!attackFlag && (PElength > 10.0f && PElength <= 40.0f))
	{
		phase = Enemy::Phase::move;
	}
	else if(!attackFlag)
	{
		phase = Enemy::Phase::stay;
	}

	if (onGround)
	{
		if (PElength <= 15.0f)
		{
			phase = Enemy::Phase::attack;
			for (std::unique_ptr<Bullet>& bullet : bullets)
			{
				attackFlag = true;
				bullet->SetAttackFlag(attackFlag);
			}
		}
	}

	switch (phase)
	{
	default:
	case Enemy::Phase::attack:
		for (std::unique_ptr<Bullet>& bullet : bullets)
		{
			if (attackFlag)
			{
				bullet->Attack();
			}
		}
		break;
	case Enemy::Phase::move:
		Move();
		break;
	case Enemy::Phase::stay:
		Stay();
		break;
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
	enemyObj->SetPosition({ randPos.x, 8.0f, randPos.z });
	enemyObj->SetScale({ 1.0f, 1.0f, 1.0f });
	enemyObj->SetColor({ 0.0f, 0.8f, 0.0f, 1.0f });
	ePos = enemyObj->GetPosition();
	oldePos = { ePos.x, 0.0f, ePos.z };
}

bool Enemy::EnemyCollision()
{
	eAlive = false;
	LoadFlag = false;
	for (std::unique_ptr<Bullet>& bullet : bullets)
	{
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

bool Enemy::MapCollide(XMFLOAT3 boxPos, XMFLOAT3 boxRadius, XMFLOAT3& pos, XMFLOAT3 radius, int mapNumber, const XMFLOAT3 oldPos)
{
	//フラグ
	bool hitFlag = false;

	// 判定
	float maxBoxX = boxPos.x + boxRadius.x;
	float minBoxX = boxPos.x - boxRadius.x;
	float maxBoxY = boxPos.y + boxRadius.y;
	float minBoxY = boxPos.y - boxRadius.y;
	float maxBoxZ = boxPos.z + boxRadius.z;
	float minBoxZ = boxPos.z - boxRadius.z;

	if ((pos.x <= maxBoxX && pos.x >= minBoxX) &&
		(pos.z <= maxBoxZ && pos.z >= minBoxZ))
	{
		if (maxBoxY + radius.y > pos.y && boxPos.y < oldPos.y)
		{
			pos.y = maxBoxY + radius.y;
			hitFlag = true;
			onGround = true;
		}
		else if (minBoxY - radius.y < pos.y && boxPos.y > oldPos.y)
		{
			pos.y = minBoxY - radius.y;
			hitFlag = true;
			onGround = true;
		}
	}

	if ((pos.x <= maxBoxX && pos.x >= minBoxX) &&
		(pos.y <= maxBoxY && pos.y >= minBoxY))
	{
		if (maxBoxZ + radius.z > pos.z && boxPos.z < oldPos.z)
		{
			pos.z = maxBoxZ + radius.z;
			hitFlag = true;
			if (!jumpFlag)
			{
				onGround = false;
				jumpFlag = true;
				// 上昇率の更新
				eUp = 1.25f;
			}
		}
		else if (minBoxZ - radius.z < pos.z && boxPos.z > oldPos.z)
		{
			pos.z = minBoxZ - radius.z;
			hitFlag = true;
			if (!jumpFlag)
			{
				onGround = false;
				jumpFlag = true;
				// 上昇率の更新
				eUp = 1.25f;
			}
		}
	}

	if ((pos.z <= maxBoxZ && pos.z >= minBoxZ) &&
		(pos.y <= maxBoxY && pos.y >= minBoxY))
	{
		if (maxBoxX + radius.x > pos.x && boxPos.x < oldPos.x)
		{
			pos.x = maxBoxX + radius.x;
			hitFlag = true;
			if (!jumpFlag)
			{
				onGround = false;
				jumpFlag = true;
				// 上昇率の更新
				eUp = 1.25f;
			}
		}
		else if (minBoxX - radius.x < pos.x && boxPos.x > oldPos.x)
		{
			pos.x = minBoxX - radius.x;
			hitFlag = true;
			if (!jumpFlag)
			{
				onGround = false;
				jumpFlag = true;
				// 上昇率の更新
				eUp = 1.25f;
			}
		}
	}

	return hitFlag;
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

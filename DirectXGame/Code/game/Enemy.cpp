#include "Enemy.h"

Model* Enemy::bulletModel = nullptr;
Model* Enemy::enemyModel = nullptr;

bool Enemy::StaticInit()
{
	bulletModel = bulletModel->CreateFromObject("sphere");
	enemyModel = enemyModel->CreateFromObject("sphere");

	if (bulletModel == nullptr || enemyModel == nullptr)
	{
		assert(NULL);
		return false;
	}

	return true;
}

bool Enemy::Initialize(Player* player)
{
	assert(player);
	this->player = player;

	enemyObj = Object3d::Create();
	enemyObj->SetModel(enemyModel);

	enemyObj->SetPosition({ 0.0f, 0.0f, 0.0f });
	enemyObj->SetScale({ 1.0f, 1.0f, 1.0f });
	enemyObj->SetColor({ 0.0f, 0.8f, 0.0f, 1.0f});
	ePos = enemyObj->GetPosition();
	eRadius = enemyObj->GetScale();

	return true;
}

bool Enemy::BulletCreate()
{
	std::unique_ptr<Bullet> newBullet = std::make_unique<Bullet>();
	newBullet->Initialize(bulletModel);
	newBullet->SetPos(ePos);
	bullets.push_back(std::move(newBullet));
	return true;
}

void Enemy::Update()
{
	if (!eAlive)
	{
		eAliveCount++;

		if (eAliveCount == 60)
		{
			pPos = player->GetPos();
			Spawn();
			eAlive = true;
			onGround = false;
			attackFlag = true;
			eAliveCount = 0;
		}

		ePos = enemyObj->GetPosition();
		eOldPos = ePos;
		enemyObj->Update();

		return;
	}

	eOldPos = ePos;
	if (attackCount <= 60)
	{
		attackCount++;
	}

	pPos = player->GetPos();
	PElength = GetLength(pPos, ePos);
	lengthOld = GetLength(ePos, spawnPos);

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


	if (PElength <= 30.0f)
	{
		phase = Enemy::Phase::move;
	}
	else
	{
		phase = Enemy::Phase::stay;
	}

	if (onGround)
	{
		if (PElength <= 15.0f)
		{
			phase = Enemy::Phase::attack;
		}
	}

	switch (phase)
	{
	case Enemy::Phase::attack:
		Attack();
		break;
	case Enemy::Phase::move:
		Move();
		break;
	case Enemy::Phase::stay:
	default:
		Stay();
	}

	enemyObj->SetPosition(ePos);
	enemyObj->Update();
}

void Enemy::Attack()
{
	if (attackFlag && attackCount >= 60)
	{
		BulletCreate();
		attackCount = 0;
	}

	for (std::unique_ptr<Bullet>& bullet : bullets)
	{
		bullet->Search();
	}
}

void Enemy::Move()
{
	XMVECTOR playerPos = { pPos.x, pPos.y, pPos.z, 1 };
	XMVECTOR enemyPos = { ePos.x, ePos.y, ePos.z, 1 };

	XMVECTOR subPlayerEnemy = XMVectorSubtract(playerPos, enemyPos);
	XMVECTOR NsubPlayerEnemy = XMVector3Normalize(subPlayerEnemy);

	XMFLOAT3 subPE = { NsubPlayerEnemy.m128_f32[0], NsubPlayerEnemy.m128_f32[1], NsubPlayerEnemy.m128_f32[2] };

	ePos.x += subPE.x / 5;
	ePos.z += subPE.z / 5;
}

void Enemy::Stay()
{
	XMVECTOR oldEnemyPos = { spawnPos.x, spawnPos.y, spawnPos.z, 1 };
	XMVECTOR enemyPos = { ePos.x, ePos.y, ePos.z, 1 };

	XMVECTOR subOldEnemy = XMVectorSubtract(oldEnemyPos, enemyPos);
	XMVECTOR NsubOldEnemy = XMVector3Normalize(subOldEnemy);

	XMFLOAT3 subPE = { NsubOldEnemy.m128_f32[0], NsubOldEnemy.m128_f32[1], NsubOldEnemy.m128_f32[2] };

	if (lengthOld <= 0.0f)
	{
		ePos = spawnPos;
	}
	else
	{
		ePos.x += subPE.x / 5;
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
	spawnPos = { ePos.x, 0.0f, ePos.z };
}

void Enemy::Reset()
{
	eAlive = false;
	for (std::unique_ptr<Bullet>& bullet : bullets)
	{
		bullets.remove(bullet);
		break;
	}
}

bool Enemy::EnemyCollision()
{
	eAlive = false;
	eAliveCount = 0;
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
		bullet->Update(pPos, ePos);
		if (Collision::CollisionObject(bullet->GetObj(), player->GetObj()))
		{
			bullets.remove(bullet);
			return true;
		}

		if (GetLength(ePos, bullet->GetPos()) >= 20.0f)
		{
			bullets.remove(bullet);
			return false;
		}
	}
	return false;
}

bool Enemy::MapCollide(XMFLOAT3 boxPos, XMFLOAT3 boxRadius)
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

	if ((ePos.x <= maxBoxX && ePos.x >= minBoxX) &&
		(ePos.z <= maxBoxZ && ePos.z >= minBoxZ))
	{
		if (maxBoxY + eRadius.y > ePos.y && boxPos.y < eOldPos.y)
		{
			//ePos.y = maxBoxY + eRadius.y;
			hitFlag = true;
			if (maxBoxY + eRadius.y >= ePos.y)
			{
				ePos.y = eOldPos.y;
			}
			onGround = true;
		}
		else if (minBoxY - eRadius.y < ePos.y && boxPos.y > eOldPos.y)
		{
			//ePos.y = minBoxY - eRadius.y;
			hitFlag = true;
			if (maxBoxY + eRadius.y <= ePos.y)
			{
				ePos.y = eOldPos.y;
			}
		}
	}

	if ((ePos.x <= maxBoxX && ePos.x >= minBoxX) &&
		(ePos.y <= maxBoxY && ePos.y >= minBoxY))
	{
		if (maxBoxZ + eRadius.z > ePos.z && boxPos.z < eOldPos.z)
		{
			ePos.z = maxBoxZ + eRadius.z;
			hitFlag = true;
			if (!jumpFlag && onGround)
			{
				onGround = false;
				jumpFlag = true;
				// 上昇率の更新
				eUp = 1.25f;
			}
		}
		else if (minBoxZ - eRadius.z < ePos.z && boxPos.z > eOldPos.z)
		{
			ePos.z = minBoxZ - eRadius.z;
			hitFlag = true;
			if (!jumpFlag && onGround)
			{
				onGround = false;
				jumpFlag = true;
				// 上昇率の更新
				eUp = 1.25f;
			}
		}
	}

	if ((ePos.z <= maxBoxZ && ePos.z >= minBoxZ) &&
		(ePos.y <= maxBoxY && ePos.y >= minBoxY))
	{
		if (maxBoxX + eRadius.x > ePos.x && boxPos.x < eOldPos.x)
		{
			ePos.x = maxBoxX + eRadius.x;
			hitFlag = true;
			if (!jumpFlag && onGround)
			{
				onGround = false;
				jumpFlag = true;
				// 上昇率の更新
				eUp = 1.25f;
			}
		}
		else if (minBoxX - eRadius.x < ePos.x && boxPos.x > eOldPos.x)
		{
			ePos.x = minBoxX - eRadius.x;
			hitFlag = true;
			if (!jumpFlag && onGround)
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

bool Enemy::StageCollide(XMFLOAT3 stagePos, XMFLOAT3 stageScale)
{
	//ePos = enemyObj->GetPosition();
	//eRadius = enemyObj->GetScale();

	// 判定
	float maxX = stagePos.x + stageScale.x;
	float maxY = stagePos.y + stageScale.y;
	float maxZ = stagePos.z + stageScale.z;
	float minX = stagePos.x - stageScale.x;
	float minY = stagePos.y - stageScale.y;
	float minZ = stagePos.z - stageScale.z;

	bool hitFlag = false;

	if ((ePos.x < maxX && ePos.x > minX) &&
		(ePos.z < maxZ && ePos.z > minZ))
	{
		if (maxY + eRadius.y > ePos.y && stagePos.y < eOldPos.y)
		{
			if (stagePos.y + eRadius.y >= ePos.y)
			{
				ePos.y = eOldPos.y;
			}
			hitFlag = true;
		}
	}
	//enemyObj->SetPosition(ePos);
	enemyObj->Update();

	return hitFlag;
}

bool Enemy::RopeCollide(XMFLOAT3 rPos, XMFLOAT3 rRadius)
{
	//フラグ
	bool hitFlag = false;

	// 判定
	float maxRopeX = rPos.x + rRadius.x;
	float maxRopeY = rPos.y + rRadius.y;
	float maxRopeZ = rPos.z + rRadius.z;
	float minRopeX = rPos.x - rRadius.x;
	float minRopeY = rPos.y - rRadius.y;
	float minRopeZ = rPos.z - rRadius.z;

	float maxEnemyX = ePos.x + eRadius.x;
	float maxEnemyY = ePos.y + eRadius.y;
	float maxEnemyZ = ePos.z + eRadius.z;
	float minEnemyX = ePos.x - eRadius.x;
	float minEnemyY = ePos.y - eRadius.y;
	float minEnemyZ = ePos.z - eRadius.z;

	if ((minRopeX < maxEnemyX && maxRopeX > minEnemyX) &&
		(minRopeY < maxEnemyY && maxRopeY > minEnemyY) &&
		(minRopeZ < maxEnemyZ && maxRopeZ > minEnemyZ))
	{
		hitFlag = true;
	}

	//if ((ePos.x < maxRopeX && ePos.x > minRopeX) &&
	//	(ePos.z < maxRopeZ && ePos.z > minRopeZ))
	//{
	//	if (maxRopeY> ePos.y)
	//	{
	//		hitFlag = true;
	//	}
	//	else if (minRopeY < ePos.y)
	//	{
	//		hitFlag = true;
	//	}
	//}

	//if ((ePos.x < maxRopeX && ePos.x > minRopeX) &&
	//	(ePos.y < maxRopeY && ePos.y > minRopeY))
	//{
	//	if (maxRopeZ > ePos.z)
	//	{
	//		hitFlag = true;
	//	}
	//	else if (minRopeZ < ePos.z)
	//	{
	//		hitFlag = true;
	//	}
	//}

	//if ((ePos.z < maxRopeZ && ePos.z > minRopeZ) &&
	//	(ePos.y < maxRopeY && ePos.y > minRopeY))
	//{
	//	if (maxRopeX > ePos.x)
	//	{
	//		hitFlag = true;
	//	}
	//	else if (minRopeX < ePos.x)
	//	{
	//		hitFlag = true;
	//	}
	//}

	enemyObj->Update();

	return hitFlag;
}

void Enemy::Draw()
{
	if (eAlive)
	{
		enemyObj->Draw();
	}

	for (std::unique_ptr<Bullet>& bullet : bullets)
	{
		bullet->Draw();
	}
}

void Enemy::Finalize()
{
	delete enemyModel;
	delete bulletModel;
}

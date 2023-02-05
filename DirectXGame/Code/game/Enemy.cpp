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

	randPos.x = (float)(-40 + rand() % 80);
	randPos.z = (float)(-40 + rand() % 80);

	phase = Enemy::Phase::stay;
	pPos = player->GetObj()->GetPosition();
	ePos = { randPos.x, 15.0f, randPos.z };
	//ePos = { 0.0f, 15.0f, 20.0f };
	eScale = { 1.0f, 1.0f, 1.0f };

	enemyObj->SetPosition(ePos);
	enemyObj->SetScale(eScale);
	enemyObj->SetColor({ 0.0f, 0.8f, 0.0f, 1.0f });
	enemyObj->Update();
	spawnPos = { ePos.x, 0.0f, ePos.z };
	eAlive = true;

	return true;
}

bool Enemy::BulletCreate()
{
	std::unique_ptr<Bullet> newBullet = std::make_unique<Bullet>();
	newBullet->Initialize(bulletModel, pPos, ePos);
	newBullet->SetPos(enemyObj->GetPosition());
	bullets.push_back(std::move(newBullet));
	return true;
}

void Enemy::Update()
{
	if (!eAlive)
	{
		if (eAliveCount <= 60)
		{
			eAliveCount++;
		}
		else
		{
			Spawn();
			eAlive = true;
			phase = Enemy::Phase::stay;
			return;
		}
	}

	if (attackCount <= 30)
	{
		attackCount++;
	}

	eOldPos = enemyObj->GetPosition();
	enemyObj->SetPosition(ePos);

	pPos = player->GetObj()->GetPosition();
	PElength = GetLength(pPos, ePos);
	lengthOld = GetLength(ePos, spawnPos);

	for (std::unique_ptr<Bullet>& bullet : bullets)
	{
		bullet->Update();
	}
	Jump();

	if (attackFlag)
	{
		if (PElength <= 20.0f)
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
	}
	else
	{
		phase = Enemy::Phase::stay;
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

	enemyObj->Update();
}

void Enemy::Attack()
{
	if (eAlive && attackCount >= 30)
	{
		BulletCreate();
		attackCount = 0;
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

void Enemy::Jump()
{
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
		eDown -= 0.75f;
		ePos.y += eDown;
		if (onGround)
		{
			eDown = 0.0f;
		}
	}
}

void Enemy::Spawn()
{
	randPos.x = (float)(-40 + rand() % 80);
	randPos.z = (float)(-40 + rand() % 80);

	phase = Enemy::Phase::stay;
	ePos = { randPos.x, 15.0f, randPos.z };
	eScale = { 1.0f, 1.0f, 1.0f };

	enemyObj->SetPosition(ePos);
	enemyObj->SetScale(eScale);
	enemyObj->SetColor({ 0.0f, 0.8f, 0.0f, 1.0f });
	enemyObj->Update();
	spawnPos = { ePos.x, 0.0f, ePos.z };
}

void Enemy::Reset()
{
	eAlive = false;
	bullets.erase(bullets.begin(), bullets.end());
}

bool Enemy::EnemyCollision(const std::unique_ptr<Object3d>& object)
{
	if (!Collision::CollisionObject(object, enemyObj))
	{
		return false;
	}
	else
	{
		eAlive = false;
		eAliveCount = 0;
		catchFlag = false;
		bullets.erase(bullets.begin(), bullets.end());
		return true;
	}
}

bool Enemy::BulletCollision()
{
	for (std::unique_ptr<Bullet>& bullet : bullets)
	{
		if (GetLength(ePos, bullet->GetPos()) >= 30.0f)
		{
			bullets.remove(bullet);
			return false;
		}
		if (Collision::CollisionObject(bullet->GetObj(), player->GetObj()))
		{
			bullets.remove(bullet);
			return true;
		}
	}
	return false;
}

bool Enemy::MapCollide(XMFLOAT3 boxPos, XMFLOAT3 boxScale)
{
	for (std::unique_ptr<Bullet>& bullet : bullets)
	{
		if (bullet->MapCollide(boxPos, boxScale))
		{
			bullets.remove(bullet);
			break;
		}
	}

	//フラグ
	bool hitFlag = false;

	// 判定
	float maxBoxX = boxPos.x + boxScale.x;
	float minBoxX = boxPos.x - boxScale.x;
	float maxBoxY = boxPos.y + boxScale.y;
	float minBoxY = boxPos.y - boxScale.y;
	float maxBoxZ = boxPos.z + boxScale.z;
	float minBoxZ = boxPos.z - boxScale.z;

	if ((ePos.x <= maxBoxX && ePos.x >= minBoxX) &&
		(ePos.z <= maxBoxZ && ePos.z >= minBoxZ))
	{
		if (maxBoxY + eScale.y > ePos.y && boxPos.y < eOldPos.y)
		{
			hitFlag = true;
			if (maxBoxY + eScale.y >= ePos.y)
			{
				ePos.y = eOldPos.y;
			}
			onGround = true;
		}
		else if (minBoxY - eScale.y < ePos.y && boxPos.y > eOldPos.y)
		{
			hitFlag = true;
			if (maxBoxY + eScale.y <= ePos.y)
			{
				ePos.y = eOldPos.y;
			}
		}
	}

	if ((ePos.x <= maxBoxX && ePos.x >= minBoxX) &&
		(ePos.y <= maxBoxY && ePos.y >= minBoxY))
	{
		if (maxBoxZ + eScale.z > ePos.z && boxPos.z < eOldPos.z)
		{
			ePos.z = maxBoxZ + eScale.z;
			hitFlag = true;
			if (!jumpFlag && onGround)
			{
				onGround = false;
				jumpFlag = true;
				// 上昇率の更新
				eUp = 1.25f;
			}
		}
		else if (minBoxZ - eScale.z < ePos.z && boxPos.z > eOldPos.z)
		{
			ePos.z = minBoxZ - eScale.z;
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
		if (maxBoxX + eScale.x > ePos.x && boxPos.x < eOldPos.x)
		{
			ePos.x = maxBoxX + eScale.x;
			hitFlag = true;
			if (!jumpFlag && onGround)
			{
				onGround = false;
				jumpFlag = true;
				// 上昇率の更新
				eUp = 1.25f;
			}
		}
		else if (minBoxX - eScale.x < ePos.x && boxPos.x > eOldPos.x)
		{
			ePos.x = minBoxX - eScale.x;
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

	enemyObj->SetPosition(ePos);
	enemyObj->Update();

	return hitFlag;
}

bool Enemy::StageCollide(XMFLOAT3 stagePos, XMFLOAT3 stageScale)
{
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
		if (maxY + eScale.y > ePos.y && stagePos.y < eOldPos.y)
		{
			if (stagePos.y + eScale.y >= ePos.y)
			{
				ePos.y = eOldPos.y;
			}
			hitFlag = true;
		}
	}

	enemyObj->SetPosition(ePos);
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

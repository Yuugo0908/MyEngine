#include "Enemy.h"

Model* Enemy::bulletModel = nullptr;
Model* Enemy::enemyModel = nullptr;

bool Enemy::ModelInit()
{
	bulletModel = bulletModel->CreateFromObject("bullet");
	enemyModel = enemyModel->CreateFromObject("enemy");

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
	eAlive = false;

	exclamation_mark = Particle::Create("exclamation_mark");
	question_mark = Particle::Create("question_mark");

	return true;
}

bool Enemy::BulletCreate()
{
	// �e�̐����J�E���g
	if (attackCount <= 10)
	{
		attackCount++;
		return false;
	}

	std::unique_ptr<Bullet> newBullet = std::make_unique<Bullet>();
	newBullet->Initialize(bulletModel, pPos, ePos);
	newBullet->SetPos(enemyObj->GetPosition());
	bullets.push_back(std::move(newBullet));
	attackCount = 0;
	return true;
}

void Enemy::Update()
{
	if (!eAlive)
	{
		eAlive = true;
		phase = Enemy::Phase::stay;

		ePos = enemyObj->GetPosition();
		eOldPos = ePos;
		eScale = { 0.8f, 0.8f, 0.8f };

		enemyObj->SetPosition(ePos);
		enemyObj->SetScale(eScale);
		enemyObj->SetRotation({ 0, 30, 0 });
		enemyObj->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		enemyObj->Update();
		return;
	}

	// �G�������W�܂ŗ��������ꍇ�A�X�|�[���n�_�ɖ߂�
	ReSpawn();

	// 1�t���[���O�̓G�̍��W
	eOldPos = enemyObj->GetPosition();
	// ���W�X�V
	enemyObj->SetPosition(ePos);

	pPos = player->GetObj()->GetPosition();
	PElength = GetLength(pPos, ePos);
	lengthOld = GetLength(ePos, spawnPos);

	// �v���C���[�̃T�[�`����
	Search();

	for (std::unique_ptr<Bullet>& bullet : bullets)
	{
		bullet->Update();
	}

	// ��苗�����Ȃ�߂Â��Ă���
	//if (PElength <= 30.0f)
	//{
	//	phase = Enemy::Phase::move;
	//}

	// �U���͈͂Ȃ�U�����Ă���
	//if (attackFlag)
	//{
	//	if (PElength <= 15.0f)
	//	{
	//		phase = Enemy::Phase::attack;
	//	}
	//}
	//else
	//{
	//	phase = Enemy::Phase::stay;
	//}

	switch (phase)
	{
	case Enemy::Phase::attack:
		if (!visibleFlag)
		{
			exclamation_mark->CreateParticles(
				{ ePos.x, ePos.y + 3.0f, ePos.z },
				2.0f, 2.0f,
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				1, 30, false, false
			);

			visibleFlag = true;
			invisibleFlag = false;

			// �W�����v����
			onGround = false;
			jumpFlag = true;
			// �㏸���̍X�V
			eUp = 0.5f;
		}
		Attack();
		break;
	case Enemy::Phase::move:
		if (!visibleFlag)
		{
			exclamation_mark->CreateParticles(
				{ ePos.x, ePos.y + 3.0f, ePos.z },
				2.0f, 2.0f,
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				1, 30, false, false
			);

			visibleFlag = true;
			invisibleFlag = false;

			// �W�����v����
			onGround = false;
			jumpFlag = true;
			// �㏸���̍X�V
			eUp = 0.5f;
		}
		Move();
		break;
	case Enemy::Phase::stay:
	default:
		if (!invisibleFlag)
		{
			question_mark->CreateParticles(
				{ ePos.x, ePos.y + 3.0f, ePos.z },
				2.0f, 2.0f,
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				1, 30, false, false
			);

			visibleFlag = false;
			invisibleFlag = true;
		}
		Stay();
	}

	Jump();
	enemyObj->Update();
}

void Enemy::Attack()
{
	if (eAlive)
	{
		BulletCreate();
	}
	TrackRot(ePos, pPos);
}

void Enemy::Move()
{
	//XMVECTOR playerPos = { pPos.x, pPos.y, pPos.z, 1 };
	//XMVECTOR enemyPos = { ePos.x, ePos.y, ePos.z, 1 };

	//XMVECTOR subPlayerEnemy = XMVectorSubtract(playerPos, enemyPos);
	//XMVECTOR NsubPlayerEnemy = XMVector3Normalize(subPlayerEnemy);

	//XMFLOAT3 subPE = { NsubPlayerEnemy.m128_f32[0], NsubPlayerEnemy.m128_f32[1], NsubPlayerEnemy.m128_f32[2] };

	//ePos.x += subPE.x / 5;
	//ePos.z += subPE.z / 5;
	TrackRot(ePos, pPos);
	enemyObj->Update();
}

void Enemy::Stay()
{
	//enemyObj->SetRotation({});
	enemyObj->Update();
}

void Enemy::Jump()
{
	// �W�����v
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
		eDown = -0.75f;
		ePos.y += eDown;
		if (onGround)
		{
			eDown = 0.0f;
		}
	}
}

void Enemy::TrackRot(const XMFLOAT3& startPos, const XMFLOAT3& endPos)
{
	XMVECTOR sPos = { startPos.x, startPos.y, startPos.z, 0 };
	XMVECTOR ePos = { endPos.x, endPos.y, endPos.z, 0 };

	XMVECTOR subPos = XMVectorSubtract(sPos, ePos);

	float angle = (float)atan2(subPos.m128_f32[0], subPos.m128_f32[2]);

	enemyObj->SetRotation({ 0, XMConvertToDegrees(angle), 0 });
}

void Enemy::ReSpawn()
{
	if (ePos.y <= limitPos)
	{
		eAlive = true;
		phase = Enemy::Phase::stay;

		ePos = spawnPos;
		eOldPos = ePos;
		eScale = { 0.8f, 0.8f, 0.8f };

		enemyObj->SetPosition(ePos);
		enemyObj->SetScale(eScale);
		enemyObj->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		enemyObj->Update();
	}
}

void Enemy::Search()
{
	XMVECTOR sPos = { ePos.x, ePos.y, ePos.z, 0 };
	XMVECTOR ePos = { pPos.x, pPos.y, pPos.z, 0 };
	XMFLOAT3 enemyRot = enemyObj->GetRotation();

	XMVECTOR subPos = XMVectorSubtract(sPos, ePos);

	float angle = (float)atan2(subPos.m128_f32[0], subPos.m128_f32[2]);
	angle = XMConvertToDegrees(angle);
	angle = fabsf(angle);
	enemyRot.y = fabsf(enemyRot.y);
	float angleMax = angle + enemyRot.y;
	float angleMin = angle - enemyRot.y;


	DebugText::GetInstance()->Print(100, 100, 2, "%f", angle);

	// �v���C���[���痣��Ă�����T�[�`���Ȃ�
	if (PElength >= 10.0f)
	{
		phase = Enemy::Phase::stay;
		attackFlag = false;
		return;
	}

	if (angle <= 60.0f)
	{
		phase = Enemy::Phase::move;
	}
	else
	{
		phase = Enemy::Phase::stay;
	}
}

void Enemy::Reset()
{
	eAlive = false;
	bullets.erase(bullets.begin(), bullets.end());
}

bool Enemy::ObstacleDetection(XMFLOAT3 pPos, XMFLOAT3 boxPos, XMFLOAT3 boxScale)
{
	float length = GetLength(pPos, ePos);

	if (length < 30.0f)
	{
		if (Collision::CollisionRayBox(pPos, ePos, boxPos, boxScale))
		{
			attackFlag = false;
			return true;
		}
		else
		{
			attackFlag = true;
			return false;
		}
	}
	return false;
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
		bullets.erase(bullets.begin(), bullets.end());
		return true;
	}
}

bool Enemy::BulletCollision()
{
	for (std::unique_ptr<Bullet>& bullet : bullets)
	{
		// �G������̋������ꂽ�������
		if (GetLength(ePos, bullet->GetPos()) >= 30.0f)
		{
			bullets.remove(bullet);
			return false;
		}

		// �e���v���C���[�ɓ��������������
		else if (Collision::CollisionObject(bullet->GetObj(), player->GetObj()))
		{
			bullets.remove(bullet);
			return true;
		}
	}
	return false;
}

bool Enemy::Danger()
{
	for (std::unique_ptr<Bullet>& bullet : bullets)
	{
		if (GetLength(player->GetObj()->GetPosition(), bullet->GetPos()) <= 5.0f)
		{
			return true;
		}
	}
	return false;
}

bool Enemy::MapCollide(const XMFLOAT3 boxPos, const XMFLOAT3 boxScale)
{
	if (!eAlive)
	{
		return false;
	}

	// �e�̃}�b�v�I�u�W�F�N�g�̓����蔻��
	for (std::unique_ptr<Bullet>& bullet : bullets)
	{
		if (bullet->MapCollide(boxPos, boxScale))
		{
			bullets.remove(bullet);
			break;
		}
	}

	//�t���O
	bool hitFlag = false;

	// ����
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
			if (!jumpFlag && onGround && boxScale.y <= 3.0f)
			{
				onGround = false;
				jumpFlag = true;
				// �㏸���̍X�V
				eUp = 1.25f;
			}
		}
		else if (minBoxZ - eScale.z < ePos.z && boxPos.z > eOldPos.z)
		{
			ePos.z = minBoxZ - eScale.z;
			hitFlag = true;
			if (!jumpFlag && onGround && boxScale.y <= 3.0f)
			{
				onGround = false;
				jumpFlag = true;
				// �㏸���̍X�V
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
			if (!jumpFlag && onGround && boxScale.y <= 3.0f)
			{
				onGround = false;
				jumpFlag = true;
				// �㏸���̍X�V
				eUp = 1.25f;
			}
		}
		else if (minBoxX - eScale.x < ePos.x && boxPos.x > eOldPos.x)
		{
			ePos.x = minBoxX - eScale.x;
			hitFlag = true;
			if (!jumpFlag && onGround && boxScale.y <= 3.0f)
			{
				onGround = false;
				jumpFlag = true;
				// �㏸���̍X�V
				eUp = 1.25f;
			}
		}
	}

	enemyObj->SetPosition(ePos);
	enemyObj->Update();

	return hitFlag;
}

bool Enemy::StageCollide(const XMFLOAT3 stagePos, const XMFLOAT3 stageScale)
{
	if (!eAlive)
	{
		return false;
	}

	// ����
	float maxX = stagePos.x + stageScale.x;
	float maxY = stagePos.y + stageScale.y;
	float maxZ = stagePos.z + stageScale.z;
	float minX = stagePos.x - stageScale.x;
	float minY = stagePos.y - stageScale.y;
	float minZ = stagePos.z - stageScale.z;

	if ((ePos.x <= maxX && ePos.x >= minX) &&
		(ePos.z <= maxZ && ePos.z >= minZ))
	{
		if (maxY + eScale.y > ePos.y && stagePos.y < eOldPos.y)
		{
			if (stagePos.y + eScale.y >= ePos.y)
			{
 				ePos.y = eOldPos.y;
			}
			onGround = true;
		}
	}

	enemyObj->SetPosition(ePos);
	enemyObj->Update();

	return onGround;
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

void Enemy::reactionDraw()
{
	exclamation_mark->Draw();
	question_mark->Draw();
}

void Enemy::Finalize()
{
	delete enemyModel;
	delete bulletModel;
}

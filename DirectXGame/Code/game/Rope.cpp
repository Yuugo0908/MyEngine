#include "Rope.h"

bool Rope::Initialize(Keyboard* keyboard)
{
	assert(keyboard);
	this->keyboard = keyboard;
	easing = new Easing;

	// ���f���̐���
	ropeModel = ropeModel->CreateFromObject("rope");
	ropeObj = Object3d::Create();
	ropeObj->SetModel(ropeModel);

	// �ʒu�A�X�P�[����ϐ��Ɋi�[
	ropeObj->SetScale({ 0.2f, 0.2f, 0.2f });
	rPos = ropeObj->GetPosition();
	rScale = ropeObj->GetScale();
	ropeObj->Update();

	return true;
}

void Rope::Update(XMFLOAT3& pPos, XMFLOAT3& ePos, const std::unique_ptr<Object3d>& object)
{
	if (throwCount < 30)
	{
		throwCount++;
	}

	if (!rFlag)
	{
		rPos = pPos + manageRopePos;
		rScale = manageRopeScale;
		ropeObj->SetPosition(rPos);
		ropeObj->SetScale(rScale);
		pEaseFlag = false;
		eEaseFlag = false;

		if (!rThrowFlag && !rBackFlag && keyboard->TriggerKey(DIK_J))
		{
			rThrowFlag = true;
			avoidTime = 0.0f;
		}
		if (throwCount == 30)
		{
			Throw(pPos, ePos, rPos, rScale, object);
		}

		ropeObj->Update();
		return;
	}

	// ���[�v�����Ă���ꍇ�A�G�������񂹂�
	if (keyboard->TriggerKey(DIK_K) && keyboard->PushKey(DIK_W))
	{
		pEaseFlag = true;
		avoidTime = 0.0f;
	}
	else if (keyboard->TriggerKey(DIK_K) && keyboard->PushKey(DIK_S))
	{
		eEaseFlag = true;
		avoidTime = 0.0f;
	}

	if (pEaseFlag)
	{
		easing->EaseInUpdate(pPos, ePos, pPos, pEaseFlag, avoidTime);
	}
	else if (eEaseFlag)
	{
		easing->EaseInUpdate(ePos, pPos, ePos, eEaseFlag, avoidTime);
	}

	{
		//�v���C���[�ƃG�l�~�[�̋���
		XMFLOAT3 length = { pPos.x - ePos.x, pPos.y - ePos.y, pPos.z - ePos.z };
		float len = GetLength(pPos, ePos);

		//�ő�l���傫���Ȃ�
		if (len > maxRope)
		{
			float wq = len / maxRope;
			len = maxRope;
			ePos = { pPos.x - length.x / wq, pPos.y - length.y / wq, pPos.z - length.z / wq };
		}

		// Y������̊p�x
		angleY = (float)atan2(pPos.x - ePos.x, pPos.z - ePos.z);
		vecXZ = sqrtf((pPos.x - ePos.x) * (pPos.x - ePos.x) + (pPos.z - ePos.z) * (pPos.z - ePos.z));
		// X������̊p�x
		angleX = (float)atan2(ePos.y - pPos.y, vecXZ);

		rPos = { (pPos.x + ePos.x) / 2, (pPos.y + ePos.y) / 2, (pPos.z + ePos.z) / 2 };
		rScale = { 0.2f, 0.2f , len / 2.0f };
		ropeObj->SetPosition(rPos);
		ropeObj->SetScale(rScale);
		ropeObj->SetRotation({ XMConvertToDegrees(angleX), XMConvertToDegrees(angleY), 0 });
		ropeObj->Update();
	}
}

void Rope::Throw(XMFLOAT3& pPos, XMFLOAT3& ePos, XMFLOAT3& rPos, XMFLOAT3& rScale, const std::unique_ptr<Object3d>& object)
{
	// �t���O��true����Ȃ��ꍇ�͏��������ă��^�[������
	if (!rThrowFlag && !rBackFlag)
	{
		manageRopePos = {};
		manageRopeScale = {};
		moveFlag = true;
		return;
	}

	moveFlag = false;
	// Y������̊p�x
	angleY = (float)atan2(pPos.x - ePos.x, pPos.z - ePos.z);
	vecXZ = sqrtf((pPos.x - ePos.x) * (pPos.x - ePos.x) + (pPos.z - ePos.z) * (pPos.z - ePos.z));
	// X������̊p�x
	angleX = (float)atan2(ePos.y - pPos.y, vecXZ);
	ropeObj->SetRotation({ XMConvertToDegrees(angleX), XMConvertToDegrees(angleY), 0 });

	XMVECTOR playerPos = { pPos.x, pPos.y, pPos.z, 1 };
	XMVECTOR enemyPos = { ePos.x, ePos.y, ePos.z, 1 };

	XMVECTOR subPlayerEnemy = XMVectorSubtract(enemyPos, playerPos);
	XMVECTOR NsubPlayerEnemy = XMVector3Normalize(subPlayerEnemy);

	XMFLOAT3 subPE = { NsubPlayerEnemy.m128_f32[0], NsubPlayerEnemy.m128_f32[1], NsubPlayerEnemy.m128_f32[2] };

	if (rThrowFlag)
	{
		manageRopePos.x += subPE.x;
		manageRopePos.y += subPE.y;
		manageRopePos.z += subPE.z;

		manageRopeScale.x += 0.02f;
		manageRopeScale.y += 0.02f;
		manageRopeScale.z += 0.7f;

		easing->EaseOutUpdate(rPos, manageRopePos, rPos, rThrowFlag, avoidTime);
		easing->EaseOutUpdate(rScale, manageRopeScale, rPos, rThrowFlag, avoidTime);

		Collision(object);

		if (avoidTime >= 1.0f)
		{
			avoidTime = 0.0f;
			rThrowFlag = false;
			rBackFlag = true;
		}
	}

	if (rBackFlag)
	{
		manageRopePos.x -= subPE.x;
		manageRopePos.y -= subPE.y;
		manageRopePos.z -= subPE.z;

		manageRopeScale.x -= 0.02f;
		manageRopeScale.y -= 0.02f;
		manageRopeScale.z -= 0.7f;

		easing->EaseOutUpdate(rPos, manageRopePos, rPos, rBackFlag, avoidTime);
		easing->EaseOutUpdate(rScale, manageRopeScale, rPos, rBackFlag, avoidTime);

		Collision(object);

		if (avoidTime >= 1.0f)
		{
			avoidTime = 0.0f;
			rThrowFlag = false;
			rBackFlag = false;
			moveFlag = true;
			throwCount = 0;
		}
	}
}

void Rope::Collision(const std::unique_ptr<Object3d>& object)
{
	if (!Collision::CollisionObject(object, ropeObj))
	{
		return;
	}

	manageRopePos = {};
	manageRopeScale = {};
	avoidTime = 0.0f;
	rThrowFlag = false;
	rBackFlag = false;
	moveFlag = true;
	if (keyboard->PushKey(DIK_S))
	{
		eEaseFlag = true;
	}
	else
	{
		pEaseFlag = true;
	}
	throwCount = 0;
	rFlag = true;
}

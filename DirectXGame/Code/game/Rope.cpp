#include "Rope.h"

bool Rope::Initialize(Keyboard* keyboard, Mouse* mouse)
{
	if (ropeModel == NULL)
	{
		assert(keyboard);
		assert(mouse);
		this->keyboard = keyboard;
		this->mouse = mouse;

		easing = new Easing;

		// ���f���̐���
		ropeModel = ropeModel->CreateFromObject("rope");
		ropeObj = Object3d::Create();
		ropeObj->SetModel(ropeModel);
	}

	rFlag = false; // �ڐG�t���O
	rThrowFlag = false; // ���[�v���΂�
	rBackFlag = false; // ���[�v��߂�

	// �ʒu�A�X�P�[����ϐ��Ɋi�[
	ropeObj->SetScale({ 0.0f, 0.0f, 0.0f });
	rPos = ropeObj->GetPosition();
	rScale = ropeObj->GetScale();
	ropeObj->Update();

	ray.start = XMVectorSet(rPos.x, rPos.y, rPos.z, 1);
	ray.dir = XMVectorSet(0, 0, 1, 0);

	return true;
}

void Rope::Update(XMFLOAT3& pPos)
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

		if (!rThrowFlag && !rBackFlag && mouse->TriggerMouseLeft())
		{
			rPos = pPos;
			moveFlag = false;
			rThrowFlag = true;
			avoidTime = 0.0f;
		}

		ropeObj->Update();
		return;
	}

	objLength = GetLength(pPos, endPos);
	{
		XMFLOAT3 length = { pPos.x - endPos.x, pPos.y - endPos.y, pPos.z - endPos.z };

		angleY = (float)atan2(pPos.x - endPos.x, pPos.z - endPos.z);
		vecXZ = sqrtf((pPos.x - endPos.x) * (pPos.x - endPos.x) + (pPos.z - endPos.z) * (pPos.z - endPos.z));
		angleX = (float)atan2(endPos.y - pPos.y, vecXZ);

		rPos = { (pPos.x + endPos.x) / 2, (pPos.y + endPos.y) / 2, (pPos.z + endPos.z) / 2 };
		rScale = { 0.2f, 0.2f , objLength / 2.0f };
		ropeObj->SetPosition(rPos);
		ropeObj->SetScale(rScale);
		ropeObj->SetRotation({ XMConvertToDegrees(angleX), XMConvertToDegrees(angleY), 0 });
		ropeObj->Update();
	}


	if (pEaseFlag)
	{
		easing->EaseInUpdate(pPos, endPos, pPos, pEaseFlag, avoidTime);
	}
	//else if (eEaseFlag)
	//{
	//	easing->EaseInUpdate(ePos, pPos, ePos, eEaseFlag, avoidTime);
	//}
}

void Rope::Throw(XMFLOAT3& pPos, XMFLOAT3 ePos, float length)
{
	// �t���O��true����Ȃ��ꍇ�͏��������ă��^�[������
	if (!rThrowFlag && !rBackFlag)
	{
		manageRopePos = {};
		manageRopeScale = {};
		moveFlag = true;
		return;
	}

	if (length < maxRope)
	{
		rRotFlag = true;
	}

	XMVECTOR playerPos = { pPos.x, pPos.y, pPos.z, 1 };
	XMVECTOR enemyPos = { ePos.x, ePos.y, ePos.z, 1 };

	XMVECTOR subPlayerEnemy = XMVectorSubtract(enemyPos, playerPos);
	XMVECTOR NsubPlayerEnemy = XMVector3Normalize(subPlayerEnemy);

	XMFLOAT3 subPE = { NsubPlayerEnemy.m128_f32[0], NsubPlayerEnemy.m128_f32[1], NsubPlayerEnemy.m128_f32[2] };

	XMFLOAT3 cSpeed = Camera::GetInstance()->CameraTrack(pPos);
	float cRot = Camera::GetInstance()->CameraRot(pPos);

	if (rThrowFlag)
	{
		if (rRotFlag)
		{
			// Y������̊p�x
			angleY = (float)atan2(pPos.x - ePos.x, pPos.z - ePos.z);
			vecXZ = sqrtf((pPos.x - ePos.x) * (pPos.x - ePos.x) + (pPos.z - ePos.z) * (pPos.z - ePos.z));
			// X������̊p�x
			angleX = (float)atan2(ePos.y - pPos.y, vecXZ);
			ropeObj->SetRotation({ XMConvertToDegrees(angleX), XMConvertToDegrees(angleY), 0 });
			ray.dir = XMVectorSet(XMConvertToDegrees(angleX), XMConvertToDegrees(angleY), 0, 0);
			manageRopePos.x += subPE.x;
			manageRopePos.y += subPE.y;
			manageRopePos.z += subPE.z;
		}
		else
		{
			ropeObj->SetRotation({ 0, XMConvertToDegrees(cRot), 0 });
			manageRopePos.x += cSpeed.x;
			manageRopePos.z += cSpeed.z;
			rRotFlag = false;
		}

		manageRopeScale.x += 0.02f;
		manageRopeScale.y += 0.02f;
		manageRopeScale.z += 0.7f;

		avoidTime += 0.1f;

		if (avoidTime >= 1.0f)
		{
			avoidTime = 0.0f;
			rThrowFlag = false;
			rBackFlag = true;
		}
	}

	if (rBackFlag)
	{
		if (rRotFlag)
		{
			// Y������̊p�x
			angleY = (float)atan2(pPos.x - ePos.x, pPos.z - ePos.z);
			vecXZ = sqrtf((pPos.x - ePos.x) * (pPos.x - ePos.x) + (pPos.z - ePos.z) * (pPos.z - ePos.z));
			// X������̊p�x
			angleX = (float)atan2(ePos.y - pPos.y, vecXZ);
			ropeObj->SetRotation({ XMConvertToDegrees(angleX), XMConvertToDegrees(angleY), 0 });
			ray.dir = XMVectorSet(XMConvertToDegrees(angleX), XMConvertToDegrees(angleY), 0, 0);
			manageRopePos.x -= subPE.x;
			manageRopePos.y -= subPE.y;
			manageRopePos.z -= subPE.z;
		}
		else
		{
			manageRopePos.x -= cSpeed.x;
			manageRopePos.z -= cSpeed.z;
		}

		manageRopeScale.x -= 0.02f;
		manageRopeScale.y -= 0.02f;
		manageRopeScale.z -= 0.7f;


		avoidTime += 0.1f;

		if (avoidTime >= 1.0f)
		{
			avoidTime = 0.0f;
			rThrowFlag = false;
			rBackFlag = false;
			rRotFlag = false;
			moveFlag = true;
			throwCount = 0;
			manageRopePos = {};
			manageRopeScale = {};
		}
	}
}

void Rope::Collision(XMFLOAT3& startPos, XMFLOAT3& endPos)
{
	manageRopePos = {};
	manageRopeScale = {};
	avoidTime = 0.0f;
	rThrowFlag = false;
	rBackFlag = false;
	rRotFlag = false;
	this->startPos = startPos;
	this->endPos = endPos;
	rFlag = true;
	pEaseFlag = true;
	throwCount = 0;
}

void Rope::Reset()
{
	avoidTime = 0.0f;
	rThrowFlag = false;
	rBackFlag = false;

	// �ϐ�
	rPos = {};
	rScale = {};
	manageRopePos = {}; // ���[�v�ʒu�Ǘ��p
	manageRopeScale = { 0.0f, 0.0f, 0.0f }; // ���[�v�X�P�[���Ǘ��p
	angleX = 0.0f; // X��
	angleY = 0.0f; // Y��
	vecXZ = 0.0f; // XZ���ʏ�̃x�N�g��
	objLength = 0.0f;
	rFlag = false; // �ڐG�t���O
	throwCount = 0;

	// �ːi�p
	startPos = {}; // �J�n�ʒu
	endPos = {}; // �I�_�ʒu
	pEaseFlag = false;
	eEaseFlag = false;

	// �ړ��Ǘ��t���O
	moveFlag = false; // ���@���ړ����Ă��邩
}

void Rope::CircularMotion(XMFLOAT3& pos, const XMFLOAT3 centerPos, const float r, int& angle, const int add)
{
	angle += add;

	// �~�^���̏���
	pos.x = (cosf(3.14f / 180.0f * angle) * r) + centerPos.x;
	pos.y = (sinf(3.14f / 180.0f * angle) * r) + centerPos.y;
	pos.z = (tanf(3.14f / 180.0f * angle) * r) + centerPos.z;
}

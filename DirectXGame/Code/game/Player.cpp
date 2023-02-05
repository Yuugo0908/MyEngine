#include "Player.h"

bool Player::Initialize(Keyboard* keyboard, Mouse* mouse)
{
	assert(keyboard);
	assert(mouse);
	this->keyboard = keyboard;
	this->mouse = mouse;

	// ���f���̐���
	playerModel = playerModel->CreateFromObject("sphere");
	playerObj = Object3d::Create();
	playerObj->SetModel(playerModel);

	// �ʒu��ϐ��Ɋi�[
	pPos = { 0.0f, 10.0f, -10.0f };
	pPosOld = pPos;
	pScale = { 0.8f,0.8f,0.8f };

	playerObj->SetPosition(pPos);
	playerObj->SetScale(pScale);
	playerObj->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	playerObj->Update();

	return true;
}

void Player::Update(bool rFlag, bool moveFlag)
{
	cameraTrack = camera->CameraTrack(pPos);
	cameraRot = camera->CameraRot(pPos);
	cameraTrack = cameraTrack * pSpeed;

	pPosOld = playerObj->GetPosition();
	playerObj->SetPosition(pPos);
	playerObj->SetScale(pScale);
	playerObj->SetRotation({ 0, XMConvertToDegrees(cameraRot), 0 });

	if (moveFlag)
	{
		Jump();
		Rush(rFlag);

		// �ړ�
		rate = 1.0f;
		// �ړ��ʂ̔{���v�Z
		if (keyboard->PushKey(DIK_A) || keyboard->PushKey(DIK_D))
		{
			if (keyboard->PushKey(DIK_W) || keyboard->PushKey(DIK_S))
			{
				rate = sqrtf(1.0f / 2.0f);
			}
		}

		cameraTrack = cameraTrack * pAcc * rate;

		if (keyboard->PushKey(DIK_D))
		{
			pPos.x += cameraTrack.z;
			pPos.z -= cameraTrack.x;
		}
		else if (keyboard->PushKey(DIK_A))
		{
			pPos.x -= cameraTrack.z;
			pPos.z += cameraTrack.x;
		}
		if (keyboard->PushKey(DIK_W))
		{
			pPos.x += cameraTrack.x;
			pPos.z += cameraTrack.z;
		}
		else if (keyboard->PushKey(DIK_S))
		{
			pPos.x -= cameraTrack.x;
			pPos.z -= cameraTrack.z;
		}
	}

	playerObj->Update();
}

void Player::Attack(XMFLOAT3 targetPos, bool& flag, float& avoidTime)
{
	// �t���O��true����Ȃ��ꍇ�̓��^�[������
	if (!flag)
	{
		avoidTime = 0.0f;
		return;
	}

	if (avoidTime < 1.0f)
	{
		avoidTime += 0.08f;
	}

	pPos = Easing::GetInstance()->easeIn(pPos, targetPos, avoidTime);
	playerObj->Update();
}

void Player::Rush(bool rFlag)
{
	if (avoidFlag)
	{
		pAcc -= pGra;
	}
	// ���@�̓ːi
	else if (!rFlag && mouse->TriggerMouseRight())
	{
		avoidFlag = true;
		// �����͂̍X�V
		pAcc = 2.2f;
	}

	if (!avoidFlag)
	{
		pAcc = 1.0f;
	}
	else if (pAcc <= 1.0f)
	{
		avoidFlag = false;
	}
}

void Player::Jump()
{
	if (!jumpFlag && keyboard->TriggerKey(DIK_SPACE))
	{
		onGround = false;
		jumpFlag = true;
		// �㏸���̍X�V
		pVal = 1.25f;
	}

	// �W�����v
	if (jumpFlag)
	{
		pVal -= pGra;
		pPos.y += pVal;
		if (onGround)
		{
			jumpFlag = false;
			pVal = 0.0f;
		}
	}
	// �d��
	else
	{
		pDown = -0.75f;
		pPos.y += pDown;
		if (onGround)
		{
			pDown = 0.0f;
		}
	}
}

void Player::Reset()
{
	pPos = { 0.0f, 10.0f, 0.0f };//���W
	pPosOld = { 0.0f, 10.0f, 0.0f };
	pRot = { 0.0f, 0.0f, 0.0f };//��]

	playerObj->SetPosition(pPos);
	playerObj->SetScale({ 0.8f,0.8f,0.8f });
	playerObj->SetRotation(pRot);
	playerObj->Update();

	pSpeed = 0.35f;
	onGround = false;//���R�����̃t���O
	jumpFlag = false;
	moveFlag = false;//�ړ��Ǘ��t���O
	pMove = 0.0f;//�ړ���
	pAcc = 0.2f;//����
	pVal = 0.2f;//���x
	pDown = 0.0f;
	pGra = 0.1f;//�d��
	rate = 1.0f; // �΂߈ړ����̐���

	// �ːi�p
	avoidFlag = false; // ����J�n�t���O
	avoidCount = 0;

	// �J���������擾�p
	cameraTrack = {};
	cameraRot = 0.0f;
}

bool Player::MapCollide(XMFLOAT3 boxPos, XMFLOAT3 boxScale)
{
	//�t���O
	bool hitFlag = false;

	// ����
	float maxBoxX = boxPos.x + boxScale.x;
	float minBoxX = boxPos.x - boxScale.x;
	float maxBoxY = boxPos.y + boxScale.y;
	float minBoxY = boxPos.y - boxScale.y;
	float maxBoxZ = boxPos.z + boxScale.z;
	float minBoxZ = boxPos.z - boxScale.z;

	if ((pPos.x <= maxBoxX && pPos.x >= minBoxX) &&
		(pPos.z <= maxBoxZ && pPos.z >= minBoxZ))
	{
		if (maxBoxY + pScale.y > pPos.y && boxPos.y < pPosOld.y)
		{
			hitFlag = true;
			if (maxBoxY + pScale.y >= pPos.y)
			{
				pPos.y = pPosOld.y;
			}
			onGround = true;
		}
		else if (minBoxY - pScale.y < pPos.y && boxPos.y > pPosOld.y)
		{
			hitFlag = true;
			if (minBoxY - pScale.y <= pPos.y)
			{
				pPos.y = pPosOld.y;
			}
		}
	}

	if ((pPos.x <= maxBoxX && pPos.x >= minBoxX) &&
		(pPos.y <= maxBoxY && pPos.y >= minBoxY))
	{
		if (maxBoxZ + pScale.z > pPos.z && boxPos.z < pPosOld.z)
		{
			pPos.z = maxBoxZ + pScale.z;
			hitFlag = true;
			avoidFlag = false;
		}
		else if (minBoxZ - pScale.z < pPos.z && boxPos.z > pPosOld.z)
		{
			pPos.z = minBoxZ - pScale.z;
			hitFlag = true;
			avoidFlag = false;
		}
	}

	if ((pPos.z <= maxBoxZ && pPos.z >= minBoxZ) &&
		(pPos.y <= maxBoxY && pPos.y >= minBoxY))
	{
		if (maxBoxX + pScale.x > pPos.x && boxPos.x < pPosOld.x)
		{
			pPos.x = maxBoxX + pScale.x;
			hitFlag = true;
			avoidFlag = false;
		}
		else if (minBoxX - pScale.x < pPos.x && boxPos.x > pPosOld.x)
		{
			pPos.x = minBoxX - pScale.x;
			hitFlag = true;
			avoidFlag = false;
		}
	}

	playerObj->SetPosition(pPos);
	playerObj->Update();

	return hitFlag;
}

bool Player::StageCollide(XMFLOAT3 stagePos, XMFLOAT3 stageScale)
{
	// ����
	float maxX = stagePos.x + stageScale.x;
	float maxY = stagePos.y + stageScale.y;
	float maxZ = stagePos.z + stageScale.z;
	float minX = stagePos.x - stageScale.x;
	float minY = stagePos.y - stageScale.y;
	float minZ = stagePos.z - stageScale.z;

	bool hitFlag = false;

	if ((pPos.x < maxX && pPos.x > minX) &&
		(pPos.z < maxZ && pPos.z > minZ))
	{
		if (maxY + pScale.y > pPos.y && stagePos.y < pPosOld.y)
		{
			if (stagePos.y + pScale.y >= pPos.y)
			{
				pPos.y = pPosOld.y;
			}
			hitFlag = true;
		}
	}

	playerObj->SetPosition(pPos);
	playerObj->Update();

	return hitFlag;
}

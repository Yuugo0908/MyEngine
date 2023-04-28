#include "Player.h"

bool Player::Initialize(const XMFLOAT3 pos, const XMFLOAT3 scale)
{
	// ���f���̐���
	playerModel = playerModel->CreateFromObject("player");
	playerObj = Object3d::Create();
	playerObj->SetModel(playerModel);

	pPos = pos;
	pScale = scale;
	reSpawnPos = pos;

	playerObj->SetPosition(pPos);
	playerObj->SetScale(pScale);
	playerObj->Update();

	return true;
}

void Player::Update()
{
	bool rFlag = Rope::GetInstance()->GetrFlag();
	bool throwFlag = Rope::GetInstance()->GetThrowFlag();

	// �J�����������Ă�������𒲂ׂ�
	cameraTrack = camera->CameraTrack(pPos);

	if (!rushFlag && (keyboard->ReleaseKey(DIK_D) || keyboard->ReleaseKey(DIK_A) || keyboard->ReleaseKey(DIK_W) || keyboard->ReleaseKey(DIK_S)))
	{
		inertiaSaveMove = pPos - pPosOld;
	}

	// �_���[�W���󂯂���̏���
	if (damageInterval > 0)
	{
		damageInterval--;
		float alpha = 0.5f;
		playerObj->SetColor({ 1.0f, 1.0f, 1.0f, alpha });
	}
	else
	{
		playerObj->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	}

	if (!throwFlag && !rFlag)
	{
		// �΂߈ړ�
		rate = 1.0f;

		// �ړ��l�͈̔͂�0.0f�`1.0f�ɂ���
		pMove = (std::min)((std::max)(pMove, 0.0f), 1.0f);

		// �ړ��ʂ̔{���v�Z
		if (controller->GetPadState(Controller::State::LEFT_L_STICK, Controller::Type::NONE) || controller->GetPadState(Controller::State::LEFT_R_STICK, Controller::Type::NONE))
		{
			if (controller->GetPadState(Controller::State::LEFT_U_STICK, Controller::Type::NONE) || controller->GetPadState(Controller::State::LEFT_D_STICK, Controller::Type::NONE))
			{
				rate = sqrtf(1.0f / 2.0f);
			}
		}
		if (keyboard->PushKey(DIK_A) || keyboard->PushKey(DIK_D))
		{
			if (keyboard->PushKey(DIK_W) || keyboard->PushKey(DIK_S))
			{
				rate = sqrtf(1.0f / 2.0f);
			}
		}

		// ���K�������J���������ɉ����x�A�ړ��ʁA�΂߈ړ����̕␳��������
		cameraTrack.x = cameraTrack.x * pSpeed * pAcc * rate * pMove;
		cameraTrack.z = cameraTrack.z * pSpeed * pAcc * rate * pMove;

		// �W�����v
		Jump();
		// ���
		Avoid();

		if (controller->GetPadState(Controller::State::LEFT_R_STICK, Controller::Type::NONE) || keyboard->PushKey(DIK_D))
		{
			pPos.x += cameraTrack.z;
			pPos.z -= cameraTrack.x;
			moveFlag = true;
		}
		else if (controller->GetPadState(Controller::State::LEFT_L_STICK, Controller::Type::NONE) || keyboard->PushKey(DIK_A))
		{
			pPos.x -= cameraTrack.z;
			pPos.z += cameraTrack.x;
			moveFlag = true;
		}

		if (controller->GetPadState(Controller::State::LEFT_U_STICK, Controller::Type::NONE) || keyboard->PushKey(DIK_W))
		{
			pPos.x += cameraTrack.x;
			pPos.z += cameraTrack.z;
			moveFlag = true;
		}
		else if (controller->GetPadState(Controller::State::LEFT_D_STICK, Controller::Type::NONE) || keyboard->PushKey(DIK_S))
		{
			pPos.x -= cameraTrack.x;
			pPos.z -= cameraTrack.z;
			moveFlag = true;
		}

		if (!keyboard->PushKey(DIK_D) && !keyboard->PushKey(DIK_A) && !keyboard->PushKey(DIK_W) && !keyboard->PushKey(DIK_S))
		{
			moveFlag = false;
		}

		if (moveFlag)
		{
			pMove += 0.1f;
		}
		else if (!avoidFlag)
		{
			pMove -= 0.1f;
			pPos.x += inertiaSaveMove.x * pMove;
			pPos.z += inertiaSaveMove.z * pMove;

			if (pMove <= 0.0f)
			{
				inertiaSaveMove = {};
			}
		}
	}

	// �O�t���[���̃v���C���[���W���擾
	pPosOld = playerObj->GetPosition();
	// �v���C���[�̍��W�𔽉f
	playerObj->SetPosition(pPos);
	playerObj->SetScale(pScale);
	playerObj->Update();
}

void Player::Rush(XMFLOAT3 targetPos, bool& flag, float& avoidTime)
{
	// �t���O��true����Ȃ��ꍇ�̓��^�[������
	if (!flag)
	{
		avoidTime = 0.0f;
		rushFlag = false;
		return;
	}

	rushFlag = true;
	if (avoidTime < 1.0f)
	{
		avoidTime += 0.1f;
	}

	pPos = Easing::easeIn(pPos, targetPos, avoidTime);
	playerObj->Update();
}

void Player::Avoid()
{
	if (avoidFlag)
	{
		pAcc -= pGra;
		pMove = 1.0f;
	}
	// ���@�̓ːi
	else if (mouse->TriggerMouseRight() || controller->GetPadState(Controller::State::RT, Controller::Type::NONE))
	{
		avoidFlag = true;
		// �����͂̍X�V
		pAcc = 3.0f;
	}

	// �ړ��L�[�ƃW�����v�L�[�̓��͏��
	if (!moveFlag && avoidFlag)
	{
		pPos.x -= cameraTrack.x;
		pPos.z -= cameraTrack.z;
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
	if (!jumpFlag && (keyboard->TriggerKey(DIK_SPACE) || controller->GetPadState(Controller::State::A, Controller::Type::TRIGGER)))
	{
		onGround = false;
		jumpFlag = true;
		moveFlag = true;
		// �㏸���̍X�V
		pVel = 1.25f;
	}

	// �W�����v
	if (jumpFlag)
	{
		pVel -= pGra;
		pPos.y += pVel;
		pPos.x += inertiaSaveJump.x * 0.15f;
		pPos.z += inertiaSaveJump.z * 0.15f;

		if (onGround)
		{
			jumpFlag = false;
			moveFlag = false;
			pVel = 0.0f;
			inertiaSaveJump = {};
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

bool Player::Damage(const std::unique_ptr<Object3d>& object)
{
	if (damageInterval == 0 && Collision::CollisionObject(object, playerObj))
	{
		damageInterval = 120;
		return true;
	}
	return false;
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
	pMove = 0.0f;//�ړ���
	pAcc = 0.2f;//����
	pVel = 0.2f;//���x
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

void Player::ReSpawn()
{
	pPos = reSpawnPos;
	playerObj->SetPosition(pPos);
	playerObj->SetScale(pScale);
	playerObj->Update();
}

void Player::TrackRot(const XMFLOAT3& startPos, const XMFLOAT3& endPos)
{
	XMVECTOR sPos = { startPos.x, startPos.y, startPos.z, 0 };
	XMVECTOR ePos = { endPos.x, endPos.y, endPos.z, 0 };

	XMVECTOR subPos = XMVectorSubtract(sPos, ePos);

	float angle = (float)atan2(subPos.m128_f32[0], subPos.m128_f32[2]);

	playerObj->SetRotation({ 0, XMConvertToDegrees(angle), 0 });
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

bool Player::StageCollide(XMFLOAT3 stagePos, XMFLOAT3 stageScale, bool& reverseFlag)
{
	//�t���O
	bool hitFlag = false;

	// ����
	float maxX = stagePos.x + stageScale.x;
	float maxY = stagePos.y + stageScale.y;
	float maxZ = stagePos.z + stageScale.z;
	float minX = stagePos.x - stageScale.x;
	float minY = stagePos.y - stageScale.y;
	float minZ = stagePos.z - stageScale.z;

	if ((pPos.x <= maxX && pPos.x >= minX) &&
		(pPos.z <= maxZ && pPos.z >= minZ))
	{
		if (maxY + pScale.y > pPos.y && stagePos.y < pPosOld.y)
		{
			hitFlag = true;
			if (stagePos.y + pScale.y > pPos.y)
			{
				pPos.y = pPosOld.y;
			}
			onGround = true;
		}

		else if (minY - pScale.y < pPos.y && stagePos.y > pPosOld.y)
		{
			hitFlag = true;
			if (stagePos.y - pScale.y <= pPos.y)
			{
				pPos.y = pPosOld.y;
			}
			reverseFlag = true;
		}
	}

	playerObj->SetPosition(pPos);
	playerObj->Update();

	return hitFlag;
}

bool Player::PoleCollide(XMFLOAT3 polePos, XMFLOAT3 poleScale)
{
	//�t���O
	bool hitFlag = false;

	// ����
	float maxBoxX = polePos.x + poleScale.x;
	float minBoxX = polePos.x - poleScale.x;
	float maxBoxY = polePos.y + poleScale.y;
	float minBoxY = polePos.y - poleScale.y;
	float maxBoxZ = polePos.z + poleScale.z;
	float minBoxZ = polePos.z - poleScale.z;

	if ((pPos.x <= maxBoxX && pPos.x >= minBoxX) &&
		(pPos.z <= maxBoxZ && pPos.z >= minBoxZ))
	{
		if (maxBoxY + pScale.y > pPos.y && polePos.y < pPosOld.y)
		{
			hitFlag = true;
		}
		else if (minBoxY - pScale.y < pPos.y && polePos.y > pPosOld.y)
		{
			hitFlag = true;
		}
	}

	else if ((pPos.x <= maxBoxX && pPos.x >= minBoxX) &&
		(pPos.y <= maxBoxY && pPos.y >= minBoxY))
	{
		if (maxBoxZ + pScale.z > pPos.z && polePos.z < pPosOld.z)
		{
			hitFlag = true;
		}
		else if (minBoxZ - pScale.z < pPos.z && polePos.z > pPosOld.z)
		{
			hitFlag = true;
		}
	}

	else if ((pPos.z <= maxBoxZ && pPos.z >= minBoxZ) &&
		(pPos.y <= maxBoxY && pPos.y >= minBoxY))
	{
		if (maxBoxX + pScale.x > pPos.x && polePos.x < pPosOld.x)
		{
			hitFlag = true;
		}
		else if (minBoxX - pScale.x < pPos.x && polePos.x > pPosOld.x)
		{
			hitFlag = true;
		}
	}

	if (hitFlag)
	{
		inertiaSaveJump = pPos - pPosOld;
		onGround = false;
		jumpFlag = true;
		// �㏸���̍X�V
		pVel = 1.25f;
	}

	playerObj->SetPosition(pPos);
	playerObj->Update();

	return hitFlag;
}

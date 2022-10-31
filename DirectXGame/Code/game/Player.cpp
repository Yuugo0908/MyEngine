#include "Player.h"

bool Player::Initialize(Keyboard* keyboard)
{
	assert(keyboard);
	this->keyboard = keyboard;

	rope = new Rope;
	easing = new Easing;

	// ���f���̐���
	playerModel = playerModel->CreateFromObject("cube");
	playerObj = Object3d::Create();
	playerObj->SetModel(playerModel);

	// �ʒu��ϐ��Ɋi�[
	playerObj->SetPosition({ 0.0f, 5.0f, -5.0f });
	playerObj->SetScale({ 0.8f,0.8f,0.8f });
	pPos = playerObj->GetPosition();
	playerObj->Update();

	return true;
}

void Player::Finalize()
{
	delete playerModel;
}

void Player::Update(bool moveFlag)
{
	rFlag = rope->GetrFlag();
	pPos = playerObj->GetPosition();

	Jump();
	Rush();

	// �ړ�
	if (moveFlag)
	{
		rate = 1.0f;
		// �ړ��ʂ̔{���v�Z
		if (keyboard->PushKey(DIK_A) || keyboard->PushKey(DIK_D))
		{
			if (keyboard->PushKey(DIK_W) || keyboard->PushKey(DIK_S))
			{
				rate = 0.7f;
			}
		}

		pMove = pAcc * rate;

		if (keyboard->PushKey(DIK_D))
		{
			pPos.x += pMove;
		}
		else if (keyboard->PushKey(DIK_A))
		{
			pPos.x -= pMove;
		}
		if (keyboard->PushKey(DIK_W))
		{
			pPos.z += pMove;
		}
		else if (keyboard->PushKey(DIK_S))
		{
			pPos.z -= pMove;
		}
	}

	playerObj->SetPosition(pPos);
	playerObj->Update();
}

void Player::Draw()
{
	playerObj->Draw();
}

void Player::Rush()
{
	if (rushCount <= 30)
	{
		rushCount++;
	}

	// ���@�̓ːi
	pMove = avoidMove * rate;
	if (!rFlag && keyboard->TriggerKey(DIK_K) && rushCount >= 30)
	{
		easeFlag = true;
		startPos = pPos;
		endPos = pPos;
		if (keyboard->PushKey(DIK_D))
		{
			endPos.x += pMove;
		}
		else if (keyboard->PushKey(DIK_A))
		{
			endPos.x -= pMove;
		}
		if (keyboard->PushKey(DIK_W))
		{
			endPos.z += pMove;
		}
		else if (keyboard->PushKey(DIK_S))
		{
			endPos.z -= pMove;
		}
	}
	easing->EaseInUpdate(startPos, endPos, pPos, easeFlag, avoidTime);
}

void Player::Jump()
{
	if (pPos.y > 0.0f)
	{
		jumpFlag = true;
	}
	else
	{
		pPos.y = 0.0f;
		pVal = 0.0f;
		jumpFlag = false;
	}

	// �W�����v
	if (keyboard->TriggerKey(DIK_SPACE) && !jumpFlag)
	{
		jumpFlag = true;
		// �㏸���̍X�V
		pVal = 1.25f;
	}
	if (jumpFlag && !easeFlag)
	{
		pVal -= pGra;
		pPos.y += pVal;
	}
}

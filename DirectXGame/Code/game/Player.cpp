#include "Player.h"

bool Player::Initialize(Keyboard* keyboard)
{
	assert(keyboard);
	this->keyboard = keyboard;

	rope = new Rope;
	easing = new Easing;

	// モデルの生成
	playerModel = playerModel->CreateFromObject("cube");
	playerObj = Object3d::Create();
	playerObj->SetModel(playerModel);

	// 位置を変数に格納
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

	// 移動
	if (moveFlag)
	{
		rate = 1.0f;
		// 移動量の倍数計算
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

	// 自機の突進
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

	// ジャンプ
	if (keyboard->TriggerKey(DIK_SPACE) && !jumpFlag)
	{
		jumpFlag = true;
		// 上昇率の更新
		pVal = 1.25f;
	}
	if (jumpFlag && !easeFlag)
	{
		pVal -= pGra;
		pPos.y += pVal;
	}
}

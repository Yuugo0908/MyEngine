#include "Player.h"

bool Player::Initialize(Keyboard* keyboard, Mouse* mouse)
{
	assert(keyboard);
	assert(mouse);
	this->keyboard = keyboard;
	this->mouse = mouse;

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

void Player::Update(bool rFlag, bool moveFlag)
{
	Jump();
	Rush(rFlag);
	playerObj->SetPosition(pPos);

	cameraTrack = camera->CameraTrack(pPos);
	cameraTrack = cameraTrack * pSpeed;

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
		cameraTrack = cameraTrack * rate;

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

	//pPos = playerObj->GetPosition();
	playerObj->Update();
}

void Player::Rush(bool rFlag)
{
	if (rushCount <= 30)
	{
		rushCount++;
	}

	// 自機の突進

	if (!rFlag && mouse->TriggerMouseRight() && rushCount >= 30)
	{
		easeFlag = true;
		startPos = pPos;
		endPos = pPos;
		if (keyboard->PushKey(DIK_D))
		{
			endPos.x += cameraTrack.z * avoidMove;
			endPos.z -= cameraTrack.x * avoidMove;
		}
		else if (keyboard->PushKey(DIK_A))
		{
			endPos.x -= cameraTrack.z * avoidMove;
			endPos.z += cameraTrack.x * avoidMove;
		}
		if (keyboard->PushKey(DIK_W))
		{
			endPos.x += cameraTrack.x * avoidMove;
			endPos.z += cameraTrack.z * avoidMove;
		}
		else if (keyboard->PushKey(DIK_S))
		{
			endPos.x -= cameraTrack.x * avoidMove;
			endPos.z -= cameraTrack.z * avoidMove;
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

#include "Player.h"

bool Player::Initialize(Keyboard* keyboard, Mouse* mouse)
{
	assert(keyboard);
	assert(mouse);
	this->keyboard = keyboard;
	this->mouse = mouse;

	rope = new Rope;

	// モデルの生成
	playerModel = playerModel->CreateFromObject("cube");
	playerObj = Object3d::Create();
	playerObj->SetModel(playerModel);

	// 位置を変数に格納
	playerObj->SetPosition({ 50.0f, 0.0f, 50.0f });
	playerObj->SetScale({ 0.8f,0.8f,0.8f });
	pPos = playerObj->GetPosition();
	pScale = playerObj->GetScale();
	playerObj->Update();

	return true;
}

void Player::Update(bool rFlag, bool moveFlag)
{
	cameraTrack = camera->CameraTrack(pPos);
	cameraRot = camera->CameraRot(pPos);
	cameraTrack = cameraTrack * pSpeed;

	playerObj->SetPosition(pPos);
	playerObj->SetScale(pScale);
	playerObj->SetRotation({ 0, XMConvertToDegrees(cameraRot), 0 });

	if (moveFlag)
	{
		Jump();
		Rush(rFlag);

		// 移動
		rate = 1.0f;
		// 移動量の倍数計算
		if (keyboard->PushKey(DIK_A) || keyboard->PushKey(DIK_D))
		{
			if (keyboard->PushKey(DIK_W) || keyboard->PushKey(DIK_S))
			{
				rate = 0.7f;
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

	//pPos = playerObj->GetPosition();
	playerObj->Update();
}

void Player::Rush(bool rFlag)
{
	if (avoidFlag)
	{
		pAcc -= pGra;
	}
	// 自機の突進
	else if (!rFlag && mouse->TriggerMouseRight())
	{
		avoidFlag = true;
		// 加速力の更新
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
		// 上昇率の更新
		pVal = 1.25f;
	}

	// ジャンプ
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
	// 重力
	else
	{
		pDown -= 0.5f;
		pPos.y += pDown;
		if (onGround)
		{
			pDown = 0.0f;
		}
	}
}

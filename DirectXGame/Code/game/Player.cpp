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
	pPos = { 0.0f, 10.0f, 0.0f };

	playerObj->SetPosition(pPos);
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
		pDown = -0.75f;
		pPos.y += pDown;
		if (onGround)
		{
			pDown = 0.0f;
		}
	}
}

bool Player::MapCollide(XMFLOAT3 boxPos, XMFLOAT3 boxRadius, XMFLOAT3& pos, XMFLOAT3 radius, int mapNumber, const XMFLOAT3 oldPos)
{
	//フラグ
	bool hitFlag = false;

	// 判定
	float maxBoxX = boxPos.x + boxRadius.x;
	float minBoxX = boxPos.x - boxRadius.x;
	float maxBoxY = boxPos.y + boxRadius.y;
	float minBoxY = boxPos.y - boxRadius.y;
	float maxBoxZ = boxPos.z + boxRadius.z;
	float minBoxZ = boxPos.z - boxRadius.z;

	if ((pos.x <= maxBoxX && pos.x >= minBoxX) &&
		(pos.z <= maxBoxZ && pos.z >= minBoxZ))
	{
		if (maxBoxY + radius.y > pos.y && boxPos.y < oldPos.y)
		{
			//pos.y = maxBoxY + radius.y;
			hitFlag = true;
			if (maxBoxY + radius.y >= pos.y)
			{
				pos.y = oldPos.y;
			}
			onGround = true;
		}
		else if (minBoxY - radius.y < pos.y && boxPos.y > oldPos.y)
		{
			pos.y = minBoxY - radius.y;
			hitFlag = true;
			if (minBoxY - radius.y <= pos.y)
			{
				pos.y = oldPos.y;
			}
		}
	}

	if ((pos.x <= maxBoxX && pos.x >= minBoxX) &&
		(pos.y <= maxBoxY && pos.y >= minBoxY))
	{
		if (maxBoxZ + radius.z > pos.z && boxPos.z < oldPos.z)
		{
			pos.z = maxBoxZ + radius.z;
			hitFlag = true;
			avoidFlag = false;
		}
		else if (minBoxZ - radius.z < pos.z && boxPos.z > oldPos.z)
		{
			pos.z = minBoxZ - radius.z;
			hitFlag = true;
			avoidFlag = false;
		}
	}

	if ((pos.z <= maxBoxZ && pos.z >= minBoxZ) &&
		(pos.y <= maxBoxY && pos.y >= minBoxY))
	{
		if (maxBoxX + radius.x > pos.x && boxPos.x < oldPos.x)
		{
			pos.x = maxBoxX + radius.x;
			hitFlag = true;
			avoidFlag = false;
		}
		else if (minBoxX - radius.x < pos.x && boxPos.x > oldPos.x)
		{
			pos.x = minBoxX - radius.x;
			hitFlag = true;
			avoidFlag = false;
		}
	}

	return hitFlag;
}

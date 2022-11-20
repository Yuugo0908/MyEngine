#include "Bullet.h"

bool Bullet::Initialize()
{
	bulletModel = bulletModel->CreateFromObject("sphere");
	bulletObj = Object3d::Create();
	bulletObj->SetModel(bulletModel);
	bulletObj->SetPosition({ 0.0f, 100.0f, 0.0f });
	bulletObj->SetScale({ 0.5, 0.5, 0.5 });
	bulletObj->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	return true;
}

void Bullet::Update(const XMFLOAT3& pPos, const XMFLOAT3& ePos)
{
	//bPos = bulletObj->GetPosition();
	if (shakeFlag)
	{
		Camera::GetInstance()->CameraShake(shakeFlag);
	}

	if (attackCount <= 30)
	{
		attackCount++;
	}

	if (GetLength(oldePos, bPos) >= 45.0f)
	{
		attackFlag = false;
		bPos = ePos;
		attackCount = 0;
	}

	if (!attackFlag)
	{
		bPos = ePos;
		oldePos = ePos;

		XMVECTOR playerPos = { pPos.x, pPos.y, pPos.z, 1 };
		XMVECTOR bulletPos = { ePos.x, ePos.y, ePos.z, 1 };

		XMVECTOR subPlayerEnemy = XMVectorSubtract(playerPos, bulletPos);
		XMVECTOR NsubPlayerEnemy = XMVector3Normalize(subPlayerEnemy);

		bSpeed = { NsubPlayerEnemy.m128_f32[0], NsubPlayerEnemy.m128_f32[1], NsubPlayerEnemy.m128_f32[2] };
	}

	bulletObj->SetPosition(bPos);
	bulletObj->Update();
}

void Bullet::Attack()
{
	if (attackCount >= 30)
	{
		bPos.x += bSpeed.x / 2;
		bPos.y += bSpeed.y / 3;
		bPos.z += bSpeed.z / 2;
	}
}

void Bullet::Finalize()
{
	delete bulletModel;
}

void Bullet::Draw()
{
	bulletObj->Draw();
}

void Bullet::Collision()
{
	shakeFlag = true;
	attackFlag = false;
	attackCount = 0;
}

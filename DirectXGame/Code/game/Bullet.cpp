#include "Bullet.h"

bool Bullet::Initialize(Model* bulletModel)
{
	bulletObj = Object3d::Create();
	bulletObj->SetModel(bulletModel);
	bulletObj->SetPosition({ 0.0f, 100.0f, 0.0f });
	bulletObj->SetScale({ 0.5, 0.5, 0.5 });
	bulletObj->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	return true;
}

void Bullet::Update(const XMFLOAT3& pPos, const XMFLOAT3& ePos)
{
	this->pPos = pPos;
	this->ePos = ePos;
	if (shakeFlag)
	{
		Camera::GetInstance()->CameraShake(shakeFlag);
	}

	if (attackCount < 50)
	{
		attackCount++;
	}

	if (GetLength(ePosOld, bPos) >= 20.0f)
	{
		attackFlag = false;
		Search();
		attackCount = 0;
	}

	if (!attackFlag)
	{
		bPos = ePos;
		Search();
	}

	bulletObj->SetPosition(bPos);
	bulletObj->Update();
}

void Bullet::Attack()
{

	XMVECTOR playerPos = { pPos.x, pPos.y, pPos.z, 1 };
	XMVECTOR bulletPos = { ePos.x, ePos.y, ePos.z, 1 };

	XMVECTOR subPlayerEnemy = XMVectorSubtract(playerPos, bulletPos);
	XMVECTOR NsubPlayerEnemy = XMVector3Normalize(subPlayerEnemy);

	bSpeed = { NsubPlayerEnemy.m128_f32[0], NsubPlayerEnemy.m128_f32[1], NsubPlayerEnemy.m128_f32[2] };

	if (attackCount >= 50)
	{
		bPos.x += bSpeed.x / 2;
		bPos.y += bSpeed.y / 2;
		bPos.z += bSpeed.z / 2;
	}
}

void Bullet::Search()
{
	ePosOld = ePos;

	XMVECTOR playerPos = { pPos.x, pPos.y, pPos.z, 1 };
	XMVECTOR bulletPos = { ePos.x, ePos.y, ePos.z, 1 };

	XMVECTOR subPlayerEnemy = XMVectorSubtract(playerPos, bulletPos);
	XMVECTOR NsubPlayerEnemy = XMVector3Normalize(subPlayerEnemy);

	bSpeed = { NsubPlayerEnemy.m128_f32[0], NsubPlayerEnemy.m128_f32[1], NsubPlayerEnemy.m128_f32[2] };
}

void Bullet::Draw()
{
	bulletObj->Draw();
}

void Bullet::Collision()
{
	shakeFlag = true;
	attackFlag = false;
	Search();
	attackCount = 0;
}

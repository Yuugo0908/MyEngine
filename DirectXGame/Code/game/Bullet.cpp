#include "Bullet.h"

bool Bullet::Initialize()
{
	bulletModel = bulletModel->CreateFromObject("sphere");
	bullet = Object3d::Create();
	bullet->SetModel(bulletModel);

	bullet->SetPosition({ 0.0f, 100.0f, 0.0f });
	bullet->SetScale({ 0.5, 0.5, 0.5 });
	bullet->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	bPos = bullet->GetPosition();
	return true;
}

void Bullet::Update(const XMFLOAT3& pPos, const XMFLOAT3& ePos)
{
	bullet->SetPosition(bPos);
	bullet->Update();

	if (attackFlag) { return; }

	bPos = bullet->GetPosition();
	bPos = ePos;

	XMVECTOR playerPos = { pPos.x, pPos.y, pPos.z, 1 };
	XMVECTOR bulletPos = { bPos.x, bPos.y, bPos.z, 1 };

	XMVECTOR subPlayerEnemy = XMVectorSubtract(playerPos, bulletPos);
	XMVECTOR NsubPlayerEnemy = XMVector3Normalize(subPlayerEnemy);

	bSpeed = { NsubPlayerEnemy.m128_f32[0], NsubPlayerEnemy.m128_f32[1], NsubPlayerEnemy.m128_f32[2] };
}

void Bullet::Attack(const std::unique_ptr<Object3d>& object, const XMFLOAT3& ePos)
{
	attackFlag = true;
	bPos.x += bSpeed.x / 3;
	bPos.y += bSpeed.y / 3;
	bPos.z += bSpeed.z / 3;

	if (Collision::CollisionObject(object, bullet) || GetLength(ePos, bPos) >= 30.0f)
	{
		attackFlag = false;
	}
}

void Bullet::Finalize()
{
	delete bulletModel;
}

void Bullet::Draw()
{
	bullet->Draw();
}

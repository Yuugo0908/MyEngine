#include "Enemy.h"

bool Enemy::Initialize()
{
	enemyModel = enemyModel->CreateFromObject("sphere");
	enemy = Object3d::Create();
	enemy->SetModel(enemyModel);

	enemy->SetPosition({ 0.0f, 5.0f, 5.0f });
	enemy->SetScale({ 1.0f, 1.0f, 1.0f });

	ePos = enemy->GetPosition();
	return true;
}

void Enemy::Update()
{
	if (!eAlive)
	{
		eAliveCount++;
		enemy->SetPosition({ 0.0f, 100.0f, 0.0f });

		if (eAliveCount == 60)
		{
			enemy->SetPosition({ 0.0f, 5.0f, 5.0f });
			eAlive = true;
			eAliveCount = 0;
		}

		ePos = enemy->GetPosition();
		enemy->Update();
		return;
	}

	if (ePos.y > 0.0f)
	{
		eVal -= eGra;
		ePos.y += eVal;
	}

	else if (ePos.y <= 0.0f)
	{
		ePos.y = 0.0f;
		eVal = 0.0f;
	}
	enemy->SetPosition(ePos);
	enemy->Update();
}

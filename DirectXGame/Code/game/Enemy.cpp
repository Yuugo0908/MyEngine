#include "Enemy.h"

bool Enemy::Initialize()
{
	player = new Player;

	// ƒ‚ƒfƒ‹‚Ì¶¬
	enemyModel = enemyModel->CreateFromObject("sphere");
	enemyObj = Object3d::Create();
	enemyObj->SetModel(enemyModel);

	// ˆÊ’u‚ð•Ï”‚ÉŠi”[
	enemyObj->SetPosition({ 0.0f, 5.0f, 5.0f });
	enemyObj->SetScale({ 1.0f, 1.0f, 1.0f });
	ePos = enemyObj->GetPosition();
	enemyObj->Update();

	return true;
}

void Enemy::Finalize()
{
	delete enemyModel;
}

void Enemy::Update()
{
	easeFlag = player->GetEaseFlag();
	if (!eAlive)
	{
		eAliveCount++;
		enemyObj->SetPosition({ 0.0f, 100.0f, 0.0f });

		if (eAliveCount == 60)
		{
			enemyObj->SetPosition({ 0.0f, 5.0f, 5.0f });
			eAlive = true;
			eAliveCount = 0;
		}

		ePos = enemyObj->GetPosition();
		enemyObj->Update();
		return;
	}

	if (ePos.y > 0.0f)
	{
		if (!easeFlag)
		{
			eVal -= eGra;
			ePos.y += eVal;
		}
	}

	else if (ePos.y <= 0.0f)
	{
		ePos.y = 0.0f;
		eVal = 0.0f;
	}
	enemyObj->SetPosition(ePos);
	enemyObj->Update();
}

void Enemy::Draw()
{
	enemyObj->Draw();
}

#include "Rope.h"

bool Rope::Initialize(Keyboard* keyboard)
{
	assert(keyboard);
	this->keyboard = keyboard;

	// モデルの生成
	ropeModel = ropeModel->CreateFromObject("rope");
	ropeObject = Object3d::Create();
	ropeObject->SetModel(ropeModel);

	// 位置、スケールを変数に格納
	ropeObject->SetScale({ 0.2f, 0.2f, 0.2f });
	rPos = ropeObject->GetPosition();
	rScale = ropeObject->GetScale();
	ropeObject->Update();

	return true;
}

void Rope::Update(XMFLOAT3& pPos, XMFLOAT3& ePos, const std::unique_ptr<Object3d>& object)
{
	if (!rFlag)
	{
		rPos = pPos + manageRopePos;
		rScale = manageRopeScale;
		ropeObject->SetPosition(rPos);
		ropeObject->SetScale(rScale);
		pEaseFlag = false;
		eEaseFlag = false;

		if (!rThrowFlag && !rBackFlag && keyboard->TriggerKey(DIK_J))
		{
			rThrowFlag = true;
			avoidTime = 0.0f;
		}
		Throw(pPos, ePos, rPos, rScale, object);

		ropeObject->Update();
		return;
	}

	// ロープがついている場合、敵を引き寄せる
	if (keyboard->TriggerKey(DIK_K) && keyboard->PushKey(DIK_W))
	{
		pEaseFlag = true;
		avoidTime = 0.0f;
	}
	else if (keyboard->TriggerKey(DIK_K) && keyboard->PushKey(DIK_S))
	{
		eEaseFlag = true;
		avoidTime = 0.0f;
	}

	if (pEaseFlag)
	{
		easing->EaseInUpdate(pPos, ePos, pPos, pEaseFlag, avoidTime);
	}
	else if (eEaseFlag)
	{
		easing->EaseInUpdate(ePos, pPos, ePos, eEaseFlag, avoidTime);
	}

	{
		//プレイヤーとエネミーの距離
		XMFLOAT3 length = { pPos.x - ePos.x, pPos.y - ePos.y, pPos.z - ePos.z };
		float len = GetLength(pPos, ePos);

		//最大値より大きいなら
		if (len > maxRope)
		{
			float wq = len / maxRope;
			len = maxRope;
			ePos = { pPos.x - length.x / wq, pPos.y - length.y / wq, pPos.z - length.z / wq };
		}

		// Y軸周りの角度
		angleY = (float)atan2(pPos.x - ePos.x, pPos.z - ePos.z);
		vecXZ = sqrtf((pPos.x - ePos.x) * (pPos.x - ePos.x) + (pPos.z - ePos.z) * (pPos.z - ePos.z));
		// X軸周りの角度
		angleX = (float)atan2(ePos.y - pPos.y, vecXZ);

		rPos = { (pPos.x + ePos.x) / 2, (pPos.y + ePos.y) / 2, (pPos.z + ePos.z) / 2 };
		rScale = { 0.2f, 0.2f , len / 2.0f };
		ropeObject->SetPosition(rPos);
		ropeObject->SetScale(rScale);
		ropeObject->SetRotation({ XMConvertToDegrees(angleX), XMConvertToDegrees(angleY), 0 });
		ropeObject->Update();
	}
}

void Rope::Throw(XMFLOAT3& pPos, XMFLOAT3& ePos, XMFLOAT3& rPos, XMFLOAT3& rScale, const std::unique_ptr<Object3d>& object)
{
	// フラグがtrueじゃない場合は初期化してリターンする
	if (!rThrowFlag && !rBackFlag)
	{
		manageRopePos = {};
		manageRopeScale = {};
		moveFlag = true;
		return;
	}

	moveFlag = false;
	// Y軸周りの角度
	angleY = (float)atan2(pPos.x - ePos.x, pPos.z - ePos.z);
	vecXZ = sqrtf((pPos.x - ePos.x) * (pPos.x - ePos.x) + (pPos.z - ePos.z) * (pPos.z - ePos.z));
	// X軸周りの角度
	angleX = (float)atan2(ePos.y - pPos.y, vecXZ);
	ropeObject->SetRotation({ XMConvertToDegrees(angleX), XMConvertToDegrees(angleY), 0 });

	XMVECTOR playerPos = { pPos.x, pPos.y, pPos.z, 1 };
	XMVECTOR enemyPos = { ePos.x, ePos.y, ePos.z, 1 };

	XMVECTOR subPlayerEnemy = XMVectorSubtract(enemyPos, playerPos);
	XMVECTOR NsubPlayerEnemy = XMVector3Normalize(subPlayerEnemy);

	XMFLOAT3 subPE = { NsubPlayerEnemy.m128_f32[0], NsubPlayerEnemy.m128_f32[1], NsubPlayerEnemy.m128_f32[2] };

	if (rThrowFlag)
	{
		manageRopePos.x += subPE.x;
		manageRopePos.y += subPE.y;
		manageRopePos.z += subPE.z;

		manageRopeScale.x += 0.02f;
		manageRopeScale.y += 0.02f;
		manageRopeScale.z += 0.7f;

		easing->EaseOutUpdate(rPos, manageRopePos, rPos, rThrowFlag, avoidTime);
		easing->EaseOutUpdate(rScale, manageRopeScale, rPos, rThrowFlag, avoidTime);

		Collision(object);

		if (avoidTime >= 1.0f)
		{
			avoidTime = 0.0f;
			rThrowFlag = false;
			rBackFlag = true;
		}
	}

	if (rBackFlag)
	{
		manageRopePos.x -= subPE.x;
		manageRopePos.y -= subPE.y;
		manageRopePos.z -= subPE.z;

		manageRopeScale.x -= 0.02f;
		manageRopeScale.y -= 0.02f;
		manageRopeScale.z -= 0.7f;

		easing->EaseOutUpdate(rPos, manageRopePos, rPos, rBackFlag, avoidTime);
		easing->EaseOutUpdate(rScale, manageRopeScale, rPos, rBackFlag, avoidTime);

		Collision(object);

		if (avoidTime >= 1.0f)
		{
			avoidTime = 0.0f;
			rThrowFlag = false;
			rBackFlag = false;
			moveFlag = true;
		}
	}
}

void Rope::Collision(const std::unique_ptr<Object3d>& object)
{
	if (!Collision::CollisionObject(object, ropeObject))
	{
		return;
	}

	manageRopePos = {};
	manageRopeScale = {};
	avoidTime = 0.0f;
	rThrowFlag = false;
	rBackFlag = false;
	if (keyboard->PushKey(DIK_S))
	{
		eEaseFlag = true;
	}
	else
	{
		pEaseFlag = true;
	}
	rFlag = true;
}

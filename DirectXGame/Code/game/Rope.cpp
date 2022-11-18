#include "Rope.h"

bool Rope::Initialize(Keyboard* keyboard, Mouse* mouse)
{
	assert(keyboard);
	assert(mouse);
	this->keyboard = keyboard;
	this->mouse = mouse;

	easing = new Easing;

	// モデルの生成
	ropeModel = ropeModel->CreateFromObject("rope");
	ropeObj = Object3d::Create();
	ropeObj->SetModel(ropeModel);

	// 位置、スケールを変数に格納
	ropeObj->SetScale({ 0.0f, 0.0f, 0.0f });
	rPos = ropeObj->GetPosition();
	rScale = ropeObj->GetScale();
	ropeObj->Update();

	ray.start = XMVectorSet(rPos.x, rPos.y, rPos.z, 1);
	ray.dir = XMVectorSet(0, 0, 1, 0);

	return true;
}

void Rope::Update(XMFLOAT3& pPos, XMFLOAT3& ePos, const std::unique_ptr<Object3d>& object)
{
	if (throwCount < 30)
	{
		throwCount++;
	}

	objLength = GetLength(pPos, ePos);

	if (!rFlag)
	{
		rPos = pPos + manageRopePos;
		rScale = manageRopeScale;
		ropeObj->SetPosition(rPos);
		ropeObj->SetScale(rScale);
		pEaseFlag = false;
		eEaseFlag = false;

		if (!rThrowFlag && !rBackFlag && mouse->TriggerMouseLeft())
		{
			moveFlag = false;
			rThrowFlag = true;
			avoidTime = 0.0f;
			if (objLength <= maxRope)
			{
				rRotFlag = true;
			}
		}
		if (throwCount == 30)
		{
			Throw(pPos, ePos, rPos, rScale, object);
		}

		if (Collision::CollisionObject(object, ropeObj))
		{
			Collision();
		}
		ropeObj->Update();
		return;
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

		//最大値より大きいなら
		if (objLength > maxRope)
		{
			float wq = objLength / maxRope;
			objLength = maxRope;
			ePos = { pPos.x - length.x / wq, pPos.y - length.y / wq, pPos.z - length.z / wq };
		}

		// Y軸周りの角度
		angleY = (float)atan2(pPos.x - ePos.x, pPos.z - ePos.z);
		vecXZ = sqrtf((pPos.x - ePos.x) * (pPos.x - ePos.x) + (pPos.z - ePos.z) * (pPos.z - ePos.z));
		// X軸周りの角度
		angleX = (float)atan2(ePos.y - pPos.y, vecXZ);

		rPos = { (pPos.x + ePos.x) / 2, (pPos.y + ePos.y) / 2, (pPos.z + ePos.z) / 2 };
		rScale = { 0.2f, 0.2f , objLength / 2.0f };
		ropeObj->SetPosition(rPos);
		ropeObj->SetScale(rScale);
		ropeObj->SetRotation({ XMConvertToDegrees(angleX), XMConvertToDegrees(angleY), 0 });
		ropeObj->Update();
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

	XMVECTOR playerPos = { pPos.x, pPos.y, pPos.z, 1 };
	XMVECTOR enemyPos = { ePos.x, ePos.y, ePos.z, 1 };

	XMVECTOR subPlayerEnemy = XMVectorSubtract(enemyPos, playerPos);
	XMVECTOR NsubPlayerEnemy = XMVector3Normalize(subPlayerEnemy);

	XMFLOAT3 subPE = { NsubPlayerEnemy.m128_f32[0], NsubPlayerEnemy.m128_f32[1], NsubPlayerEnemy.m128_f32[2] };

	if (rThrowFlag)
	{
		if (rRotFlag)
		{	
			// Y軸周りの角度
			angleY = (float)atan2(pPos.x - ePos.x, pPos.z - ePos.z);
			vecXZ = sqrtf((pPos.x - ePos.x) * (pPos.x - ePos.x) + (pPos.z - ePos.z) * (pPos.z - ePos.z));
			// X軸周りの角度
			angleX = (float)atan2(ePos.y - pPos.y, vecXZ);
			ropeObj->SetRotation({ XMConvertToDegrees(angleX), XMConvertToDegrees(angleY), 0 });
			ray.dir = XMVectorSet(XMConvertToDegrees(angleX), XMConvertToDegrees(angleY), 0, 0);
			manageRopePos.x += subPE.x;
			manageRopePos.y += subPE.y;
			manageRopePos.z += subPE.z;
		}
		else
		{
			ropeObj->SetRotation({ 0, 0, 0 });
			manageRopePos.z += 0.6f;
			rRotFlag = false;
		}
		ray.start = XMVectorSet(manageRopePos.x, manageRopePos.y, manageRopePos.z, 1);

		manageRopeScale.x += 0.02f;
		manageRopeScale.y += 0.02f;
		manageRopeScale.z += 0.6f;

		easing->EaseOutUpdate(rPos, manageRopePos, rPos, rThrowFlag, avoidTime);
		easing->EaseOutUpdate(rScale, manageRopeScale, rPos, rThrowFlag, avoidTime);

		if (avoidTime >= 1.0f)
		{
			avoidTime = 0.0f;
			rThrowFlag = false;
			rBackFlag = true;
		}
	}

	if (rBackFlag)
	{
		if (rRotFlag)
		{
			// Y軸周りの角度
			angleY = (float)atan2(pPos.x - ePos.x, pPos.z - ePos.z);
			vecXZ = sqrtf((pPos.x - ePos.x) * (pPos.x - ePos.x) + (pPos.z - ePos.z) * (pPos.z - ePos.z));
			// X軸周りの角度
			angleX = (float)atan2(ePos.y - pPos.y, vecXZ);
			ropeObj->SetRotation({ XMConvertToDegrees(angleX), XMConvertToDegrees(angleY), 0 });
			ray.dir = XMVectorSet(XMConvertToDegrees(angleX), XMConvertToDegrees(angleY), 0, 0);
			manageRopePos.x -= subPE.x;
			manageRopePos.y -= subPE.y;
			manageRopePos.z -= subPE.z;
		}
		else
		{
			manageRopePos.z -= 0.6f;
		}
		ray.start = XMVectorSet(manageRopePos.x, manageRopePos.y, manageRopePos.z, 1);

		manageRopeScale.x -= 0.02f;
		manageRopeScale.y -= 0.02f;
		manageRopeScale.z -= 0.6f;

		easing->EaseOutUpdate(rPos, manageRopePos, rPos, rBackFlag, avoidTime);
		easing->EaseOutUpdate(rScale, manageRopeScale, rPos, rBackFlag, avoidTime);

		if (avoidTime >= 1.0f)
		{
			avoidTime = 0.0f;
			rThrowFlag = false;
			rBackFlag = false;
			moveFlag = true;
			rRotFlag = false;
			throwCount = 0;
		}
	}
}

void Rope::Collision()
{
	manageRopePos = {};
	manageRopeScale = {};
	avoidTime = 0.0f;
	rThrowFlag = false;
	rBackFlag = false;
	rRotFlag = false;
	if (mouse->PushMouseLeft())
	{
		eEaseFlag = true;
	}
	else
	{
		pEaseFlag = true;
	}
	throwCount = 0;
	rFlag = true;
}

void Rope::CircularMotion(XMFLOAT3& pos, const XMFLOAT3 centerPos, const float r, int& angle, const int add)
{
	angle += add;

	// 円運動の処理
	pos.x = (cosf(3.14f / 180.0f * angle) * r) + centerPos.x;
	pos.y = (sinf(3.14f / 180.0f * angle) * r) + centerPos.y;
	pos.z = (tanf(3.14f / 180.0f * angle) * r) + centerPos.z;
}

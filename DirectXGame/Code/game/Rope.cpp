#include "Rope.h"

bool Rope::Initialize(Keyboard* keyboard, Mouse* mouse)
{
	if (ropeModel == NULL)
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
	}

	// 位置、スケールを変数に格納
	ropeObj->SetScale({ 0.0f, 0.0f, 0.0f });
	rPos = ropeObj->GetPosition();
	rRadius = ropeObj->GetScale();
	ropeObj->Update();

	ray.start = XMVectorSet(rPos.x, rPos.y, rPos.z, 1);
	ray.dir = XMVectorSet(0, 0, 1, 0);

	return true;
}

void Rope::Update(XMFLOAT3& pPos)
{
	if (throwCount < 30)
	{
		throwCount++;
	}

	if (!rFlag)
	{
		rPos = pPos + manageRopePos;
		rRadius = manageRopeScale;
		ropeObj->SetPosition(rPos);
		ropeObj->SetScale(rRadius);
		pEaseFlag = false;
		eEaseFlag = false;

		if (!rThrowFlag && !rBackFlag && mouse->TriggerMouseLeft())
		{
			rPos = pPos;
			moveFlag = false;
			rThrowFlag = true;
			avoidTime = 0.0f;
			if (objLength <= 10.0f)
			{
				rRotFlag = true;
			}
		}
		if (throwCount == 30)
		{
			Throw(pPos);
		}
		ropeObj->Update();
		return;
	}

	objLength = GetLength(pPos, endPos);
	{
		XMFLOAT3 length = { pPos.x - endPos.x, pPos.y - endPos.y, pPos.z - endPos.z };

		angleY = (float)atan2(pPos.x - endPos.x, pPos.z - endPos.z);
		vecXZ = sqrtf((pPos.x - endPos.x) * (pPos.x - endPos.x) + (pPos.z - endPos.z) * (pPos.z - endPos.z));
		angleX = (float)atan2(endPos.y - pPos.y, vecXZ);

		rPos = { (pPos.x + endPos.x) / 2, (pPos.y + endPos.y) / 2, (pPos.z + endPos.z) / 2 };
		rRadius = { 0.2f, 0.2f , objLength / 2.0f };
		ropeObj->SetPosition(rPos);
		ropeObj->SetScale(rRadius);
		ropeObj->SetRotation({ XMConvertToDegrees(angleX), XMConvertToDegrees(angleY), 0 });
		ropeObj->Update();
	}


	if (pEaseFlag)
	{
		easing->EaseInUpdate(pPos, endPos, pPos, pEaseFlag, avoidTime);
	}
	//else if (eEaseFlag)
	//{
	//	easing->EaseInUpdate(ePos, pPos, ePos, eEaseFlag, avoidTime);
	//}
}

void Rope::Throw(XMFLOAT3& pPos)
{
	// フラグがtrueじゃない場合は初期化してリターンする
	if (!rThrowFlag && !rBackFlag)
	{
		manageRopePos = {};
		manageRopeScale = {};
		moveFlag = true;
		return;
	}

	XMFLOAT3 cSpeed = Camera::GetInstance()->CameraTrack(pPos);
	float cRot = Camera::GetInstance()->CameraRot(pPos);

	if (rThrowFlag)
	{
		ropeObj->SetRotation({ 0, XMConvertToDegrees(cRot), 0});
		manageRopePos.x += cSpeed.x;
		manageRopePos.z += cSpeed.z;

		manageRopeScale.x += 0.02f;
		manageRopeScale.y += 0.02f;
		manageRopeScale.z += 0.6f;

		avoidTime += 0.1f;

		if (avoidTime >= 1.0f)
		{
			avoidTime = 0.0f;
			rThrowFlag = false;
			rBackFlag = true;
		}
	}

	if (rBackFlag)
	{
		manageRopePos.x -= cSpeed.x;
		manageRopePos.z -= cSpeed.z;

		manageRopeScale.x -= 0.02f;
		manageRopeScale.y -= 0.02f;
		manageRopeScale.z -= 0.6f;

		avoidTime += 0.1f;

		if (avoidTime >= 1.0f)
		{
			avoidTime = 0.0f;
			rThrowFlag = false;
			rBackFlag = false;
			moveFlag = true;
			throwCount = 0;
		}
	}
}

void Rope::Collision(XMFLOAT3& startPos, XMFLOAT3& endPos)
{
	manageRopePos = {};
	manageRopeScale = {};
	avoidTime = 0.0f;
	rThrowFlag = false;
	rBackFlag = false;
	rRotFlag = false;
	this->startPos = startPos;
	this->endPos = endPos;
	rFlag = true;
	pEaseFlag = true;
	throwCount = 0;
}

void Rope::CircularMotion(XMFLOAT3& pos, const XMFLOAT3 centerPos, const float r, int& angle, const int add)
{
	angle += add;

	// 円運動の処理
	pos.x = (cosf(3.14f / 180.0f * angle) * r) + centerPos.x;
	pos.y = (sinf(3.14f / 180.0f * angle) * r) + centerPos.y;
	pos.z = (tanf(3.14f / 180.0f * angle) * r) + centerPos.z;
}

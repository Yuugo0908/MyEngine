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

	rFlag = false; // 接触フラグ
	rThrowFlag = false; // ロープを飛ばす
	rBackFlag = false; // ロープを戻す

	// 位置、スケールを変数に格納
	ropeObj->SetScale({ 0.0f, 0.0f, 0.0f });
	rPos = ropeObj->GetPosition();
	rScale = ropeObj->GetScale();
	ropeObj->Update();

	return true;
}

void Rope::Update(XMFLOAT3& pPos)
{
	startPos = pPos;

	if (throwCount < 30)
	{
		throwCount++;
	}

	if (!rFlag)
	{
		rPos = startPos + manageRopePos;
		rScale = manageRopeScale;
		ropeObj->SetPosition(rPos);
		ropeObj->SetScale(rScale);
		pEaseFlag = false;
		eEaseFlag = false;

		if (!rThrowFlag && !rBackFlag && mouse->TriggerMouseLeft())
		{
			rPos = startPos;
			moveFlag = false;
			rThrowFlag = true;
			avoidTime = 0.0f;
		}

		ropeObj->Update();
		return;
	}

	objLength = GetLength(startPos, endPos);
	XMFLOAT3 length = { startPos.x - endPos.x, startPos.y - endPos.y, startPos.z - endPos.z };

	angleY = (float)atan2(startPos.x - endPos.x, startPos.z - endPos.z);
	vecXZ = sqrtf((startPos.x - endPos.x) * (startPos.x - endPos.x) + (startPos.z - endPos.z) * (startPos.z - endPos.z));
	angleX = (float)atan2(endPos.y - startPos.y, vecXZ);

	rPos = { (startPos.x + endPos.x) / 2, (startPos.y + endPos.y) / 2, (startPos.z + endPos.z) / 2 };
	rScale = { 0.2f, 0.2f , objLength / 2.0f };
	ropeObj->SetPosition(rPos);
	ropeObj->SetScale(rScale);
	ropeObj->SetRotation({ XMConvertToDegrees(angleX), XMConvertToDegrees(angleY), 0 });
	ropeObj->Update();
}

void Rope::Throw(XMFLOAT3 pPos, XMFLOAT3& ePos, float& length)
{
	// フラグがtrueじゃない場合は初期化してリターンする
	if (!rThrowFlag && !rBackFlag)
	{
		manageRopePos = {};
		manageRopeScale = {};
		moveFlag = true;
		return;
	}

	if (length < 15.0f)
	{
		rRotFlag = true;
	}

	XMVECTOR playerPos = { pPos.x, pPos.y, pPos.z, 1 };
	XMVECTOR enemyPos = { ePos.x, ePos.y, ePos.z, 1 };

	XMVECTOR subPlayerEnemy = XMVectorSubtract(enemyPos, playerPos);
	XMVECTOR NsubPlayerEnemy = XMVector3Normalize(subPlayerEnemy);

	XMFLOAT3 subPE = { NsubPlayerEnemy.m128_f32[0], NsubPlayerEnemy.m128_f32[1], NsubPlayerEnemy.m128_f32[2] };

	cSpeed = Camera::GetInstance()->CameraTrack(pPos);
	cRot = Camera::GetInstance()->CameraRot(pPos);

	if (rThrowFlag)
	{
		if (rRotFlag)
		{
			// Y軸周りの角度
			angleY = (float)atan2(pPos.x - ePos.x, pPos.z - ePos.z);
			vecXZ = sqrtf((pPos.x - ePos.x) * (pPos.x - ePos.x) + (pPos.z - ePos.z) * (pPos.z - ePos.z));
			// X軸周りの角度
			angleX = (float)atan2(ePos.y - pPos.y, vecXZ);
			rRot = { XMConvertToDegrees(angleX), XMConvertToDegrees(angleY), 0 };
			ropeObj->SetRotation(rRot);
			manageRopePos.x += subPE.x;
			manageRopePos.y += subPE.y;
			manageRopePos.z += subPE.z;
		}
		else
		{
			rRot = { 0, XMConvertToDegrees(cRot), 0 };
			ropeObj->SetRotation(rRot);
			manageRopePos.x += cSpeed.x;
			manageRopePos.z += cSpeed.z;
			rRotFlag = false;
		}

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
		if (rRotFlag)
		{
			// Y軸周りの角度
			angleY = (float)atan2(pPos.x - ePos.x, pPos.z - ePos.z);
			vecXZ = sqrtf((pPos.x - ePos.x) * (pPos.x - ePos.x) + (pPos.z - ePos.z) * (pPos.z - ePos.z));
			// X軸周りの角度
			angleX = (float)atan2(ePos.y - pPos.y, vecXZ);
			ropeObj->SetRotation({ XMConvertToDegrees(angleX), XMConvertToDegrees(angleY), 0 });
			manageRopePos.x -= subPE.x;
			manageRopePos.y -= subPE.y;
			manageRopePos.z -= subPE.z;
		}
		else
		{
			manageRopePos.x -= cSpeed.x;
			manageRopePos.z -= cSpeed.z;
		}

		manageRopeScale.x -= 0.02f;
		manageRopeScale.y -= 0.02f;
		manageRopeScale.z -= 0.6f;


		avoidTime += 0.1f;

		if (avoidTime >= 1.0f)
		{
			avoidTime = 0.0f;
			rThrowFlag = false;
			rBackFlag = false;
			rRotFlag = false;
			moveFlag = true;
			throwCount = 0;
			manageRopePos = {};
			manageRopeScale = {};
			cSpeed = {};
			cRot = 0.0f;

			ePos = { 0.0f, -1000.0f, 0.0f };
			length = 10.0f;
		}
	}
}

bool Rope::Collision(const std::unique_ptr<Object3d>& object, XMFLOAT3 pPos)
{
	//ロープを飛ばしていなかった場合は即リターンする
	if (!rThrowFlag && !rBackFlag)
	{
		return false;
	}

	// レイの当たり判定(直線上に敵がいればtrueそれ以外はfalse)
	XMFLOAT3 pos = object->GetPosition();
	XMFLOAT3 scale = object->GetScale();

	XMFLOAT3 lay = pPos;
	XMFLOAT3 vec = manageRopePos;
	XMFLOAT3 center = pos;
	float radius = scale.x;

	center = center - lay;

	float A = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
	float B = vec.x * center.x + vec.y * center.y + vec.z * center.z;
	float C = center.x * center.x + center.y * center.y + center.z * center.z - radius * radius;

	if (A == 0.0f)
	{
		return false;
	}

	float s = B * B - A * C;
	if (s < 0.0f)
	{
		return false;
	}

	s = sqrtf(s);
	float a1 = (B - s) / A;
	float a2 = (B + s) / A;

	if (a1 < 0.0f || a2 < 0.0f)
	{
		return false;
	}

	// 球の当たり判定
	//長さ
	float l_x = rPos.x - pos.x;
	float l_y = rPos.y - pos.y;
	float l_z = rPos.z - pos.z;

	float r = (scale.z + manageRopeScale.z);

	if ((l_x * l_x) + (l_y * l_y) + (l_z * l_z) <= (r * r))
	{
		endPos = pos;
		manageRopePos = {};
		manageRopeScale = {};
		avoidTime = 0.0f;
		rThrowFlag = false;
		rBackFlag = false;
		rRotFlag = false;
		rFlag = true;
		throwCount = 0;
		return true;
	}

	return false;
}

void Rope::Reset()
{
	avoidTime = 0.0f;
	rThrowFlag = false;
	rBackFlag = false;

	// 変数
	rPos = {};
	rScale = {};
	rRot = {};
	manageRopePos = {}; // ロープ位置管理用
	manageRopeScale = { 0.0f, 0.0f, 0.0f }; // ロープスケール管理用
	angleX = 0.0f; // X軸
	angleY = 0.0f; // Y軸
	vecXZ = 0.0f; // XZ平面上のベクトル
	objLength = 0.0f;
	rFlag = false; // 接触フラグ
	throwCount = 0;

	// 突進用
	startPos = {}; // 開始位置
	endPos = {}; // 終点位置
	pEaseFlag = false;
	eEaseFlag = false;

	// 移動管理フラグ
	moveFlag = false; // 自機が移動しているか
}

void Rope::CircularMotion(XMFLOAT3& pos, const XMFLOAT3 centerPos, const float r, int& angle, const int add)
{
	angle += add;

	// 円運動の処理
	pos.x = (cosf(3.14f / 180.0f * angle) * r) + centerPos.x;
	pos.y = (sinf(3.14f / 180.0f * angle) * r) + centerPos.y;
	pos.z = (tanf(3.14f / 180.0f * angle) * r) + centerPos.z;
}

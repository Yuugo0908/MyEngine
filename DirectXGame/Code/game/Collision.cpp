#include "Collision.h"

using namespace DirectX;
using DirectX::operator+;

Collision* Collision::GetInstance()
{
	static Collision instance;

	return &instance;
}

float Collision::GetLength(XMFLOAT3 pos_a, XMFLOAT3 pos_b)
{
	XMFLOAT3 len = { pos_a.x - pos_b.x, pos_a.y - pos_b.y, pos_a.z - pos_b.z };

	return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
}

bool Collision::CollisionObject(const std::unique_ptr<Object3d>& object_a, const std::unique_ptr<Object3d>& object_b)
{
	//座標
	XMFLOAT3 p_a = object_a->GetPosition();
	XMFLOAT3 p_b = object_b->GetPosition();

	//半径
	XMFLOAT3 r_a = object_a->GetScale();
	XMFLOAT3 r_b = object_b->GetScale();

	//長さ
	float l_x = sqrtf(powf(p_a.x - p_b.x, 2));
	float l_y = sqrtf(powf(p_a.y - p_b.y, 2));
	float l_z = sqrtf(powf(p_a.z - p_b.z, 2));

	//半径の合計より短ければ
	if (l_x < r_a.x + r_b.x && l_y < r_a.y + r_b.y && l_z < r_a.z + r_b.z)
	{
		return true;
	}

	return false;
}

bool Collision::CollisionSpherePlane(const Sphere& sphere, const Plane& plane, XMVECTOR* inter)
{
	// 座標系の原点から球の中心座標への距離
	XMVECTOR distV = XMVector3Dot(sphere.center, plane.normal);
	// 平面の原点距離を減算することで、平面と球の中心との距離が出る
	float dist = distV.m128_f32[0] - plane.distance;
	// 距離の絶対値が半径より大きければ当たっていない
	if (fabsf(dist) > sphere.radius) return false;

	// 擬似交点を計算
	if (inter)
	{
		// 平面上の最近接点を擬似交点とする
		*inter = -dist * plane.normal * sphere.center;
	}
	return true;
}

bool Collision::CollisionSphereTriangle(const Sphere& sphere, const Triangle& triangle, XMVECTOR* closest)
{
	return false;
}

bool Collision::CollisionRayPlane(const Ray& ray, const Plane& plane, float* distance, XMVECTOR* inter)
{
	const float epsilon = 1.0e-5f; // 誤差吸収用の微小な値

	// 面法線とレイの方向ベクトルの内積
	float d1 = XMVector3Dot(plane.normal, ray.dir).m128_f32[0];
	// 裏面には当たらない
	if (d1 > epsilon)
	{
		return false;
	}

	// 始点と原点の距離(平面の法線方向)
	// 面法線とレイの始点座標(位置ベクトル)の内積
	float d2 = XMVector3Dot(plane.normal, ray.start).m128_f32[0];
	// 始点と平面の距離(平面の法線方向)
	float dist = d2 - plane.distance;
	// 始点と平面の距離(レイ方向)
	float t = dist / -d1;

	// 交点が始点より後ろにある場合、当たらない
	if (t < 0)
	{
		return false;
	}
	// 距離を書き込む
	if (distance)
	{
		*distance = t;
	}
	// 交点を計算
	if (inter)
	{
		*inter = ray.start + t * ray.dir;
	}

	return true;
}

bool Collision::CollisionRayTriangle(const Ray& ray, const Triangle& triangle, float* distance, XMVECTOR* inter)
{
	return false;
}

bool Collision::CollisionRaySphere(const Ray& ray, const Sphere& sphere, float* distance, XMVECTOR* inter)
{
	return false;
}

bool Collision::CollisionBoxPoint(const XMFLOAT3 boxPos, const XMFLOAT3 boxRadius, XMFLOAT3& pPos, const XMFLOAT3 pRadius, XMFLOAT3 pOldPos)
{
	//マップチップ
	//X, Y
	XMFLOAT3 mapPos = {};
	//Radius
	XMFLOAT3 mapRadius = {};

	//フラグ
	bool hitFlag = false;

	mapPos = boxPos;
	mapRadius = boxRadius;

	// 判定
	float maxMapX = mapPos.x + mapRadius.x;
	float minMapX = mapPos.x - mapRadius.x;
	float maxMapY = mapPos.y + mapRadius.y;
	float minMapY = mapPos.y - mapRadius.y;
	float maxMapZ = mapPos.z + mapRadius.z;
	float minMapZ = mapPos.z - mapRadius.z;

	if ((pPos.x <= maxMapX && pPos.x >= minMapX) &&
		(pPos.y <= maxMapY && pPos.y >= minMapY))
	{
		if (maxMapZ + pRadius.z > pPos.z && mapPos.z < pOldPos.z)
		{
			pPos.z = maxMapZ + pRadius.z;
			hitFlag = true;
		}
		else if (minMapZ - pRadius.z < pPos.z && mapPos.z > pOldPos.z)
		{
			pPos.z = minMapZ - pRadius.z;
			hitFlag = true;
		}
	}

	if ((pPos.z <= maxMapZ && pPos.z >= minMapZ) &&
		(pPos.y <= maxMapY && pPos.y >= minMapY))
	{
		if (maxMapX + pRadius.x > pPos.x && mapPos.x < pOldPos.x)
		{
			pPos.x = maxMapX + pRadius.x;
			hitFlag = true;
		}
		else if (minMapX - pRadius.x < pPos.x && mapPos.x > pOldPos.x)
		{
			pPos.x = minMapX - pRadius.x;
			hitFlag = true;
		}
	}

	if ((pPos.x <= maxMapX && pPos.x >= minMapX) &&
		(pPos.z <= maxMapZ && pPos.z >= minMapZ))
	{
		if (maxMapY + pRadius.y > pPos.y && mapPos.y < pOldPos.y)
		{
			pPos.y = maxMapY + pRadius.y;
			hitFlag = true;
		}
		else if (minMapY - pRadius.y < pPos.y && mapPos.y > pOldPos.y)
		{
			pPos.y = minMapY - pRadius.y;
			hitFlag = true;
		}
	}
	return hitFlag;
}

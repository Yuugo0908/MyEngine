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

bool Collision::CollisionSpherePlane(const Sphere& sphere, const Plane& plane, XMFLOAT3* inter)
{
	// 座標系の原点から球の中心座標への距離
	float distV = Operator::dot(sphere.center, plane.normal);
	// 平面の原点距離を減算することで、平面と球の中心との距離が出る
	float dist = distV - plane.distance;
	// 距離の絶対値が半径より大きければ当たっていない
	if (fabsf(dist) > sphere.radius) return false;

	// 擬似交点を計算
	if (inter)
	{
		// 平面上の最近接点を擬似交点とする
		*inter =  plane.normal * sphere.center * -dist;
	}
	return true;
}

bool Collision::CollisionSphereTriangle(const Sphere& sphere, const Triangle& triangle, XMVECTOR* closest)
{
	return false;
}

bool Collision::CollisionRayPlane(const Ray& ray, const Plane& plane, float* distance, XMVECTOR* inter)
{
	return false;
}

bool Collision::CollisionRayTriangle(const Ray& ray, const Triangle& triangle, float* distance, XMVECTOR* inter)
{
	return false;
}

bool Collision::CollisionRaySphere(const Ray& ray, const Sphere& sphere, float* distance, XMVECTOR* inter)
{
	return false;
}

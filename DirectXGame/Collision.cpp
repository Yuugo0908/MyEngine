#include "Collision.h"

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
	//ç¿ïW
	XMFLOAT3 p_a = object_a->GetPosition();
	XMFLOAT3 p_b = object_b->GetPosition();

	//îºåa
	float r_a = object_a->GetScale().x;
	float r_b = object_a->GetScale().x;

	//í∑Ç≥
	float l_x = sqrtf(powf(p_a.x - p_b.x, 2));
	float l_y = sqrtf(powf(p_a.y - p_b.y, 2));
	float l_z = sqrtf(powf(p_a.z - p_b.z, 2));

	//îºåaÇÃçáåvÇÊÇËíZÇØÇÍÇŒ
	if (l_x < r_a + r_b && l_y < r_a + r_b && l_z < r_a + r_b)
	{
		return true;
	}

	return false;
}

bool Collision::CollisionSpherePlane(const Sphere& sphere, const Plane* plane, XMVECTOR* inter)
{
	return false;
}

bool Collision::CollisionSphereTriangle(const Sphere& sphere, const Triangle* triangle, XMVECTOR* closest)
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

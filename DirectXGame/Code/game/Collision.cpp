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
	//���W
	XMFLOAT3 p_a = object_a->GetPosition();
	XMFLOAT3 p_b = object_b->GetPosition();

	//���a
	XMFLOAT3 r_a = object_a->GetScale();
	XMFLOAT3 r_b = object_b->GetScale();

	//����
	float l_x = sqrtf(powf(p_a.x - p_b.x, 2));
	float l_y = sqrtf(powf(p_a.y - p_b.y, 2));
	float l_z = sqrtf(powf(p_a.z - p_b.z, 2));

	//���a�̍��v���Z�����
	if (l_x < r_a.x + r_b.x && l_y < r_a.y + r_b.y && l_z < r_a.z + r_b.z)
	{
		return true;
	}

	return false;
}

bool Collision::CollisionSpherePlane(const Sphere& sphere, const Plane& plane, XMVECTOR* inter)
{
	// ���W�n�̌��_���狅�̒��S���W�ւ̋���
	XMVECTOR distV = XMVector3Dot(sphere.center, plane.normal);
	// ���ʂ̌��_���������Z���邱�ƂŁA���ʂƋ��̒��S�Ƃ̋������o��
	float dist = distV.m128_f32[0] - plane.distance;
	// �����̐�Βl�����a���傫����Γ������Ă��Ȃ�
	if (fabsf(dist) > sphere.radius) return false;

	// �[����_���v�Z
	if (inter)
	{
		// ���ʏ�̍ŋߐړ_���[����_�Ƃ���
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
	const float epsilon = 1.0e-5f; // �덷�z���p�̔����Ȓl

	// �ʖ@���ƃ��C�̕����x�N�g���̓���
	float d1 = XMVector3Dot(plane.normal, ray.dir).m128_f32[0];
	// ���ʂɂ͓�����Ȃ�
	if (d1 > epsilon)
	{
		return false;
	}

	// �n�_�ƌ��_�̋���(���ʂ̖@������)
	// �ʖ@���ƃ��C�̎n�_���W(�ʒu�x�N�g��)�̓���
	float d2 = XMVector3Dot(plane.normal, ray.start).m128_f32[0];
	// �n�_�ƕ��ʂ̋���(���ʂ̖@������)
	float dist = d2 - plane.distance;
	// �n�_�ƕ��ʂ̋���(���C����)
	float t = dist / -d1;

	// ��_���n�_�����ɂ���ꍇ�A������Ȃ�
	if (t < 0)
	{
		return false;
	}
	// ��������������
	if (distance)
	{
		*distance = t;
	}
	// ��_���v�Z
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
	//�}�b�v�`�b�v
	//X, Y
	XMFLOAT3 mapPos = {};
	//Radius
	XMFLOAT3 mapRadius = {};

	//�t���O
	bool hitFlag = false;

	mapPos = boxPos;
	mapRadius = boxRadius;

	// ����
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

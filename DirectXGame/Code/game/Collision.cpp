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

bool Collision::CollisionSphere(const Sphere& sphere1, const Sphere& sphere2)
{
	float x = sphere2.center.m128_f32[0] - sphere1.center.m128_f32[0];
	float y = sphere2.center.m128_f32[1] - sphere1.center.m128_f32[1];
	float z = sphere2.center.m128_f32[2] - sphere1.center.m128_f32[2];

	float r = sphere1.radius + sphere2.radius;

	if ((x * x) + (y * y) + (z * z) <= (r * r))
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

bool Collision::CollisionRaySphere(const Ray& ray, const Sphere& sphere)
{
	XMVECTOR m = ray.start - sphere.center;
	float b = XMVector3Dot(m, ray.dir).m128_f32[1];
	float c = XMVector3Dot(m, m).m128_f32[1] - sphere.radius * sphere.radius;

	if (c > 0.0f && b > 0.0f)
	{
		return false;
	}

	float discr = b * b - c;

	if (discr < 0.0f)
	{
		return false;
	}

	float t = -b - sqrtf(discr);

	if (t < 0)
	{
		t = 0.0f;
	}

	return true;
}

bool Collision::CollisionBoxPoint(const XMFLOAT3 boxPos, const XMFLOAT3 boxRadius, XMFLOAT3& pos, const XMFLOAT3 radius, XMFLOAT3 oldPos)
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

	if ((pos.x <= maxMapX && pos.x >= minMapX) &&
		(pos.y <= maxMapY && pos.y >= minMapY))
	{
		if (maxMapZ + radius.z > pos.z && mapPos.z < oldPos.z)
		{
			pos.z = maxMapZ + radius.z;
			hitFlag = true;
		}
		else if (minMapZ - radius.z < pos.z && mapPos.z > oldPos.z)
		{
			pos.z = minMapZ - radius.z;
			hitFlag = true;
		}
	}

	if ((pos.z <= maxMapZ && pos.z >= minMapZ) &&
		(pos.y <= maxMapY && pos.y >= minMapY))
	{
		if (maxMapX + radius.x > pos.x && mapPos.x < oldPos.x)
		{
			pos.x = maxMapX + radius.x;
			hitFlag = true;
		}
		else if (minMapX - radius.x < pos.x && mapPos.x > oldPos.x)
		{
			pos.x = minMapX - radius.x;
			hitFlag = true;
		}
	}

	if ((pos.x <= maxMapX && pos.x >= minMapX) &&
		(pos.z <= maxMapZ && pos.z >= minMapZ))
	{
		if (maxMapY + radius.y > pos.y && mapPos.y < oldPos.y)
		{
			pos.y = maxMapY + radius.y;
			hitFlag = true;
		}
		else if (minMapY - radius.y < pos.y && mapPos.y > oldPos.y)
		{
			pos.y = minMapY - radius.y;
			hitFlag = true;
		}
	}
	return hitFlag;
}

bool Collision::CollisionStage(const XMFLOAT3 stagePos, const XMFLOAT3 stageRadius, XMFLOAT3& pos, const XMFLOAT3 radius, XMFLOAT3 oldPos)
{
	// ����
	float maxX = stagePos.x + stageRadius.x;
	float maxY = stagePos.y + stageRadius.y;
	float maxZ = stagePos.z + stageRadius.z;
	float minX = stagePos.x - stageRadius.x;
	float minY = stagePos.y - stageRadius.y;
	float minZ = stagePos.z - stageRadius.z;

	bool hitFlag = false;

	if ((pos.x < maxX && pos.x > minX) &&
		(pos.z < maxZ && pos.z > minZ))
	{
		if (maxY + radius.y > pos.y && stagePos.y < oldPos.y)
		{
			if (stagePos.y + radius.y >= pos.y)
			{
				pos.y = oldPos.y;
			}
			hitFlag = true;
		}
	}

	return hitFlag;
}



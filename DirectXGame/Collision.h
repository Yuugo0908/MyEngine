#pragma once
#include "Object3d.h"
#include "Operator.h"
class Collision
{
private: // �G�C���A�X
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;
public: // �T�u�N���X
	struct Sphere
	{
		// ���S���W
		XMFLOAT3 center = { 0, 0, 0 };
		// ���a
		float radius = 1.0f;
	};
	struct Plane
	{
		// �@���x�N�g��
		XMFLOAT3 normal = { 0, 0, 0 };
		// ���_����̋���
		float distance = 0.0f;
	};
	struct Ray
	{
		// �n�_���W
		XMFLOAT3 start = { 0, 0, 0 };
		// ����
		XMFLOAT3 dir = { 1, 0, 0 };
	};
	struct Triangle
	{
		// ���_���W
		XMFLOAT3 p0;
		XMFLOAT3 p1;
		XMFLOAT3 p2;
		// �@���x�N�g��
		XMFLOAT3 normal;
	};
public: // �ÓI�����o�֐�

	static Collision* GetInstance();
	//�I�u�W�F�N�g���m�̓����蔻��
	static bool CollisionObject(const std::unique_ptr<Object3d>& object_a, const std::unique_ptr<Object3d>& object_b);
	// ���ƕ��ʂ̓����蔻��
	static bool CollisionSpherePlane(const Sphere& sphere, const Plane& plane, XMFLOAT3* inter);
	// ���ƎO�p�`�̓����蔻��
	static bool CollisionSphereTriangle(const Sphere& sphere, const Triangle& triangle, XMVECTOR* closest);

	// ���C�ƕ��ʂ̓����蔻��
	static bool CollisionRayPlane(const Ray& ray, const Plane& plane, float* distance = nullptr, XMVECTOR* inter = nullptr);
	// ���C�Ɩ@���t���O�p�`�̓����蔻��
	static bool CollisionRayTriangle(const Ray& ray, const Triangle& triangle, float* distance = nullptr, XMVECTOR* inter = nullptr);
	// ���C�Ƌ��̓����蔻��
	static bool CollisionRaySphere(const Ray& ray, const Sphere& sphere, float* distance = nullptr, XMVECTOR* inter = nullptr);

public: // �����o�֐�

	// �I�u�W�F�N�g���m�̋������擾
	float GetLength(XMFLOAT3 pos_a, XMFLOAT3 pos_b);

private: // �����o�ϐ�

	Operator* ope = nullptr;
};


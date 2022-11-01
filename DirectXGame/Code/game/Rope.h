#pragma once

#include "Object3d.h"
#include "Keyboard.h"
#include "Easing.h"
#include "Collision.h"
#include "Operator.h"
#include <DirectXMath.h>

class Rope
{
private: // �G�C���A�X
// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;

public: // �����o�֐�

	bool Initialize(Keyboard* keyboard, Mouse* mouse);

	void Finalize()
	{
		delete ropeModel;
	}

	void Draw()
	{
		ropeObj->Draw();
	}

	void Update(XMFLOAT3& pPos, XMFLOAT3& ePos, const std::unique_ptr<Object3d>& object);

	void Throw(XMFLOAT3& pPos, XMFLOAT3& ePos, XMFLOAT3& rPos, XMFLOAT3& rScale, const std::unique_ptr<Object3d>& object);

	void Collision(const std::unique_ptr<Object3d>& object);

	float GetLength(XMFLOAT3 posA, XMFLOAT3 posB)
	{
		XMFLOAT3 len = { posA.x - posB.x, posA.y - posB.y, posA.z - posB.z };
		return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
	}

	// �~�^��
	void CircularMotion(XMFLOAT3& pos, const XMFLOAT3 centerPos, const float r, int& angle, const int add);

	// �ړ����Ă��邩
	const bool& GetmoveFlag() { return moveFlag; }
	void SetmoveFlag(bool moveFlag) { this->moveFlag = moveFlag; }
	// ���[�v���Ȃ����Ă��邩
	const bool& GetrFlag() { return rFlag; }
	void SetrFlag(bool rFlag) { this->rFlag = rFlag; }

private: // �����o�ϐ�

	Keyboard* keyboard = nullptr;
	Mouse* mouse = nullptr;
	Easing* easing = nullptr;

	Collision::Ray ray;

	//���[�v���f��
	Model* ropeModel = nullptr;
	std::unique_ptr<Object3d> ropeObj = nullptr;

	// �ϐ�
	XMFLOAT3 rPos = {};
	XMFLOAT3 rScale = {};
	float angleX = 0.0f; // X��
	float angleY = 0.0f; // Y��
	float vecXZ = 0.0f; // XZ���ʏ�̃x�N�g��
	float objLength = 0.0f;
	const float maxRope = 15.0f; // ���[�v�̍ő�
	bool rFlag = false; // �ڐG�t���O
	bool rThrowFlag = false; // ���[�v���΂�
	bool rBackFlag = false; // ���[�v��߂�
	XMFLOAT3 manageRopePos = {}; // ���[�v�ʒu�Ǘ��p
	XMFLOAT3 manageRopeScale = {0.2f, 0.2f, 0.2f}; // ���[�v�X�P�[���Ǘ��p
	int throwCount = 0;

	// �ːi�p
	XMFLOAT3 startPos = {}; // �J�n�ʒu
	XMFLOAT3 endPos = {}; // �I�_�ʒu
	float avoidMove = 5.0f; // ����
	float avoidTime = 0.0f; // �o�ߎ���
	bool pEaseFlag = false;
	bool eEaseFlag = false;

	// �ړ��Ǘ��t���O
	bool moveFlag = false; // ���@���ړ����Ă��邩
};


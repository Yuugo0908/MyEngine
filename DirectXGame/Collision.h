#pragma once
#include "Object3d.h"
#include <DirectXMath.h>
class Collision
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

	static Collision* GetInstance();

	// �I�u�W�F�N�g���m�̋������擾
	float GetLength(XMFLOAT3 pos_a, XMFLOAT3 pos_b);

	//�I�u�W�F�N�g���m�̓����蔻��
	bool CollisionObject(const std::unique_ptr<Object3d>& object_a, const std::unique_ptr<Object3d>& object_b);
};


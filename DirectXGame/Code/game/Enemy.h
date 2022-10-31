#pragma once
#include "Object3d.h"
#include "Keyboard.h"
#include "Easing.h"
#include "Collision.h"
#include "Operator.h"
#include "Player.h"
#include <DirectXMath.h>

class Enemy
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

	bool Initialize();

	void Finalize();

	void Update();

	void Draw();

	const std::unique_ptr<Object3d>& GetObj() { return enemyObj; }

private: // �����o�ϐ�

	Player* player = nullptr;

	// ���f��
	Model* enemyModel = nullptr;
	std::unique_ptr<Object3d> enemyObj = nullptr;

	// �G�l�~�[
	XMFLOAT3 ePos = {};
	bool eFlag = false; // ���R�����̃t���O
	float eVal = 0.2f; // ���x
	float eGra = 0.1f; // �d��
	bool eAlive = false;// �����Ă��邩�̃t���O
	int eAliveCount = 0;
	int enemyCount = 0; // �|������

	// ����Ǘ��t���O
	bool easeFlag = false;
};


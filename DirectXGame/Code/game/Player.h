#pragma once
#include "Object3d.h"
#include "Keyboard.h"
#include "Easing.h"
#include "Collision.h"
#include "Operator.h"
#include "Camera.h"
#include "Rope.h"

class Player
{
public: // �����o�֐�

	bool Initialize(Keyboard* keyboard, Mouse* mouse);

	void Finalize() { delete playerModel; }

	void Update(bool rFlag, bool moveFlag);

	void Draw() { playerObj->Draw(); }

	void Rush(bool rFlag);

	void Jump();

	// �I�u�W�F�N�g
	const std::unique_ptr<Object3d>& GetObj() { return playerObj; }
	// �ːi
	const bool& GetEaseFlag() { return easeFlag; }

	// �ʒu
	const XMFLOAT3& GetPos() { return pPos; }
	void SetPos(XMFLOAT3 pPos) { this->pPos = pPos; }

	const XMFLOAT3& GetScale() { return pScale; }

private: // �����o�ϐ�

	Keyboard* keyboard = nullptr;
	Mouse* mouse = nullptr;
	Easing* easing = nullptr;
	Rope* rope = nullptr;
	Camera* camera = Camera::GetInstance();

	// ���f��
	Model* playerModel = nullptr;
	std::unique_ptr<Object3d> playerObj = nullptr;

	// �v���C���[
	XMFLOAT3 pPos = {};//���W
	XMFLOAT3 pScale = {};
	float pSpeed = 0.35f;
	bool jumpFlag = false;//���R�����̃t���O
	bool moveFlag = false;//�ړ��Ǘ��t���O
	float pMove = 0.0f;//�ړ���
	float pAcc = 0.2f;//����
	float pVal = 0.2f;//���x
	const float pGra = 0.1f;//�d��
	float rate = 1.0f; // �΂߈ړ����̐���

	// �ːi�p
	XMFLOAT3 startPos = {}; // �J�n�ʒu
	XMFLOAT3 endPos = {}; // �I�_�ʒu
	float avoidMove = 20.0f; // ����
	float avoidTime = 0.0f; // �o�ߎ���
	bool easeFlag = false; // �C�[�W���O�J�n�t���O
	int rushCount = 0;

	// ���[�v�Ǘ��p
	bool rFlag = false;

	// �J���������擾�p
	XMFLOAT3 cameraTrack = {};
};


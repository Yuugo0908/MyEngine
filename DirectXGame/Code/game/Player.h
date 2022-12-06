#pragma once
#include "Object3d.h"
#include "Keyboard.h"
#include "Collision.h"
#include "Operator.h"
#include "Camera.h"
#include "Rope.h"
#include "Mapchip.h"

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
	const bool& GetAvoidFlag() { return avoidFlag; }
	void SetAvoidFlag(bool avoidFlag) { this->avoidFlag = avoidFlag; }
	// �W�����v
	const bool& GetOnGround() { return onGround; }
	void SetOnGround(bool onGround) { this->onGround = onGround; }

	// �ʒu
	const XMFLOAT3& GetPos() { return pPos; }
	void SetPos(XMFLOAT3 pPos) { this->pPos = pPos; }

	const XMFLOAT3& GetRot() { return pRot; }
	void SetRot(XMFLOAT3 pRot) { this->pRot = pRot; }

	const XMFLOAT3& GetScale() { return pScale; }
	void SetScale(XMFLOAT3 pScale) { this->pScale = pScale; }

private: // �����o�ϐ�

	Keyboard* keyboard = nullptr;
	Mouse* mouse = nullptr;
	Rope* rope = nullptr;
	Camera* camera = Camera::GetInstance();

	// ���f��
	Model* playerModel = nullptr;
	std::unique_ptr<Object3d> playerObj = nullptr;

	// �v���C���[
	XMFLOAT3 pPos = {};//���W
	XMFLOAT3 pScale = {};//�傫��
	XMFLOAT3 pRot = {};//��]

	float pSpeed = 0.35f;
	bool onGround = false;//���R�����̃t���O
	bool jumpFlag = false;
	bool moveFlag = false;//�ړ��Ǘ��t���O
	float pMove = 0.0f;//�ړ���
	float pAcc = 0.2f;//����
	float pVal = 0.2f;//���x
	float pDown = 0.0f;
	float pGra = 0.1f;//�d��
	float rate = 1.0f; // �΂߈ړ����̐���

	// �ːi�p
	bool avoidFlag = false; // ����J�n�t���O
	int avoidCount = 0;

	// ���[�v�Ǘ��p
	bool rFlag = false;

	// �J���������擾�p
	XMFLOAT3 cameraTrack = {};
	float cameraRot = 0.0f;
};


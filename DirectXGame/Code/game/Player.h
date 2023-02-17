#pragma once
#include "Object3d.h"
#include "Keyboard.h"
#include "Controller.h"
#include "Collision.h"
#include "Easing.h"
#include "Operator.h"
#include "Camera.h"

class Player
{
public: // �����o�֐�

	bool Initialize();

	void Finalize() { delete playerModel; }

	void Update(bool rFlag, bool moveFlag);

	void Draw() { playerObj->Draw(); }

	void Attack(XMFLOAT3 targetPos, bool& flag, float& avoidTime);

	void Rush(bool rFlag);

	void Jump();

	void Reset();

	// �}�b�v�`�b�v�����蔻��
	bool MapCollide(XMFLOAT3 boxPos, XMFLOAT3 boxScale);

	bool StageCollide(XMFLOAT3 stagePos, XMFLOAT3 stageScale);

	// �I�u�W�F�N�g
	const std::unique_ptr<Object3d>& GetObj() { return playerObj; }
	// ���
	const bool& GetAvoidFlag() { return avoidFlag; }
	void SetAvoidFlag(bool avoidFlag) { this->avoidFlag = avoidFlag; }
	// ���n
	const bool& GetOnGround() { return onGround; }
	void SetOnGround(bool onGround) { this->onGround = onGround; }

private: // �����o�ϐ�

	Controller* controller = nullptr;
	Keyboard* keyboard = nullptr;
	Mouse* mouse = nullptr;
	Camera* camera = Camera::GetInstance();

	// ���f��
	Model* playerModel = nullptr;
	std::unique_ptr<Object3d> playerObj = nullptr;

	// �v���C���[
	XMFLOAT3 pPos = {};//���W
	XMFLOAT3 pPosOld = {};
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

	// �J���������擾�p
	XMFLOAT3 cameraTrack = {};
	float cameraRot = 0.0f;
};


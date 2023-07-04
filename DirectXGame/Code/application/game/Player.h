#pragma once
#include "Object3d.h"
#include "Keyboard.h"
#include "Controller.h"
#include "Collision.h"
#include "Easing.h"
#include "Operator.h"
#include "Camera.h"
#include "Particle.h"
#include "DirectXCommon.h"
#include "Rope.h"

class Player
{
private:
	Player();

	~Player();

public:
	// �R�s�[�R���X�g���N�^�𖳌���
	Player(const Player& obj) = delete;
	// ������Z�q�𖳌���
	Player& operator=(const Player& obj) = delete;
public: // �����o�֐�

	static Player* GetInstance();

	bool Initialize(const XMFLOAT3 pos, const XMFLOAT3 scale);

	void Update();

	void Rush(XMFLOAT3 targetPos, bool& flag, float& avoidTime);

	void Avoid();

	void Jump();

	bool Damage(const std::unique_ptr<Object3d>& object);

	void Reset();

	void ReSpawn();

	void TrackRot(const XMFLOAT3& startPos, const XMFLOAT3& endPos);

	void CameraTarget() { camera->SetTarget(pPos); }

	// �}�b�v�����蔻��
	bool MapCollide(XMFLOAT3 boxPos, XMFLOAT3 boxScale);

	bool StageCollide(XMFLOAT3 stagePos, XMFLOAT3 stageScale, bool& reverseFlag);

	bool PoleCollide(XMFLOAT3 polePos, XMFLOAT3 poleScale);

	// �I�u�W�F�N�g
	const std::unique_ptr<Object3d>& GetObj() { return playerObj; }
	// ���
	const bool& GetAvoidFlag() { return avoidFlag; }
	void SetAvoidFlag(bool avoidFlag) { this->avoidFlag = avoidFlag; }
	// ���n
	const bool& GetOnGround() { return onGround; }
	void SetOnGround(bool onGround) { this->onGround = onGround; }

	// �W�����v
	void SetJumpFlag(bool jumpFlag) { this->jumpFlag = jumpFlag; }

private: // �����o�ϐ�

	Keyboard* keyboard = Keyboard::GetInstance();
	Mouse* mouse = Mouse::GetInstance();
	Controller* controller = Controller::GetInstance();
	Camera* camera = Camera::GetInstance();

	// ���f��
	Model* playerModel = nullptr;
	std::unique_ptr<Object3d> playerObj = nullptr;

	// �v���C���[
	XMFLOAT3 pPos = {};//���W
	XMFLOAT3 pPosOld = {};
	XMFLOAT3 pScale = {};//�傫��
	XMFLOAT3 pRot = {};//��]
	XMFLOAT3 inertiaSaveJump = {}; // �W�����v����
	XMFLOAT3 inertiaSaveMove = {};
	XMFLOAT3 reSpawnPos = {};

	float pSpeed = 0.35f;
	bool onGround = false;//���R�����̃t���O
	bool jumpFlag = false;
	bool moveFlag = false; // �ړ��t���O
	bool rushFlag = false; // �ːi�t���O
	float pMove = 0.0f;//�ړ���
	float pAcc = 0.2f;//����
	float pVel = 0.2f;//���x
	float pDown = 0.0f;
	float pGra = 0.1f;//�d��
	float rate = 1.0f; // �΂߈ړ����̐���
	int damageInterval = 0; // �U�����󂯂��ۂ̃C���^�[�o��
	float trackTime = 0.0f;

	// �ːi�p
	bool avoidFlag = false; // ����J�n�t���O
	bool firstAvoidFlag = true;
	int avoidCount = 0;

	// �J���������擾�p
	XMFLOAT3 cameraTrack = {};
	float cameraRot = 0.0f;
};

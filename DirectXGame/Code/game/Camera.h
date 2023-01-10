#pragma once
#include "Mouse.h"
#include "Operator.h"
#include "DebugText.h"
class Camera
{
private:
	// �J����������
	Camera();

	~Camera();

public:
	// �R�s�[�R���X�g���N�^�𖳌���
	Camera(const Camera& obj) = delete;
	// ������Z�q�𖳌���
	Camera& operator=(const Camera& obj) = delete;

public: // �ÓI�����o�֐�

	static Camera* GetInstance();
	// ������
	bool Initialize(const int window_width, const int window_height, Mouse* mouse);
	// ���_���W�̎擾
	const XMFLOAT3& GetEye() { return eye; }
	// ���_���W�̐ݒ�
	void SetEye(XMFLOAT3 eye);
	// �����_���W�̎擾
	const XMFLOAT3& GetTarget() { return target; }
	// �����_���W�̐ݒ�
	void SetTarget(XMFLOAT3 target);
	// ������x�N�g���̎擾
	const XMFLOAT3& GetUp() { return up; }
	// ������x�N�g���̐ݒ�
	void SetUp(XMFLOAT3 up);

	// �r���[�s��̎擾
	const XMMATRIX& GetMatView() {
		return matView;
	}

	// �ˉe�s��̎擾
	const XMMATRIX& GetMatProjection() {
		return matProjection;
	}

	// �r���[�ˉe�s��̎擾
	const XMMATRIX& GetMatViewProjection() {
		return matViewProjection;
	}

	// �x�N�g���ɂ��ړ�
	void CameraMoveVector(XMFLOAT3 move);
	void CameraMoveEyeVector(XMFLOAT3 move);
	void CameraMoveVector(const XMVECTOR& move);
	void CameraMoveEyeVector(const XMVECTOR& move);

	// ���t���[���X�V
	void Update();
	// �r���[�s����X�V
	void UpdateViewMatrix();
	// �ˉe�s����X�V
	void UpdateProjectionMatrix();
	// �J�����̃V�F�C�N�����s
	void CameraShake(bool& flag);
	// �J�����̒ǔ�
	XMFLOAT3 CameraTrack(XMFLOAT3 pPos);
	float CameraRot(XMFLOAT3 pPos);

	void Reset();


private: // �����o�ϐ�
	// ���̓N���X�̃|�C���^
	Mouse* mouse;
	// �r���[�s��
	XMMATRIX matView = DirectX::XMMatrixIdentity();
	// �ˉe�s��
	XMMATRIX matProjection = DirectX::XMMatrixIdentity();
	// �r���[�ˉe�s��
	XMMATRIX matViewProjection = DirectX::XMMatrixIdentity();
	// ��]�s��
	XMMATRIX matRot = DirectX::XMMatrixIdentity();
	//�ΏۂƃJ�����̋���
	float distance = 15.0f;
	// �X�P�[�����O
	float scaleX = 1.0f;
	float scaleY = 1.0f;
	// ���_���W
	XMFLOAT3 eye = { 0, 0, distance };
	// �����_���W
	XMFLOAT3 target = { 0, 0, 0 };
	// ������x�N�g��
	XMFLOAT3 up = { 0, 1, 0 };
	// �J�����ƑΏە��܂ł̋���
	XMVECTOR cameraLength = {};
	// �r���[�s��_�[�e�B�t���O
	bool viewDirty = false;
	// �ˉe�s��_�[�e�B�t���O
	bool projectionDirty = false;
	// �A�X�y�N�g��
	float aspectRatio = 1.0f;
	// �V�F�C�N���W
	XMFLOAT3 shake = {};
	// �V�F�C�N�Ǘ��J�E���^�[
	int shakeCount = 0;
	// �J�����̓���Ǘ��p�J�E���^�[
	int moveCount = 30;
	// �����_���王�_�ւ̃x�N�g���ƁA������x�N�g��
	XMVECTOR vTargetEye = {};
	XMVECTOR vUp = {};

	Mouse::MouseMove mouseMove;

	bool dirty = false;
	float dy = 0.0f;
	float angleX = 0;
	float angleY = 0;
	float speed = 0.75f;
};
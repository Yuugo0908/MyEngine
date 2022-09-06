#pragma once
#include <DirectXMath.h>
class Camera
{
protected: // �G�C���A�X
// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public: // �ÓI�����o�֐�

	// �J����������
	Camera(const int window_width, const int window_height);
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

	/// ���t���[���X�V
	void Update();
	/// �r���[�s����X�V
	void UpdateViewMatrix();
	/// �ˉe�s����X�V
	void UpdateProjectionMatrix();


public: // �����o�ϐ�
	// �r���[�s��
	XMMATRIX matView = DirectX::XMMatrixIdentity();
	// �ˉe�s��
	XMMATRIX matProjection = DirectX::XMMatrixIdentity();
	// �r���[�ˉe�s��
	XMMATRIX matViewProjection = DirectX::XMMatrixIdentity();
	//�ΏۂƃJ�����̋���
	float distance = 20.0f;
	// ���_���W
	XMFLOAT3 eye = { 0, 0, distance };
	// �����_���W
	XMFLOAT3 target = { 0, 0, 0 };
	// ������x�N�g��
	XMFLOAT3 up = { 0, 1, 0 };
	// �r���[�s��_�[�e�B�t���O
	bool viewDirty = false;
	// �ˉe�s��_�[�e�B�t���O
	bool projectionDirty = false;
	// �A�X�y�N�g��
	float aspectRatio = 1.0f;
};
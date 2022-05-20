#pragma once

#include "FbxModel.h"
#include "Camera.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <string>
#include <DirectXMath.h>

class FbxObject3d
{
protected: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public: // �ÓI�����o�֐�
	// setter
	static void SetDevice(ID3D12Device* device){FbxObject3d::device = device;}
	static void SetCamera(Camera* camera) { FbxObject3d::camera = camera; }
private:
	// �f�o�C�X
	static ID3D12Device* device;
	// �J����
	static Camera* camera;
public: // �T�u�N���X
	// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferDataTransform
	{
		XMMATRIX viewproj;	// �r���[�v���W�F�N�V�����s��
		XMMATRIX world;		// ���[���h�s��
		XMFLOAT3 cameraPos;	// �J�������W�i���[���h���W�j
	};
public: // �����o�֐�
	/// <summary>
	/// ������
	/// </summary>
	void Initialize();
<<<<<<< HEAD
protected: // �����o�ϐ�
=======
	/// <summary>
	/// �O���t�B�b�N�X�p�C�v���C���̐���
	/// </summary>
	static void CreateGraphicsPipeline();
	/// <summary>
	/// ���t���[������
	/// </summary>
	void Update();
	/// <summary>
	/// ���f���̃Z�b�g
	/// </summary>
	/// <param name="model">���f��</param>
	void SetModel(FbxModel* fbxModel) { this->fbxModel = fbxModel; }
	/// <summary>
	/// �`��
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	// ���W�̐ݒ�
	void SetPosition(XMFLOAT3 position) { this->position = position; }
private: // �����o�ϐ�
>>>>>>> Error
	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> constBufferTransform;
};


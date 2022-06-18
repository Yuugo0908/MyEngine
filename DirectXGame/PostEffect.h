#pragma once
#include "Sprite.h"
class PostEffect : public Sprite
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

private: // �ÓI�����o�ϐ�
	// ��ʃN���A�J���[
	static const float clearColor[4];
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	PostEffect();

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �e�N�X�`������
	/// </summary>
	void CreateTex();

	/// <summary>
	/// �V�F�[�_�[���\�[�X�r���[�쐬
	/// </summary>
	void CreateSRV();

	/// <summary>
	/// �����_�[�^�[�Q�b�g�r���[����
	/// </summary>
	void CreateRTV();

	/// <summary>
	/// �[�x�o�b�t�@����
	/// </summary>
	void CreateDepth();

	/// <summary>
	/// �f�v�X�X�e���V���r���[����
	/// </summary>
	void CreateDSV();

	/// <summary>
	/// �`��R�}���h�̔��s
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �`��O����
	/// </summary>
	/// <param name="cmdList"></param>
	void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �`��㏈��
	/// </summary>
	/// <param name="cmdList"></param>
	void PostDraw(ID3D12GraphicsCommandList* cmdList);

private: // �����o�ϐ�
	// �e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texBuff;
	// �[�x�o�b�t�@
	ComPtr<ID3D12Resource> depthBuff;
	// SRV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	// RTV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	// DSV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
};


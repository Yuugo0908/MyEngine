#pragma once
#include <string>
#include <DirectXMath.h>
#include <vector>
#include <DirectXTex.h>

// DirectX::���ȗ�
using XMFLOAT2 = DirectX::XMFLOAT2;
using XMFLOAT3 = DirectX::XMFLOAT3;
using XMFLOAT4 = DirectX::XMFLOAT4;
using XMMATRIX = DirectX::XMMATRIX;
using XMVECTOR = DirectX::XMVECTOR;
using TexMetadata = DirectX::TexMetadata;
using ScratchImage = DirectX::ScratchImage;

struct Node
{
	// ���O
	std::string name;
	// ���[�J���X�P�[��
	XMVECTOR scaling = { 1, 1, 1, 0 };
	// ���[�J����]�p
	XMVECTOR rotation = { 0, 0, 0, 0 };
	// ���[�J���ړ�
	XMVECTOR translation = { 0, 0, 0, 1 };
	// ���[�J���ό`�s��
	XMMATRIX transform;
	// �O���[�o���ό`�s��
	XMMATRIX globalTransform;
	// �e�m�[�h
	Node* parent = nullptr;
};

class FbxModel
{
public:
	// �t�����h�N���X
	friend class FbxLoader;
public:// �T�u�N���X
	// ���_�f�[�^�\����
	struct VertexPosNormalUv
	{
		XMFLOAT3 pos;	// xyz���W
		XMFLOAT3 normal;// �@���x�N�g��
		XMFLOAT2 uv;	// uv���W
	};
private:
	// ���f����
	std::string name;
	// �m�[�h�z��
	std::vector<Node> nodes;
	// ���b�V�������m�[�h
	Node* meshNode = nullptr;
	// ���_�f�[�^�z��
	std::vector<VertexPosNormalUv> vertices;
	// ���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices;
	// �A���r�G���g�W��
	XMFLOAT3 ambient = { 1, 1, 1 };
	// �f�B�t���[�Y�W��
	XMFLOAT3 diffuse = { 1, 1, 1 };
	// �e�N�X�`�����^�f�[�^
	TexMetadata metadata = {};
	// �X�N���b�`�C���[�W
	ScratchImage scratchImg = {};
};
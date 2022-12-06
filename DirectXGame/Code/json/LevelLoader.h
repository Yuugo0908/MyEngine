#pragma once
#include "Operator.h"
#include "Collision.h"
#include <string>
#include <vector>
#include <json.hpp>

struct LevelData
{
	struct ObjectData
	{
		// �t�@�C����
		std::string fileName;
		// ���s�ړ�
		XMVECTOR trans;
		// ��]
		XMVECTOR rot;
		// �X�P�[��
		XMVECTOR scale;
		// �R���C�_�[���S�_
		XMVECTOR center;
		// �R���C�_�[�傫��
		XMVECTOR size;
	};
	// �I�u�W�F�N�g�z��
	std::vector<ObjectData> objects;

	// ���f�����X�g
	Model* stageModel = nullptr;
	Model* skydomeModel = nullptr; 
	std::map<std::string, Model*> models;
};

class LevelLoader
{
public:
	// �f�B���N�g��
	static const std::string baseDirectory;
	// �g���q
	static const std::string Extension;
public:
	// �t�@�C���ǂݍ���
	static LevelData* LoadFile(const std::string& fileName);

	static void Recursive(nlohmann::json& object, LevelData* levelData);

	static void SetCollision();
};
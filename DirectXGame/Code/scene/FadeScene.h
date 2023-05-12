#pragma once
#include "DirectXCommon.h"
#include "Image2d.h"
#include "Object3d.h"
#include "BaseScene.h"
#include "SafeDelete.h"
#include "Easing.h"
#include "LevelLoader.h"

enum FadeState
{
	None,
	FadeInPlay,
	FadeInEnd,
	FadeOutPlay,
	FadeOutEnd,
};

class FadeScene
{
private:

	FadeScene();

	~FadeScene();

public:
	// �R�s�[�R���X�g���N�^�𖳌���
	FadeScene(const FadeScene& obj) = delete;
	// ������Z�q�𖳌���
	FadeScene& operator=(const FadeScene& obj) = delete;

public: // �ÓI�����o�֐�
	static FadeScene* GetInstance();

public: // �����o�֐�
	// ������
	void Initialize();
	// �I��
	void Finalize();
	// ���t���[������
	void Update();
	// �`��
	void Draw();

	// json�I�u�W�F�N�g�̏�����
	void jsonObjectInit(const std::string sceneName);

	// �t�F�[�h�C��
	void FadeIn(float alpha);
	// �t�F�[�h�A�E�g
	void FadeOut(float alpha);

	void reset();

	// �t�F�[�h�t���O�̃Q�b�^�[
	const bool& GetFadeInEnd() { return fadeInEnd; }
	const bool& GetFadeOutEnd() { return fadeOutEnd; }

private: // �����o�ϐ�

	Camera* camera = Camera::GetInstance();
	Light* light = nullptr;

	// ���x���f�[�^
	LevelData* levelData = nullptr;
	// ���x���f�[�^�œǂݍ��񂾃I�u�W�F�N�g
	std::vector<std::unique_ptr<Object3d>> jsonObject{};

	enum Image2dNum
	{
		fadeNum = 10,
	};

	FadeState fadeState;
	Image2d* fadeTex = nullptr;

	float alpha = 0.0f;

	bool fadeInEnd = false;
	bool fadeOutEnd = false;
};


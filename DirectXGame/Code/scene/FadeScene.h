#pragma once
#include "DirectXCommon.h"
#include "Image2d.h"
#include "Object3d.h"
#include "BaseScene.h"
#include "SafeDelete.h"
#include "Easing.h"

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
	// ���t���[������
	void Update();
	// �`��
	void Draw();
	// �t�F�[�h�C��
	void FadeIn(float alpha);
	// �t�F�[�h�A�E�g
	void FadeOut(float alpha);

	void reset();

	// �t�F�[�h�t���O�̃Q�b�^�[
	const bool& GetFadeInEnd() { return fadeInEnd; }
	const bool& GetFadeOutEnd() { return fadeOutEnd; }

private: // �����o�ϐ�
	Image2d* fade = nullptr;
	std::vector<Image2d*> fades;

	FadeState fadeState;

	float alpha = 0.0f;
	bool fadeInEnd = false;
	bool fadeOutEnd = false;
	int fadeNum = 10;
};


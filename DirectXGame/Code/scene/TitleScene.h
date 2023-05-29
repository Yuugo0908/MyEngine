#pragma once

#include "DirectXCommon.h"
#include "Keyboard.h"
#include "Controller.h"
#include "Mouse.h"
#include "Image2d.h"
#include "DebugText.h"
#include "Object3d.h"
#include "Audio.h"
#include "BaseScene.h"
#include "SafeDelete.h"
#include "Easing.h"
#include "LevelLoader.h"
#include "FadeScene.h"

class TitleScene : public BaseScene
{
public: // �����o�֐�
	// ������
	void Initialize() override;
	// �I��
	void Finalize() override;
	// ���t���[������
	void Update() override;
	// �`��
	void Draw() override;

	// json�I�u�W�F�N�g�̏�����
	void jsonObjectInit(const std::string sceneName);
	// json�I�u�W�F�N�g�̍X�V
	void jsonObjectUpdate();

private: // �����o�ϐ�

	Keyboard* keyboard = Keyboard::GetInstance();
	Controller* controller = Controller::GetInstance();
	Mouse* mouse = Mouse::GetInstance();
	Audio* audio = Audio::GetInstance();
	Camera* camera = Camera::GetInstance();

	Light* light = nullptr;

	enum Image2dNum
	{
		titleNum = 1, backNum
	};

	// ���x���f�[�^
	LevelData* levelData = nullptr;
	// ���x���f�[�^�œǂݍ��񂾃I�u�W�F�N�g
	std::vector<std::unique_ptr<Object3d>> jsonObject{};

	// �摜
	Image2d* title = nullptr;

	float alpha = 0.0f;
	bool fadeFlag = false;
};
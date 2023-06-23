#pragma once

#include "DirectXCommon.h"
#include "Keyboard.h"
#include "Controller.h"
#include "Mouse.h"
#include "Image2d.h"
#include "DebugText.h"
#include "Object3d.h"
#include "Camera.h"
#include "Audio.h"
#include "Light.h"
#include "BaseScene.h"
#include "SafeDelete.h"
#include "FadeScene.h"

class GameOverScene : public BaseScene
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

private: // �����o�ϐ�
	Keyboard* keyboard = Keyboard::GetInstance();
	Controller* controller = Controller::GetInstance();
	Mouse* mouse = Mouse::GetInstance();
	Audio* audio = Audio::GetInstance();
	Camera* camera = Camera::GetInstance();

	// �摜
	Image2d* GameOver = nullptr;
	Image2d* backGround = nullptr;

	bool retryFlag = false;
	bool titleFlag = false;
};


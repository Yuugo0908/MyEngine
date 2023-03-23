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

#include <SafeDelete.h>
class GameClearScene : public BaseScene
{
private: // �ÓI�����o�ϐ�
	static const int debugTextTexNumber = 0;

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

	enum Image2dNum
	{
		GameClearNum, backNum, fadeNum,
	};

	// �摜
	Image2d* GameClear = nullptr;
	Image2d* backGround = nullptr;
	Image2d* fadeTex = nullptr;

	float alpha = 1.0f;
	bool fadeFlag = false;
};


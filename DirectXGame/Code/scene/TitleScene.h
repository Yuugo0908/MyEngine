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

	float GetLength(XMFLOAT3 posA, XMFLOAT3 posB)
	{
		XMFLOAT3 len = { posA.x - posB.x, posA.y - posB.y, posA.z - posB.z };
		return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
	}

private: // �����o�ϐ�

	Keyboard* keyboard = Keyboard::GetInstance();
	Controller* controller = Controller::GetInstance();
	Mouse* mouse = Mouse::GetInstance();
	Audio* audio = Audio::GetInstance();
	Camera* camera = Camera::GetInstance();

	enum Image2dNum
	{
		titleNum = 1, backNum, expNum, fadeNum,
	};

	// �摜
	Image2d* title = nullptr;
	Image2d* fadeTex = nullptr;
	Image2d* explanation = nullptr;
	Image2d* backGround = nullptr;

	float alpha = 0.0f;
	bool fadeIn = false;
	bool fadeOut = false;
	bool expFlag = false;
};
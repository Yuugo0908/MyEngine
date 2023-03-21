#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Audio.h"
#include "GameScene.h"
#include "Controller.h"
#include "Light.h"
#include "Mouse.h"

// �Q�[���S��
class MyGame
{
public:

	// ������
	void Initialize();
	// �I��
	void Finalize();
	// ���t���[���X�V
	void Update();
	// �`��
	void Draw();
	// �I�����N�G�X�g
	bool IsEndRequest();

	//�|�C���^�u����
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	Keyboard* keyboard = nullptr;
	Mouse* mouse = nullptr;
	Audio* audio = nullptr;
	GameScene* gameScene = nullptr;
	Camera* camera = nullptr;
	Image2d* image2d = nullptr;
	Controller* controller = nullptr;
	Light* light = nullptr;
};
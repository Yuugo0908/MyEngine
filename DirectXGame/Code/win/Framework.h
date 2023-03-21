#pragma once
#include "DirectXCommon.h"
#include "Audio.h"
#include "GameScene.h"
#include "Controller.h"
#include "Light.h"
#include "Mouse.h"

class Framework
{
public: // �����o�֐�

	virtual ~Framework() = default;

	// ������
	virtual void Initialize();

	// �I��
	virtual void Finalize();

	// ���t���[���X�V
	virtual void Update();

	// �`��
	virtual void Draw();

	// �I���`�F�b�N
	virtual bool IsEndRequest() { return endRequest_; }

	// ���s
	void Run();

private: // �����o�ϐ�
	bool endRequest_ = false;
	//�|�C���^�u����
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	Keyboard* keyboard = nullptr;
	Mouse* mouse = nullptr;
	Controller* controller = nullptr;
	Audio* audio = nullptr;
	GameScene* gameScene = nullptr;
	Camera* camera = nullptr;
	Image2d* image2d = nullptr;
	Light* light = nullptr;
};


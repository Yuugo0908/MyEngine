#pragma once

#include "DirectXCommon.h"
#include "Keyboard.h"
#include "Controller.h"
#include "Mouse.h"
#include "Image2d.h"
#include "DebugText.h"
#include "Object3d.h"
#include "Model.h"
#include "Operator.h"

#include "Camera.h"
#include "Audio.h"
#include "Rope.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Item.h"
#include "Collision.h"
#include "Light.h"
#include "Easing.h"
#include "LevelLoader.h"

#include <SafeDelete.h>
#include <stdlib.h>
#include <time.h>

class TitleScene {
public: // �����o�֐�

	// �R���X�g�N���^
	TitleScene();
	// �f�X�g���N�^
	~TitleScene();
	// ������
	void Initialize();
	// ���
	void Finalize();
	// ���t���[������
	void Update();
	// �`��
	void Draw();

private: // �����o�ϐ�
	DirectXCommon* dxCommon = nullptr;
	Keyboard* keyboard = Keyboard::GetInstance();
	Audio* audio = nullptr;

	// �摜
	Image2d* title = nullptr;
	Image2d* backGround = nullptr;
	float alpha = 0.0f;
	Image2d* explanation = nullptr;

	// �V�[���Ǘ��p
	bool fadeIn = false;
	bool fadeOut = false;
	bool expFlag = false;

	// �V�F�C�N�p
	bool shakeFlag = false;

	bool attackFlag = false; // �ːi�J�n�t���O
	float avoidTime = 0.0f;

	int check = 0;
};

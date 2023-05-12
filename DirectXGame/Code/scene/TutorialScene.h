#pragma once

#include "DirectXCommon.h"
#include "Keyboard.h"
#include "Controller.h"
#include "Mouse.h"
#include "Image2d.h"
#include "DebugText.h"
#include "Object3d.h"
#include "PostEffect.h"
#include "Model.h"
#include "Operator.h"
#include "Camera.h"
#include "Audio.h"
#include "Rope.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Collision.h"
#include "Light.h"
#include "Easing.h"
#include "LevelLoader.h"
#include "Particle.h"
#include "BaseScene.h"
#include "SafeDelete.h"
#include "FadeScene.h"

#include <stdlib.h>
#include <time.h>
#include <list>

class TutorialScene : public BaseScene
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

	//pos������
	void reset();
	// Imgui�̐ݒ�
	void SetImgui();
	// ���C�g�̍X�V
	void LightUpdate();
	// �J�����̍X�V
	void CameraUpdate();
	// �����蔻��
	void CollisionUpdate();
	// ���[�v�̍X�V
	void RopeUpdate();

	// json�I�u�W�F�N�g�̏�����
	void jsonObjectInit(const std::string sceneName);
	// json�I�u�W�F�N�g�̍X�V
	void jsonObjectUpdate();

	float GetLength(XMFLOAT3 posA, XMFLOAT3 posB)
	{
		XMFLOAT3 len = { posA.x - posB.x, posA.y - posB.y, posA.z - posB.z };
		return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
	}

private: // �����o�ϐ�
	Keyboard* keyboard = Keyboard::GetInstance();
	Controller* controller = Controller::GetInstance();
	Mouse* mouse = Mouse::GetInstance();
	Camera* camera = Camera::GetInstance();
	Light* light = nullptr;
	Rope* rope = nullptr;
	Player* player = nullptr;
	Enemy* enemy = nullptr;

	// �G�̕��������p���X�g
	std::list<std::unique_ptr<Enemy>> enemys;

	enum Scene
	{
		title_, tutorial_, game_, clear_, failure_
	};

	enum Image2dNum
	{
		// 0�Ԃ̓f�o�b�O�e�L�X�g�Ȃ̂ŏ��O
		HPTextNum = 1, HPBarNum, HPGaugeNum, wasdNum, spaceNum, mouseNum, mouseLeftNum, mouseRightNum
	};

	// ���x���f�[�^
	LevelData* levelData = nullptr;
	// ���x���f�[�^�œǂݍ��񂾃I�u�W�F�N�g
	std::vector<std::unique_ptr<Object3d>> jsonObject{};

	// �摜
	Image2d* HPText = nullptr;
	Image2d* PlayerHPBar = nullptr;
	Image2d* PlayerHPGauge = nullptr;
	Image2d* wasdKey = nullptr;
	Image2d* spaceKey = nullptr;
	Image2d* mouseImg = nullptr;
	Image2d* mouseLeftImg = nullptr;
	Image2d* mouseRightImg = nullptr;
	float alpha = 1.0f;

	// ����
	Audio* Bgm = Audio::GetInstance();

	// �p�[�e�B�N��
	Particle* effectBox = nullptr;
	Particle* effectCircle = nullptr;
	Particle* effectCircle2 = nullptr;
	Particle* effectTarget = nullptr;
	Particle* effectAvoid = nullptr;
	int targetEffectCount = 0;

	// �v���C���[
	XMFLOAT3 pPos = {};//���W
	XMFLOAT3 pPosOld = {};
	XMFLOAT3 pScale = {};
	bool onGround = false;//���R�����̃t���O
	bool moveFlag = false;//�ړ��Ǘ��t���O
	bool avoidFlag = false;//����Ǘ��t���O
	bool playerAttackFlag = false;
	float playerHp = 360;

	// �G�l�~�[
	int enemyCount = 0;
	bool enemyAlive = false;
	bool enemyAttackFlag = false;
	XMFLOAT3 ePos = {};
	XMFLOAT3 ePosOld = {};
	XMFLOAT3 eScale = {};

	// ���[�v
	bool rFlag = false;
	XMFLOAT3 catchPos = {};

	// �J����
	XMFLOAT3 cPos = {};
	XMFLOAT3 cTarget = {};
	const float trackLimit = -30.0f;
	float easeTime = 0.0f;

	// �V�[���Ǘ��p
	bool fadeFlag = false;
	bool gameClearFlag = false;
	bool gameOverFlag = false;

	// �V�F�C�N�p
	bool shakeFlag = false;
	bool rushFlag = false; // �ːi�J�n�t���O
	float elapsedTime = 0.0f;

	// �R���g���[���[
	// �U���p�ϐ�
	int vSpeedL = 0;
	int vSpeedR = 0;

	// �^�[�Q�b�g���W�ۑ��p
	XMFLOAT3 posSave = {};
	// �ߋ��Ƀ^�[�Q�b�g�����I�u�W�F�N�g�̍��W
	XMFLOAT3 oldPosSave = { 1000.0f, 1000.0f, 1000.0f };
	// �^�[�Q�b�g���鋗���̊
	const float baseLength = 15.0f;
	bool targetFlag = false;

	// �|�[���̍��W�Ƌ���
	XMFLOAT3 posPoleSave = {};
	float minPoleLength = 15.0f;

	// �G�l�~�[�̍��W�Ƌ���
	XMFLOAT3 posEnemySave = {};
	float minEnemyLength = 15.0f;

	// �`���[�g���A���p
	bool firstAvoidFlag = true;
	bool firstThrowFlag = true;
	bool firstAttackFlag = false;
	int imgShowCount = 0;
};

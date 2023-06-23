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
	void Reset();
	// Imgui�̐ݒ�
	void SetImgui();
	// ���C�g�̍X�V
	void LightUpdate();
	// �J�����̍X�V
	void CameraUpdate();
	// �G�̍X�V
	void EnemyUpdate();
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
	Light* light = Light::GetInstance();
	Rope* rope = nullptr;
	Player* player = nullptr;
	Enemy* enemy = nullptr;

	// �G�̕��������p���X�g
	std::list<std::unique_ptr<Enemy>> enemies;

	enum Scene
	{
		title_, tutorial_, game_, clear_, failure_
	};

	enum TutorialState
	{
		None, Throw, Avoid, Attack
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
	bool avoidFlag = false;//����Ǘ��t���O
	float playerHp = 360;

	// �G�l�~�[
	XMFLOAT3 ePos = {};
	XMFLOAT3 ePosOld = {};
	XMFLOAT3 eScale = {};
	int enemyCount = 0;
	bool getEnemyAlive = false;

	// ���[�v
	XMFLOAT3 catchPos = {};
	bool rFlag = false;
	bool rushFlag = false; // �ːi�J�n�t���O
	float elapsedTime = 0.0f;

	// �J����
	XMFLOAT3 cPos = {};
	XMFLOAT3 cTarget = {};
	const float trackLimit = -30.0f;

	// �V�[���Ǘ��p
	bool gameClearFlag = false;
	bool gameOverFlag = false;

	// �V�F�C�N�p
	bool shakeFlag = false;

	// �R���g���[���[
	// �U���p�ϐ�
	int vSpeedL = 0;
	int vSpeedR = 0;

	// �^�[�Q�b�g���W�ۑ��p
	XMFLOAT3 targetPos= {};
	// �^�[�Q�b�g�����ۑ��p
	float targetLength = FLT_MAX;
	// �ߋ��Ƀ^�[�Q�b�g�����I�u�W�F�N�g�̍��W
	XMFLOAT3 oldTargetPos = { FLT_MAX, FLT_MAX, FLT_MAX };
	// �^�[�Q�b�g���鋗���̊
	const float baseLength = 15.0f;

	// �|�[���̍��W�Ƌ���
	XMFLOAT3 targetPolePos = {};
	float minPoleLength = 15.0f;

	// �G�l�~�[�̍��W�Ƌ���
	XMFLOAT3 targetEnemyPos = {};
	float minEnemyLength = 15.0f;

	// �`���[�g���A���p
	TutorialState tutorialState = Throw; // �`���[�g���A��
	bool tutorialThrow = false;
	bool tutorialAvoid = false;
	bool tutorialAttack = false;
	int imgShowCount = 0; // �`���[�g���A���p�摜�̓_�ŃJ�E���g
};


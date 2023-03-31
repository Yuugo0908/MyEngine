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
#include "Collision.h"
#include "Light.h"
#include "Easing.h"
#include "LevelLoader.h"
#include "Particle.h"
#include "BaseScene.h"
#include "SafeDelete.h"

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

	// json�I�u�W�F�N�g�̏�����
	void jsonObjectInit(const std::string sceneName);
	// json�I�u�W�F�N�g�̍X�V
	void jsonObjectUpdate();

	void CreateParticles(XMFLOAT3 setPos, float startScale, float endScale, XMFLOAT4 startColor, XMFLOAT4 endColor, int count);

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
	Light* light = nullptr;

	Rope* rope = nullptr;
	Player* player = nullptr;
	Enemy* enemy = nullptr;
	// ���������p���X�g
	std::list<std::unique_ptr<Enemy>> enemys;
	LevelData* levelData = nullptr;

	enum Scene
	{
		title_, tutorial_, game_, clear_, failure_
	};

	enum ObjectType
	{
		sphere_, box_, stage_, wall_, pole_, skydome_,
	};

	enum Image2dNum
	{
		HPTextNum = 1, HPBarNum, HPGaugeNum, fadeNum,
	};

	// json�I�u�W�F�N�g
	std::vector<std::unique_ptr<Object3d>> jsonObject{};
	int objectType = 0;
	bool layHit = false;
	XMFLOAT3 boxPos = {};
	XMFLOAT3 boxScale = {};

	XMFLOAT3 polePos = {};
	XMFLOAT3 poleScale = {};
	float poleLength = 15.0f;

	// �摜
	Image2d* HPText = nullptr;
	Image2d* PlayerHPBar = nullptr;
	Image2d* PlayerHPGauge = nullptr;
	Image2d* fadeTex = nullptr;
	float alpha = 1.0f;

	// �p�[�e�B�N��
	Particle* box_effect = nullptr;

	// �v���C���[
	XMFLOAT3 pPos = {};//���W
	XMFLOAT3 pPosOld = {};
	XMFLOAT3 pScale = {};
	bool onGround = false;//���R�����̃t���O
	bool moveFlag = false;//�ړ��Ǘ��t���O
	bool avoidFlag = false;//����Ǘ��t���O
	float playerHp = 360;

	// �G�l�~�[
	int enemyCount = 0;
	float enemyHp = 360;
	bool eAlive = false;
	XMFLOAT3 ePos = {};
	XMFLOAT3 ePosOld = {};
	XMFLOAT3 eScale = {};

	// ���[�v
	bool rFlag = false;
	int throwCount = 0;
	XMFLOAT3 catchPos = {};

	// �J����
	XMFLOAT3 cPos = {};
	XMFLOAT3 cTarget = {};
	XMVECTOR cameraLength = {};
	const float trackLimit = -30.0f;

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

	// �����m�F�p
	XMFLOAT3 posPoleSave = {};
	XMFLOAT3 posEnemySave = {};
	float minEnemyLength = 15.0f;
	float minPoleLength = 15.0f;
};


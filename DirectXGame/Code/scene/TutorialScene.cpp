#include "TutorialScene.h"
#include <imgui.h>
#include <cassert>
#include "SceneManager.h"

void TutorialScene::Initialize() {
	rope = new Rope;
	player = new Player;
	enemy = new Enemy;

	// 3D�I�u�W�F�N�g�ɃJ�������Z�b�g
	Object3d::SetCamera(camera);

	if (!Image2d::LoadTexture(HPTextNum, L"Resources/HPText.png"))
	{
		assert(0);
	}
	HPText = Image2d::Create(HPTextNum, { 0.0f,0.0f });
	HPText->SetSize({ 1280.0f,720.0f });

	if (!Image2d::LoadTexture(HPBarNum, L"Resources/HPBar.png"))
	{
		assert(0);
	}
	PlayerHPBar = Image2d::Create(HPBarNum, { 0.0f,45.0f });
	PlayerHPBar->SetSize({ 360.0f,60.0f });

	if (!Image2d::LoadTexture(HPGaugeNum, L"Resources/PlayerHPGauge.png"))
	{
		assert(0);
	}
	PlayerHPGauge = Image2d::Create(HPGaugeNum, { 0.0f,45.0f });
	PlayerHPGauge->SetSize({ 30.0f,60.0f });

	if (!Image2d::LoadTexture(fadeNum, L"Resources/fade.png"))
	{
		assert(0);
	}
	fadeTex = Image2d::Create(fadeNum, { 0.0f,0.0f });
	fadeTex->SetSize({ 1280.0f,720.0f });
	fadeTex->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });

	// �p�[�e�B�N������
	effectBox = Particle::Create(L"Resources/effectBox.png");
	effectCircle = Particle::Create(L"Resources/effectCircle.png");
	effectTarget = Particle::Create(L"Resources/effectTarget.png");

	enemy->ModelInit();
	rope->Initialize();

	// ���C�g�̐���
	light = Light::Create();
	// ���C�g�̐F��ݒ�
	light->SetLightColor({ 1.0f, 1.0f, 1.0f });
	// 3D�I�u�W�F�N�g�Ƀ��C�g���Z�b�g
	Object3d::SetLight(light);

	//audio->PlayWave("Resources/BGM/bgm.wav", 255, 0.08f);
	jsonObjectInit("tutorial");

	// �}�E�X�J�[�\�����\��
	ShowCursor(false);
}

void TutorialScene::Finalize()
{
	//audio->Stop();
	
	// �}�E�X�J�[�\����\��
	ShowCursor(true);
	safe_delete(player);
	safe_delete(enemy);
	safe_delete(rope);
	safe_delete(light);
	safe_delete(levelData);
	safe_delete(HPText);
	safe_delete(PlayerHPBar);
	safe_delete(PlayerHPGauge);
	safe_delete(fadeTex);
	safe_delete(effectBox);
	safe_delete(effectCircle);
	safe_delete(effectTarget);
}

void TutorialScene::Update()
{
	// �}�E�X���\���A�ړ��͈͂̐���
	mouse->CursorLimit();

	// �^�C�g������ڍs��̍X�V
	//	�v���C���[�̍��W�A���a�̐ݒ�
	player->Update(rFlag, moveFlag);
	pPos = player->GetObj()->GetPosition();
	pScale = player->GetObj()->GetScale();
	if (rushFlag)
	{
		player->Rush(catchPos, rushFlag, elapsedTime);
	}
	rope->Update(pPos);

	// json�t�@�C������ǂݍ��񂾃I�u�W�F�N�g�̍X�V
	jsonObjectUpdate();
	CollisionUpdate();
	LightUpdate();
	CameraUpdate();
	effectBox->Update();
	effectCircle->Update();
	effectTarget->Update();

	// �t���O�̎擾
	rFlag = rope->GetrFlag();
	moveFlag = rope->GetmoveFlag();
	avoidFlag = player->GetAvoidFlag();
	onGround = player->GetOnGround();

	if (throwCount < 60)
	{
		throwCount++;
	}

	// �|�[���ƓG�̋������r���ĒZ��������
	float minLength = (std::min)(minEnemyLength, minPoleLength);

	// �����ɉ����đ��������W��ύX(�G��D�悷��)

	if (minLength < 15.0f)
	{
		if (minLength == minEnemyLength)
		{
			posSave = posEnemySave;
		}
		else
		{
			posSave = posPoleSave;
		}

		effectTarget->TargetEffect(
			posSave, 3.0f, 1.0f,
			{ 1.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f, 1.0f },
			throwCount
		);
	}

	rope->Throw(pPos, posSave, minLength);

	posPoleSave = {};
	posEnemySave = {};
	minPoleLength = 15.0f;
	minEnemyLength = 15.0f;

	player->GetObj()->SetPosition(pPos);

	if (enemyHp <= 0)
	{
		fadeFlag = true;
		gameClearFlag = true;
	}
	else if (playerHp <= 0)
	{
		fadeFlag = true;
		reStartFlag = true;
	}

	if (fadeFlag == false && alpha > 0.0f)
	{
		alpha -= 0.02f;
	}
	else
	{
		alpha += 0.02f;
		if (alpha >= 1.0f)
		{
			reset();
		}
	}
	fadeTex->SetColor({ 1.0f, 1.0f, 1.0f, alpha });
}

void TutorialScene::Draw()
{
	//SetImgui();

#pragma region �w�i�摜�`��
	// �w�i�摜�`��O����
	Image2d::PreDraw(DirectXCommon::GetInstance()->GetCommandList());


	// �摜�`��㏈��
	Image2d::PostDraw();
	// �[�x�o�b�t�@�N���A
	DirectXCommon::GetInstance()->ClearDepthBuffer();
#pragma endregion �w�i�摜�`��

#pragma region 3D�I�u�W�F�N�g�`��
	// 3D�I�u�W�F�N�g�`��O����
	Object3d::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	// 3D�I�u�N�W�F�N�g�̕`��
	player->GetObj()->Draw();

	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		enemy->Draw();
	}
	rope->Draw();

	for (auto& object : jsonObject)
	{
		object->Draw();
	}

	// �p�[�e�B�N���̕`��
	effectBox->Draw(DirectXCommon::GetInstance()->GetCommandList());
	effectCircle->Draw(DirectXCommon::GetInstance()->GetCommandList());
	effectTarget->Draw(DirectXCommon::GetInstance()->GetCommandList());

	// 3D�I�u�W�F�N�g�`��㏈��
	Object3d::PostDraw();
#pragma endregion 3D�I�u�W�F�N�g�`��

#pragma region �O�i�摜�`��
	// �O�i�摜�`��O����
	Image2d::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	// �O�i�摜�̕`��
	HPText->Draw();
	PlayerHPBar->Draw();

	PlayerHPGauge->SetSize({ playerHp,60 });
	PlayerHPGauge->Draw();

	fadeTex->Draw();

	// �f�o�b�O�e�L�X�g�̕`��
	DebugText::GetInstance()->DrawAll(DirectXCommon::GetInstance()->GetCommandList());
	// �摜�`��㏈��
	Image2d::PostDraw();
#pragma endregion �O�i�摜�`��
}

void TutorialScene::reset()
{
	player->Reset();
	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		enemy->Reset();
	}
	enemys.erase(enemys.begin(), enemys.end());
	rope->Reset();
	jsonObject.erase(jsonObject.begin(), jsonObject.end());

	// �v���C���[
	pPos = { 0.0f, 10.0f, 0.0f };//���W
	pPosOld = { 0.0f, 10.0f, 0.0f };
	pScale = {};
	onGround = false;//���R�����̃t���O
	moveFlag = false;//�ړ��Ǘ��t���O
	avoidFlag = false;//����Ǘ��t���O
	playerHp = 360;

	// �G�l�~�[
	enemyHp = 360;
	ePos = {};
	ePosOld = {};
	eScale = {};
	enemyCount = 0;

	// ���[�v
	rFlag = false;

	// �J����
	cPos = {};
	cTarget = {};
	cameraLength = {};

	// �V�F�C�N�p
	shakeFlag = false;

	if (gameClearFlag)
	{
		SceneManager::GetInstance()->ChangeScene("Game");
	}
	else if (reStartFlag)
	{
		SceneManager::GetInstance()->ChangeScene("Tutorial");
	}
}

void TutorialScene::SetImgui()
{
	ImGui::Begin("DebugText");
	ImGui::SetWindowSize(ImVec2(300, 200));
	ImGui::Separator();
	ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);

	ImGui::Separator();
	ImGui::Text("playerPosX : %6.2f", pPos.x);
	ImGui::Text("playerPosY : %6.2f", pPos.y);
	ImGui::Text("playerPosZ : %6.2f", pPos.z);

	ImGui::End();
}

void TutorialScene::LightUpdate()
{
	//�������������l
	static XMVECTOR lightDir = { 5, -5, 5, 0 };

	light->SetLightDir(lightDir);
	light->Update();
}

void TutorialScene::CameraUpdate()
{
	if (pPos.y >= trackLimit)
	{
		camera->SetTarget(pPos);
	}
	else
	{
		playerHp = 0;
	}

	cameraLength = { cPos.x - pPos.x, cPos.y - pPos.y, cPos.z - pPos.z, 1.0f };
	cameraLength = XMVector3Normalize(cameraLength);

	//�J�����X�V
	if (shakeFlag == true)
	{
		camera->CameraShake(shakeFlag);
	}

	if (moveFlag)
	{
		cPos = camera->GetEye();
		cTarget = camera->GetTarget();
		camera->Update();
	}
}

void TutorialScene::CollisionUpdate()
{
	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		pPos = player->GetObj()->GetPosition();
		eAlive = enemy->GetAlive();

		if (eAlive)
		{
			if (rope->Collision(enemy->GetObj(), pPos))
			{
				rushFlag = true;
				catchPos = enemy->GetObj()->GetPosition();
			}

			if (rFlag && enemy->EnemyCollision(player->GetObj()))
			{
				shakeFlag = true;
				moveFlag = true;
				rope->SetmoveFlag(moveFlag);
				// �p�[�e�B�N������
				effectBox->CreateParticles(
					enemy->GetObj()->GetPosition(),
					2.0f, 5.0f,
					{ 1.0f, 0.0f, 0.0f, 1.0f },
					{ 0.0f, 1.0f, 0.0f, 1.0f },
					5
				);
				rope->SetrFlag(false);
				rushFlag = false;
				catchPos = {};
				elapsedTime = 0.0f;
				enemyHp -= (360.0f / enemyCount);
				controller->Vibration();
				enemys.remove(enemy);
				break;
			}
			else if (!rFlag && player->Damage(enemy->GetObj()))
			{
				shakeFlag = true;
				moveFlag = true;
				rope->SetmoveFlag(moveFlag);
				playerHp -= 36;

				// �p�[�e�B�N������
				effectCircle->CreateParticles(
					player->GetObj()->GetPosition(),
					1.0f, 2.0f,
					{ 0.0f, 1.0f, 0.0f, 1.0f },
					{ 1.0f, 0.0f, 0.0f, 1.0f },
					3
				);
			}

			if (!avoidFlag && enemy->BulletCollision())
			{
				if (!rFlag)
				{
					shakeFlag = true;
					playerHp -= 36;

					// �p�[�e�B�N������
					effectCircle->CreateParticles(
						player->GetObj()->GetPosition(),
						1.0f, 2.0f,
						{ 0.0f, 1.0f, 0.0f, 1.0f },
						{ 1.0f, 0.0f, 0.0f, 1.0f },
						3
					);
				}
			}

			for (auto& object : jsonObject)
			{
				if (object->GetType() == box_ || object->GetType() == wall_)
				{
					object->Update();
					XMFLOAT3 boxPos = object->GetPosition();
					XMFLOAT3 boxScale = object->GetCollisionScale();

					// ��Q�������m���Ă�����U�����Ă��Ȃ�
					if (enemy->ObstacleDetection(pPos, boxPos, boxScale))
					{
						minEnemyLength = 15.0f;
						posEnemySave = {};
						break;
					}
					else
					{
						pPos = player->GetObj()->GetPosition();
						ePos = enemy->GetObj()->GetPosition();

						float length = GetLength(pPos, ePos);

						if (length < minEnemyLength)
						{
							minEnemyLength = length;
							posEnemySave = ePos;
						}
					}
				}
			}
		}
		enemy->Update();
	}
}

void TutorialScene::jsonObjectInit(const std::string sceneName)
{
	// ���x���f�[�^�̓ǂݍ���
	levelData = LevelLoader::LoadFile(sceneName);

	// ���x���f�[�^����I�u�W�F�N�g�𐶐��A�z�u
	for (LevelData::ObjectData& objectData : levelData->objects)
	{
		if (objectData.fileName == "player")
		{
			// ���W
			XMFLOAT3 pos;
			XMStoreFloat3(&pos, objectData.trans);
			// �傫��
			XMFLOAT3 scale;
			XMStoreFloat3(&scale, objectData.scale);
			// �����蔻��
			XMFLOAT3 size;
			XMStoreFloat3(&size, objectData.size);

			// �v���C���[�𐶐�
			player->Initialize(pos, scale);
			pPos = player->GetObj()->GetPosition();
			player->GetObj()->SetCollisionScale(size);

			// ���X�|�[��������W��ݒ�
			reSpawnPos = pos;

			// �J�����̐ݒ�
			camera->Reset();
			camera->SetTarget(pPos);
			camera->Update();
			continue;
		}

		if (objectData.fileName == "enemy")
		{
			// ���W
			XMFLOAT3 pos;
			XMStoreFloat3(&pos, objectData.trans);
			// �傫��
			XMFLOAT3 scale;
			XMStoreFloat3(&scale, objectData.scale);
			// �����蔻��
			XMFLOAT3 size;
			XMStoreFloat3(&size, objectData.size);

			// �G�l�~�[�𐶐�
			std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
			newEnemy->Initialize(player);
			newEnemy->GetObj()->SetPosition(pos);
			newEnemy->GetObj()->SetScale(scale);
			newEnemy->GetObj()->SetCollisionScale(size);
			enemys.push_back(std::move(newEnemy));
			enemyCount++;
			continue;
		}

		// 3D�I�u�W�F�N�g�𐶐�
		std::unique_ptr<Object3d> newObject = Object3d::Create();

		// �t�@�C��������o�^�ς݃��f��������
		Model* model = nullptr;
		decltype(levelData->models)::iterator it = levelData->models.find(objectData.fileName);
		if (it != levelData->models.end())
		{
			model = it->second;
		}

		newObject->SetModel(model);

		// ���W
		XMFLOAT3 pos;
		XMStoreFloat3(&pos, objectData.trans);
		newObject->SetPosition(pos);

		// ��]�p
		XMFLOAT3 rot;
		XMStoreFloat3(&rot, objectData.rot);
		newObject->SetRotation(rot);

		// �傫��
		XMFLOAT3 scale;
		XMStoreFloat3(&scale, objectData.scale);
		newObject->SetScale(scale);

		// �����蔻��
		XMFLOAT3 colScale;
		XMStoreFloat3(&colScale, objectData.size);
		newObject->SetCollisionScale(colScale);

		if (objectData.fileName == "stage")
		{
			newObject->SetType(stage_);
		}
		else if (objectData.fileName == "box")
		{
			newObject->SetType(box_);
		}
		else if (objectData.fileName == "wall")
		{
			newObject->SetType(wall_);
		}
		else if (objectData.fileName == "pole")
		{
			newObject->SetColor({ 1.0f, 0.1f, 0.1f, 1.0f });
			newObject->SetType(pole_);
		}
		else if (objectData.fileName == "skydome")
		{
			newObject->SetType(skydome_);
		}

		// �z��ɓo�^
		jsonObject.push_back(std::move(newObject));
	}
}

void TutorialScene::jsonObjectUpdate()
{
	for (auto& object : jsonObject)
	{
		object->Update();

		if (object->GetType() == box_)
		{
			XMFLOAT3 boxPos = object->GetPosition();
			XMFLOAT3 boxScale = object->GetCollisionScale();
			player->MapCollide(boxPos, boxScale);
			pPos = player->GetObj()->GetPosition();

			if (rope->Collision(object, pPos))
			{
				rope->SetrFlag(false);
				rushFlag = false;
				catchPos = {};
				elapsedTime = 0.0f;
			}

			for (std::unique_ptr<Enemy>& enemy : enemys)
			{
				enemy->MapCollide(boxPos, boxScale);
			}
		}

		else if (object->GetType() == wall_)
		{
			XMFLOAT3 wallPos = object->GetPosition();
			XMFLOAT3 wallScale = object->GetCollisionScale();
			player->MapCollide(wallPos, wallScale);
			pPos = player->GetObj()->GetPosition();

			if (rope->Collision(object, pPos))
			{
				rope->SetrFlag(false);
				rushFlag = false;
				catchPos = {};
				elapsedTime = 0.0f;
			}

			for (std::unique_ptr<Enemy>& enemy : enemys)
			{
				enemy->MapCollide(wallPos, wallScale);
			}
		}

		else if (object->GetType() == stage_)
		{
			XMFLOAT3 stagePos = object->GetPosition();
			XMFLOAT3 stageScale = object->GetCollisionScale();
			player->StageCollide(stagePos, stageScale);
			pPos = player->GetObj()->GetPosition();

			for (std::unique_ptr<Enemy>& enemy : enemys)
			{
				enemy->StageCollide(stagePos, stageScale);
			}
		}

		else if (object->GetType() == pole_)
		{
			pPos = player->GetObj()->GetPosition();
			XMFLOAT3 polePos = object->GetPosition();
			XMFLOAT3 poleScale = object->GetCollisionScale();
			float length = GetLength(pPos, polePos);

			if (length < minPoleLength)
			{
				minPoleLength = length;
				posPoleSave = polePos;
			}

			if (rope->Collision(object, pPos))
			{
				rushFlag = true;
				catchPos = object->GetPosition();
				posPoleSave = {};
				minPoleLength = 10.0f;
			}

			if (rushFlag && player->PoleCollide(polePos, poleScale))
			{
				rope->SetrFlag(false);
				rushFlag = false;
				catchPos = {};
				elapsedTime = 0.0f;
				throwCount = 0;
			}
		}
	}
}

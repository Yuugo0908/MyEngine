#include "TutorialScene.h"
#include <imgui.h>
#include <cassert>
#include "SceneManager.h"

void TutorialScene::Initialize()
{
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
	effectCircle2 = Particle::Create(L"Resources/effectCircle2.png");
	effectTarget = Particle::Create(L"Resources/effectBox2.png");
	effectAvoid = Particle::Create(L"Resources/effectBox2.png");

	enemy->ModelInit();
	rope->Initialize();

	// ���C�g�̐���
	light = Light::Create();
	// ���C�g�̐F��ݒ�
	light->SetLightColor({ 1.0f, 1.0f, 1.0f });
	// 3D�I�u�W�F�N�g�Ƀ��C�g���Z�b�g
	Object3d::SetLight(light);

	//Bgm->PlayWave("Resources/BGM/bgm.wav", 255, 0.08f);
	jsonObjectInit("Tutorial");

	// �}�E�X�J�[�\�����\��
	ShowCursor(false);
}

void TutorialScene::Finalize()
{
	//Bgm->Stop();

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
	safe_delete(effectCircle2);
	safe_delete(effectTarget);
	safe_delete(effectAvoid);
}

void TutorialScene::Update()
{
	// �}�E�X�̈ړ��͈͂̐���
	mouse->CursorLimit();

	// �^�C�g������ڍs��̍X�V
	//	�v���C���[�̍��W�A���a�̐ݒ�
	if (rushFlag)
	{
		player->Rush(catchPos, rushFlag, elapsedTime);
	}
	player->Update(rFlag, moveFlag);
	pPos = player->GetObj()->GetPosition();
	pScale = player->GetObj()->GetScale();
	rope->Update(pPos);

	// json�t�@�C������ǂݍ��񂾃I�u�W�F�N�g�̍X�V
	jsonObjectUpdate();
	CollisionUpdate();
	LightUpdate();
	CameraUpdate();
	RopeUpdate();
	effectBox->Update();
	effectCircle->Update();
	effectCircle2->Update();
	effectTarget->Update();

	// �t���O�̎擾
	rFlag = rope->GetrFlag();
	moveFlag = rope->GetmoveFlag();
	avoidFlag = player->GetAvoidFlag();

	if (avoidFlag)
	{
		effectAvoid->CreateParticles(
			pPos, 1.0f, 2.0f,
			{ 0.0f, 0.0f, 0.0f, 1.0f },
	  		{ 1.0f, 0.0f, 0.0f, 1.0f },
			1, 10, false, false
		);
	}
	effectAvoid->Update();

	if (enemyHp <= 0)
	{
		fadeFlag = true;
		gameClearFlag = true;
	}
	else if (playerHp <= 0)
	{
		fadeFlag = true;
		gameOverFlag = true;
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
	SetImgui();

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
	rope->GetObj()->Draw();

	for (auto& object : jsonObject)
	{
		if (object->GetDrawFlag())
		{
			object->Draw();
		}
	}

	// �p�[�e�B�N���̕`��
	effectBox->Draw(DirectXCommon::GetInstance()->GetCommandList());
	effectCircle->Draw(DirectXCommon::GetInstance()->GetCommandList());
	effectCircle2->Draw(DirectXCommon::GetInstance()->GetCommandList());
	effectTarget->Draw(DirectXCommon::GetInstance()->GetCommandList());
	effectAvoid->Draw(DirectXCommon::GetInstance()->GetCommandList());

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

	if (gameClearFlag)
	{
		SceneManager::GetInstance()->ChangeScene("Game");
	}
	else if (gameOverFlag)
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
	ImGui::Separator();
	ImGui::Text("cameraPosX : %6.2f", cPos.x);
	ImGui::Text("cameraPosY : %6.2f", cPos.y);
	ImGui::Text("cameraPosZ : %6.2f", cPos.z);

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
	cPos = camera->GetEye();
	cTarget = camera->GetTarget();

	if (gameClearFlag)
	{
		camera->SetTarget(pPos);
		if (pPos.y <= trackLimit)
		{
			player->ReSpawn();
		}
	}
	else
	{
		if (pPos.y >= trackLimit)
		{
			camera->SetTarget(pPos);
		}
		else
		{
			playerHp = 0;
		}
	}

	//�J�����X�V
	if (shakeFlag == true)
	{
		camera->CameraShake(shakeFlag);
	}

	camera->Update();
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
				rope->SetmoveFlag(true);
				// �p�[�e�B�N������
				effectBox->CreateParticles(
					enemy->GetObj()->GetPosition(),
					2.0f, 5.0f,
					{ 1.0f, 0.0f, 0.0f, 1.0f },
					{ 0.0f, 1.0f, 0.0f, 1.0f },
					5, 60, true, true
				);
				effectCircle2->CreateParticles(
					enemy->GetObj()->GetPosition(),
					1.0f, 10.0f,
					{ 1.0f, 0.0f, 0.0f, 1.0f },
					{ 1.0f, 0.0f, 0.0f, 1.0f },
					5, 10, false, false
				);

				oldPosSave = posEnemySave;
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
				rope->SetmoveFlag(true);
				playerHp -= 36;

				// �p�[�e�B�N������
				effectCircle->CreateParticles(
					player->GetObj()->GetPosition(),
					1.0f, 2.0f,
					{ 0.0f, 1.0f, 0.0f, 1.0f },
					{ 1.0f, 0.0f, 0.0f, 1.0f },
					3, 60, true, true
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
						3, 60, true, true
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
						break;
					}
					else
					{
						pPos = player->GetObj()->GetPosition();
						ePos = enemy->GetObj()->GetPosition();

						float length = GetLength(pPos, ePos);

						if (length < minEnemyLength)
						{
							posEnemySave = ePos;
							minEnemyLength = length;
						}
					}
				}
			}
		}
		enemy->Update();
	}
}

void TutorialScene::RopeUpdate()
{
	if (throwCount < 60)
	{
		throwCount++;
	}

	// �|�[���ƓG�̋������r���ĒZ��������
	float minLength = (std::min)(minEnemyLength, minPoleLength);
	bool getThrowFlag = rope->GetThrowFlag();

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

		for (auto& object : jsonObject)
		{
			if (object->GetType() == box_ || object->GetType() == wall_ || object->GetType() == stage_)
			{
				XMFLOAT3 pos = object->GetPosition();
				XMFLOAT3 scale = object->GetCollisionScale();
				if (GetLength(pPos, pos) >= 15.0f)
				{
					continue;
				}
				else if (Collision::CollisionRayBox(pPos, posSave, pos, scale))
				{
					minLength = 15.0f;
					rope->SetThrowFlag(false);
					break;
				}
			}
		}

		// ���ӂɂ���|�[���ɂ͔������Ȃ�
		if (GetLength(posSave, oldPosSave) <= 0.0f && minLength == minPoleLength)
		{
			minLength = 15.0f; 
			rope->SetThrowFlag(false);
		}
		else if (minLength < 15.0f)
		{
			effectTarget->TargetEffect(
				posSave, 3.0f, 1.0f,
				{ 1.0f, 0.0f, 0.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				throwCount
			);
		}
	}
	else
	{
		rope->SetThrowFlag(false);
	}

	if (GetLength(pPos, oldPosSave) > 15.0f)
	{
		oldPosSave = {};
	}

	if (minLength < 15.0f)
	{
		rope->Throw(pPos, posSave, minLength);
		// ���[�v���΂��������Ƀv���C���[������
		player->TrackRot(pPos, posSave);
	}
	else
	{
		float cameraRot = camera->CameraRot(pPos);
		// �J�����������Ă�������Ƀv���C���[������
		player->GetObj()->SetRotation({0, XMConvertToDegrees(cameraRot), 0});
	}

	posPoleSave = {};
	posEnemySave = {};
	minPoleLength = 15.0f;
	minEnemyLength = 15.0f;
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
		XMFLOAT3 pos = object->GetPosition();
		XMFLOAT3 scale = object->GetCollisionScale();
		if (Collision::CollisionRayBox(cPos, pPos, pos, scale))
		{
			object->SetDrawFlag(false);
		}
		else
		{
			object->SetDrawFlag(true);
		}

		object->Update();

		if (object->GetType() == box_)
		{
			XMFLOAT3 boxPos = object->GetPosition();
			XMFLOAT3 boxScale = object->GetCollisionScale();
			player->MapCollide(boxPos, boxScale);
			pPos = player->GetObj()->GetPosition();

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

			for (std::unique_ptr<Enemy>& enemy : enemys)
			{
				enemy->MapCollide(wallPos, wallScale);
			}
		}

		else if (object->GetType() == stage_)
		{
			bool reverseFlag = false;
			XMFLOAT3 stagePos = object->GetPosition();
			XMFLOAT3 stageScale = object->GetCollisionScale();
			player->StageCollide(stagePos, stageScale, reverseFlag);
			pPos = player->GetObj()->GetPosition();

			if (reverseFlag)
			{
				rope->SetrFlag(false);
				rushFlag = false;
				catchPos = {};
				elapsedTime = 0.0f;
			}

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
			}

			if (rushFlag && player->PoleCollide(polePos, poleScale))
			{
				rope->SetrFlag(false);
				rushFlag = false;
				catchPos = {};
				elapsedTime = 0.0f;
				oldPosSave = posPoleSave;
			}
		}
	}
}
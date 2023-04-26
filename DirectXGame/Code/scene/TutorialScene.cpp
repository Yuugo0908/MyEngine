#include "TutorialScene.h"
#include <imgui.h>
#include <cassert>
#include "SceneManager.h"

void TutorialScene::Initialize()
{
	rope = Rope::GetInstance();
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

	if (!Image2d::LoadTexture(wasdNum, L"Resources/wasdKey.png"))
	{
		assert(0);
	}
	wasdKey = Image2d::Create(wasdNum, { 0.0f, 0.0f });
	wasdKey->SetPosition({50.0f, 400.0f});
	wasdKey->SetSize({ 256.0f,256.0f });
	if (!Image2d::LoadTexture(spaceNum, L"Resources/spaceKey.png"))
	{
		assert(0);
	}
	spaceKey = Image2d::Create(spaceNum, { 0.0f, 0.0f });
	spaceKey->SetPosition({ 512.0f, 500.0f });
	spaceKey->SetSize({ 256.0f,256.0f });

	if (!Image2d::LoadTexture(mouseNum, L"Resources/mouse.png"))
	{
		assert(0);
	}
	mouseImg = Image2d::Create(mouseNum, { 0.0f, 0.0f });
	mouseImg->SetPosition({ 1000.0f, 400.0f });
	mouseImg->SetSize({ 256.0f,256.0f });

	if (!Image2d::LoadTexture(mouseLeftNum, L"Resources/mouse_left.png"))
	{
		assert(0);
	}
	mouseLeftImg = Image2d::Create(mouseLeftNum, { 0.0f, 0.0f });
	mouseLeftImg->SetPosition({ 1000.0f, 400.0f });
	mouseLeftImg->SetSize({ 256.0f,256.0f });

	if (!Image2d::LoadTexture(mouseRightNum, L"Resources/mouse_right.png"))
	{
		assert(0);
	}
	mouseRightImg = Image2d::Create(mouseRightNum, { 0.0f, 0.0f });
	mouseRightImg->SetPosition({ 1000.0f, 400.0f });
	mouseRightImg->SetSize({ 256.0f,256.0f });

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

	player->Reset();
	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		enemy->Reset();
	}
	enemys.erase(enemys.begin(), enemys.end());
	rope->Reset();
	jsonObject.erase(jsonObject.begin(), jsonObject.end());

	safe_delete(player);
	safe_delete(enemy);
	safe_delete(light);
	safe_delete(levelData);
	safe_delete(HPText);
	safe_delete(PlayerHPBar);
	safe_delete(PlayerHPGauge);
	safe_delete(fadeTex);
	safe_delete(wasdKey);
	safe_delete(spaceKey);
	safe_delete(mouseImg);
	safe_delete(mouseLeftImg);
	safe_delete(mouseRightImg);
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

	if (!fadeFlag && alpha > 0.0f)
	{
		alpha -= 0.02f;
	}

	if(gameClearFlag)
	{
		alpha += 0.02f;
		if (alpha >= 1.0f)
		{
			reset();
		}
	}
	else if (gameOverFlag)
	{
		alpha += 0.02f;
		if (alpha >= 1.0f)
		{
			gameOverFlag = false;
			fadeFlag = false;
			playerHp = 360;
			player->ReSpawn();
		}
	}
	fadeTex->SetColor({ 1.0f, 1.0f, 1.0f, alpha });

	// �G�����ׂē|���΃N���A
	if (enemyCount <= 0)
	{
		fadeFlag = true;
		gameClearFlag = true;
	}
	// �v���C���[��HP��0�ɂȂ�����Q�[���I�[�o�[
	else if (playerHp <= 0)
	{
		if (!gameOverFlag)
		{
			// �p�[�e�B�N������
			effectBox->CreateParticles(
				pPos,
				2.0f, 5.0f,
				{ 0.0f, 0.0f, 1.0f, 1.0f },
				{ 1.0f, 0.0f, 0.0f, 1.0f },
				5, 60, true, true
			);
			effectCircle2->CreateParticles(
				pPos,
				1.0f, 10.0f,
				{ 0.0f, 0.0f, 1.0f, 1.0f },
				{ 0.0f, 0.0f, 1.0f, 1.0f },
				5, 10, false, false
			);
			alpha = -1.0f;
		}

		// �G�t�F�N�g�̍X�V
		effectBox->Update();
		effectCircle2->Update();
		fadeFlag = true;
		gameOverFlag = true;
		return;
	}

	if (targetFlag && firstThrowFlag && !avoidFlag)
	{
		if (controller->GetPadState(Controller::State::X, Controller::Type::TRIGGER) || mouse->TriggerMouseLeft())
		{
			fadeFlag = false;
			alpha = 0.0f;
			firstThrowFlag = false;
			rope->SetThrowFlag(true);
		}
		else
		{
			fadeFlag = true;
			alpha = 0.7f;
			return;
		}
	}
	else if (enemyAttackFlag && firstAvoidFlag && !avoidFlag)
	{
		if (mouse->TriggerMouseRight() || controller->GetPadState(Controller::State::RT, Controller::Type::NONE))
		{
			fadeFlag = false;
			alpha = 0.0f;
			firstAvoidFlag = false;
			firstAttackFlag = true;
			player->SetAvoidFlag(true);
			player->Avoid();
		}
		else
		{
			fadeFlag = true;
			alpha = 0.7f;
			return;
		}
	}
	else if (playerAttackFlag && firstAttackFlag && !avoidFlag)
	{
		if (controller->GetPadState(Controller::State::X, Controller::Type::TRIGGER) || mouse->TriggerMouseLeft())
		{
			fadeFlag = false;
			alpha = 0.0f;
			firstAttackFlag = false;
			rope->SetThrowFlag(true);
		}
		else
		{
			fadeFlag = true;
			alpha = 0.7f;
			return;
		}
	}

	// �v���C���[�̓ːi
	player->Rush(catchPos, rushFlag, elapsedTime);

	// �^�C�g������ڍs��̍X�V
	// �v���C���[�̍��W�A���a�̐ݒ�
	player->Update();
	pPos = player->GetObj()->GetPosition();
	pScale = player->GetObj()->GetScale();
	rope->Update(pPos);

	// json�t�@�C������ǂݍ��񂾃I�u�W�F�N�g�̍X�V
	jsonObjectUpdate();
	// �����蔻��̍X�V
	CollisionUpdate();
	// ���C�g�̍X�V
	LightUpdate();
	// �J�����̍X�V
	CameraUpdate();
	// ���[�v�̍X�V
	RopeUpdate();

	// �G�t�F�N�g�̍X�V
	effectBox->Update();
	effectCircle->Update();
	effectCircle2->Update();
	effectTarget->Update();

	// �t���O�̎擾
	rFlag = rope->GetrFlag();
	avoidFlag = player->GetAvoidFlag();

	// ��������ۂɃG�t�F�N�g������
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
	if (playerHp > 0.0f)
	{
		player->GetObj()->Draw();
	}

	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		enemy->Draw();
	}
	rope->GetObj()->Draw();

	for (auto& object : jsonObject)
	{
		if (object->GetType() == stage_)
		{
			object->Draw();
		}
		else if (object->GetDrawFlag())
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

	wasdKey->Draw();
	spaceKey->Draw();
	mouseImg->Draw();

	fadeTex->Draw();

	if ((playerAttackFlag || targetFlag) && (firstThrowFlag || firstAttackFlag))
	{
		mouseLeftImg->Draw();
	}
	if (enemyAttackFlag && firstAvoidFlag)
	{
		mouseRightImg->Draw();
	}

	// �f�o�b�O�e�L�X�g�̕`��
	DebugText::GetInstance()->DrawAll(DirectXCommon::GetInstance()->GetCommandList());
	// �摜�`��㏈��
	Image2d::PostDraw();
#pragma endregion �O�i�摜�`��
}

void TutorialScene::reset()
{
	if (gameClearFlag)
	{
		SceneManager::GetInstance()->ChangeScene("Game");
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

		//// �͈͂�0.0f�`1.0f�ɂ���
		//easeTime = (std::min)((std::max)(easeTime, 0.0f), 1.0f);

		//// �^�[�Q�b�g�ƃv���C���[�̊Ԃ��^�[�Q�b�g����
		//if (targetFlag || playerAttackFlag)
		//{
		//	XMFLOAT3 cameratarget =  pPos * (1.0f - easeTime) + ((posSave + pPos) / 2) * easeTime;
		//	camera->SetTarget(cameratarget);
		//	easeTime += 0.05f;
		//}
		//else
		//{
		//	XMFLOAT3 cameratarget = pPos * (1.0f - easeTime) + ((posSave + pPos) / 2) * easeTime;
		//	camera->SetTarget(cameratarget);
		//	easeTime -= 0.05f;
		//}
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
		enemyAlive = enemy->GetAlive();

		if (enemyAlive)
		{
			if (!rushFlag && rope->GetrFlag())
			{
				rushFlag = true;
				catchPos = posSave;
			}

			if (rFlag && enemy->EnemyCollision(player->GetObj()))
			{
				// �J�����V�F�C�N�̔���
				shakeFlag = true;

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

				rope->SetrFlag(false);
				rushFlag = false;
				catchPos = {};
				elapsedTime = 0.0f;
				enemyCount--;
				controller->Vibration();
				enemys.remove(enemy);
				break;
			}
			else if (!rFlag && player->Damage(enemy->GetObj()))
			{
				shakeFlag = true;
				playerHp -= 18;

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
					playerHp -= 18;

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

			bool getThrowFlag = rope->GetThrowFlag();
			if (!getThrowFlag && !rFlag &&  enemy->Danger())
			{
				enemyAttackFlag = true;
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
						// �v���C���[�ƓG�̍��W�Ƌ������擾
						pPos = player->GetObj()->GetPosition();
						ePos = enemy->GetObj()->GetPosition();
						float length = GetLength(pPos, ePos);

						// ���[�v���͂������ɂ����ꍇ�A���̓G�̍��W�Ƌ�����ۑ�
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
	if (targetEffectCount < 60)
	{
		targetEffectCount++;
	}

	// �|�[���ƓG�̋������r���ĒZ��������
	float minLength = (std::min)(minEnemyLength, minPoleLength);

	// �ǂ��������̋����������ꍇ�A�G��D�悷��
	if (minEnemyLength < baseLength && minPoleLength < baseLength)
	{
		minLength = minEnemyLength;
	}

	// �����ɉ����đ��������W��ύX(�G��D�悷��)
	if (minLength < baseLength)
	{
		if (minLength == minEnemyLength)
		{
			posSave = posEnemySave;
			playerAttackFlag = true;
		}
		else
		{
			posSave = posPoleSave;
			targetFlag = true;
		}

		// ��Q�������m������^�[�Q�b�g���Ȃ�
		for (auto& object : jsonObject)
		{
			if (object->GetType() == box_ || object->GetType() == wall_ || object->GetType() == stage_)
			{
				XMFLOAT3 pos = object->GetPosition();
				XMFLOAT3 scale = object->GetCollisionScale();
				if (GetLength(pPos, pos) >= baseLength)
				{
					continue;
				}
				else if (Collision::CollisionRayBox(pPos, posSave, pos, scale))
				{
					minLength = baseLength;
					rope->SetThrowFlag(false);
					targetFlag = false;
					playerAttackFlag = false;
					break;
				}
			}
		}

		// �ߋ��Ƀ^�[�Q�b�g�����|�[���ɂ͔������Ȃ�
		if (GetLength(posSave, oldPosSave) <= 0.0f && minLength == minPoleLength)
		{
			minLength = minEnemyLength;
			posSave = posEnemySave;
			targetFlag = false;
		}

		// �^�[�Q�b�g���������ɂ���΃^�[�Q�b�g�G�t�F�N�g�𔭐�������
		if (minLength < baseLength)
		{
			effectTarget->TargetEffect(
				posSave, 3.0f, 1.0f,
				{ 1.0f, 0.0f, 0.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				targetEffectCount
			);
		}
	}
	else
	{
		rope->SetThrowFlag(false);
		targetFlag = false;
		playerAttackFlag = false;
	}

	if (GetLength(pPos, oldPosSave) > 30.0f)
	{
		oldPosSave = {};
	}

	if (minLength < baseLength)
	{
		rope->Throw(pPos, posSave, minLength);
		// �^�[�Q�b�g���Ă�������Ƀv���C���[������
		player->TrackRot(pPos, posSave);
		camera->SetTarget(posSave);
	}
	else
	{
		float cameraRot = camera->CameraRot(pPos);
		// �J�����������Ă�������Ƀv���C���[������
		player->GetObj()->SetRotation({ 0, XMConvertToDegrees(cameraRot), 0 });
		rope->SetThrowFlag(false);
		targetFlag = false;
		playerAttackFlag = false;
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

		// �v���C���[�ƃJ�����̊ԂɃI�u�W�F�N�g�����������A�I�u�W�F�N�g��`�悵�Ȃ�
		if (Collision::CollisionRayBox(cPos, pPos, pos, scale))
		{
			object->SetDrawFlag(false);
		}
		else
		{
			object->SetDrawFlag(true);
		}

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

			if (!rushFlag && rope->GetrFlag())
			{
				rushFlag = true;
				catchPos = posSave;
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

		object->Update();
	}
}
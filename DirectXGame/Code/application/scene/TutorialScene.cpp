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

	// �v���C���[HP�\���p�摜�̐���
	HPText = Image2d::Create(Image2d::ImgNumber::HPTextNum, { 0.0f,0.0f });
	HPText->SetSize({ 1280.0f,720.0f });
	PlayerHPBar = Image2d::Create(Image2d::ImgNumber::HPBarNum, { 0.0f,45.0f });
	PlayerHPBar->SetSize({ 360.0f,60.0f });
	PlayerHPGauge = Image2d::Create(Image2d::ImgNumber::HPGaugeNum, { 0.0f,45.0f });
	PlayerHPGauge->SetSize({ 30.0f,60.0f });

	// �`���[�g���A���p�摜�̐���
	wasdKey = Image2d::Create(Image2d::ImgNumber::wasdNum, { 0.0f, 0.0f });
	wasdKey->SetPosition({50.0f, 400.0f});
	wasdKey->SetSize({ 256.0f,256.0f });
	spaceKey = Image2d::Create(Image2d::ImgNumber::spaceNum, { 0.0f, 0.0f });
	spaceKey->SetPosition({ 512.0f, 500.0f });
	spaceKey->SetSize({ 256.0f,256.0f });
	mouseImg = Image2d::Create(Image2d::ImgNumber::mouseNum, { 0.0f, 0.0f });
	mouseImg->SetPosition({ 1000.0f, 400.0f });
	mouseImg->SetSize({ 256.0f,256.0f });
	mouseLeftImg = Image2d::Create(Image2d::ImgNumber::mouseLeftNum, { 0.0f, 0.0f });
	mouseLeftImg->SetPosition({ 1000.0f, 400.0f });
	mouseLeftImg->SetSize({ 256.0f,256.0f });
	mouseRightImg = Image2d::Create(Image2d::ImgNumber::mouseRightNum, { 0.0f, 0.0f });
	mouseRightImg->SetPosition({ 1000.0f, 400.0f });
	mouseRightImg->SetSize({ 256.0f,256.0f });

	// �p�[�e�B�N������
	effectBox = Particle::Create("effectBox");
	effectCircle = Particle::Create("effectCircle");
	effectCircle2 = Particle::Create("effectCircle2");
	effectTarget = Particle::Create("effectBox2");
	effectAvoid = Particle::Create("effectBox2");

	enemy->ModelInit();
	rope->Initialize();

	// ���C�g�̐���
	light = Light::Create();
	// ���C�g�̐F��ݒ�
	light->SetLightColor({ 1.0f, 1.0f, 1.0f });
	// 3D�I�u�W�F�N�g�Ƀ��C�g���Z�b�g
	Object3d::SetLight(light);

	//Bgm->PlayWave("Resources/BGM/bgm.wav", 255, 0.08f);
	jsonObjectInit("test");

	// �}�E�X�J�[�\�����\��
	ShowCursor(false);
}

void TutorialScene::Finalize()
{
	//Bgm->Stop();

	// �}�E�X�J�[�\����\��
	ShowCursor(true);

	player->Reset();
	for (std::unique_ptr<Enemy>& enemy : enemies)
	{
		enemy->Reset();
	}
	enemies.erase(enemies.begin(), enemies.end());
	rope->Reset();
	jsonObject.erase(jsonObject.begin(), jsonObject.end());
	jsonObject.shrink_to_fit();

	safe_delete(player);
	safe_delete(enemy);
	safe_delete(levelData);
	safe_delete(HPText);
	safe_delete(PlayerHPBar);
	safe_delete(PlayerHPGauge);
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

	// �t���O�̎擾
	rFlag = rope->GetrFlag();
	avoidFlag = player->GetAvoidFlag();

#pragma region �t�F�[�h
	if (!gameClearFlag && !gameOverFlag)
	{
		FadeScene::GetInstance()->FadeOut(1.0f);
	}

	if (gameClearFlag)
	{
		FadeScene::GetInstance()->FadeIn(0.0f);
		if (FadeScene::fadeInEnd)
		{
			Reset();
		}
	}
	else if (gameOverFlag)
	{
		FadeScene::GetInstance()->FadeIn(-1.0f);
		if (FadeScene::fadeInEnd)
		{
			gameOverFlag = false;
			playerHp = 360;
			player->ReSpawn();
		}
	}
#pragma endregion

#pragma region �N���Aor�Q�[���I�[�o�[
	// �G�����ׂē|���΃N���A
	if (enemyCount <= 0)
	{
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
				5, 20, false, false
			);
		}

		// �Q�[���I�[�o�[�t���O
		gameOverFlag = true;
		return;
	}
#pragma endregion

#pragma region �`���[�g���A��
	switch (tutorialState)
	{
	case ropeThrow:
		// ���߂ă|�[���Ƀ��[�v���΂��ꍇ
		if (throwFlag && !avoidFlag)
		{
			if (controller->GetPadState(Controller::State::X, Controller::Type::TRIGGER) || mouse->TriggerMouseLeft())
			{
				// ���[�v�𔭎˂����A���̃`���[�g���A���ɐi��
				rope->SetThrowFlag(true);

				// �X�e�[�W�I�u�W�F�N�g�̐F�����ɖ߂�
				for (auto& object : jsonObject)
				{
					object->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
				}
				tutorialState = Avoid;
			}
			else
			{
				// �^�[�Q�b�g�G�t�F�N�g�̔���
				effectTarget->TargetEffect(
					targetPos, 3.0f, 1.0f,
					{ 1.0f, 0.0f, 0.0f, 1.0f },
					{ 1.0f, 1.0f, 1.0f, 1.0f },
					targetEffectCount
				);

				// �|�[���ȊO�̃X�e�[�W�I�u�W�F�N�g�̐F���Â�����
				for (auto& object : jsonObject)
				{
					if (object->GetType() != "pole")
					{
						object->SetColor({ 0.3f, 0.3f, 0.3f, 1.0f });
					}
					object->Update();
				}
				return;
			}
		}

		break;
	case Avoid:
		// ���߂ĉ�����g���ꍇ
		if (enemyAttackFlag && !avoidFlag)
		{
			if (controller->GetPadState(Controller::State::RT, Controller::Type::TRIGGER) || mouse->TriggerMouseRight())
			{
				// ����𔭐������A���̃`���[�g���A���ɐi��
				player->SetAvoidFlag(true);
				player->Avoid();

				// �X�e�[�W�I�u�W�F�N�g�̐F�����ɖ߂�
				for (auto& object : jsonObject)
				{
					object->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
				}
				tutorialState = Attack;
			}
			else
			{
				// �X�e�[�W�I�u�W�F�N�g�̐F���Â�����
				for (auto& object : jsonObject)
				{
					object->SetColor({ 0.3f, 0.3f, 0.3f, 1.0f });
					object->Update();
				}
				return;
			}
		}
		break;
	case Attack:
		// ���߂ēG���U������ꍇ
		if (playerAttackFlag && !avoidFlag)
		{
			if (controller->GetPadState(Controller::State::X, Controller::Type::TRIGGER) || mouse->TriggerMouseLeft())
			{
				// ���[�v�𔭎˂�����(�`���[�g���A���͏I��)
				rope->SetThrowFlag(true);

				// �X�e�[�W�I�u�W�F�N�g�̐F�����ɖ߂�
				for (auto& object : jsonObject)
				{
					object->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
				}
				tutorialState = None;
			}
			else
			{
				// �^�[�Q�b�g�G�t�F�N�g�̔���
				effectTarget->TargetEffect(
					targetPos, 3.0f, 1.0f,
					{ 1.0f, 0.0f, 0.0f, 1.0f },
					{ 1.0f, 1.0f, 1.0f, 1.0f },
					targetEffectCount
				);

				// �X�e�[�W�I�u�W�F�N�g�̐F���Â�����
				for (auto& object : jsonObject)
				{
					object->SetColor({ 0.3f, 0.3f, 0.3f, 1.0f });
					object->Update();
				}
				return;
			}
		}
		break;
	default:
		break;
	}
# pragma endregion

	if (FadeScene::fadeOutEnd)
	{
		// �v���C���[�̓ːi
		player->Rush(catchPos, rushFlag, elapsedTime);
		// �^�C�g������ڍs��̍X�V
		player->Update();
	}

	// �v���C���[�̍��W�A���a�̐ݒ�
	pPos = player->GetObj()->GetPosition();
	pScale = player->GetObj()->GetScale();

	if (FadeScene::fadeOutEnd)
	{
		// �G�̍X�V
		EnemyUpdate();
	}
	// json�t�@�C������ǂݍ��񂾃I�u�W�F�N�g�̍X�V
	jsonObjectUpdate();
	if (FadeScene::fadeOutEnd)
	{
		// ���[�v�̍X�V
		RopeUpdate();
	}
	// ���C�g�̍X�V
	LightUpdate();
	// �J�����̍X�V
	CameraUpdate();

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

	for (std::unique_ptr<Enemy>& enemy : enemies)
	{
		enemy->Draw();
	}
	rope->GetObj()->Draw();

	for (auto& object : jsonObject)
	{
		if (object->GetType() == "stage")
		{
			object->Draw();
		}
		else if (object->GetDrawFlag())
		{
			object->Draw();
		}
	}

	// 3D�I�u�W�F�N�g�`��㏈��
	Object3d::PostDraw();
#pragma endregion 3D�I�u�W�F�N�g�`��

#pragma region �p�[�e�B�N���`��

	// �p�[�e�B�N���`��O����
	Particle::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	// �p�[�e�B�N���̕`��
	effectBox->Draw();
	effectCircle->Draw();
	effectCircle2->Draw();
	effectTarget->Draw();
	effectAvoid->Draw();

	for (std::unique_ptr<Enemy>& enemy : enemies)
	{
		enemy->reactionDraw();
	}

	// �p�[�e�B�N���`��㏈��
	Particle::PostDraw();

#pragma endregion �p�[�e�B�N���`��

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


	if ((playerAttackFlag || throwFlag) && (tutorialState == ropeThrow || tutorialState == Attack))
	{
		imgShowCount++;
		if (imgShowCount >= 30)
		{
			mouseLeftImg->Draw();
			if (imgShowCount >= 60)
			{
				imgShowCount = 0;
			}
		}
	}
	else if (enemyAttackFlag && tutorialState == Avoid)
	{
		imgShowCount++;
		if (imgShowCount >= 30)
		{
			mouseRightImg->Draw();
			if (imgShowCount >= 60)
			{
				imgShowCount = 0;
			}
		}
	}

	// �t�F�[�h�̕`��
	FadeScene::GetInstance()->Draw();

	// �f�o�b�O�e�L�X�g�̕`��
	DebugText::GetInstance()->DrawAll(DirectXCommon::GetInstance()->GetCommandList());
	// �摜�`��㏈��
	Image2d::PostDraw();
#pragma endregion �O�i�摜�`��
}

void TutorialScene::Reset()
{
	// �Q�[���A�X�e�[�W1����X�^�[�g
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
	}

	//�J�����X�V
	if (shakeFlag == true)
	{
		camera->CameraShake(shakeFlag);
	}

	camera->Update();
}

void TutorialScene::EnemyUpdate()
{
	for (std::unique_ptr<Enemy>& enemy : enemies)
	{
		pPos = player->GetObj()->GetPosition();
		rFlag = rope->GetrFlag();
		getEnemyAlive = enemy->GetAlive();

		// �G������ł����玟�̓G�̏����ɐi��
		if (!getEnemyAlive)
		{
			enemy->Update();
			continue;
		}

		// �ːi������ԂœG�ɐG�ꂽ��
		if (rushFlag && enemy->EnemyCollision(player->GetObj()))
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
			enemies.remove(enemy);
			break;
		}

		// �ːi�����ɓG�ɐG�ꂽ��
		if (!rushFlag && player->Damage(enemy->GetObj()))
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

		// �v���C���[�ɓG�̒e�����������ۂ̃_���[�W����
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

		// ���[�v�𔭎˂܂��͐ڒ����Ă��Ȃ���Ԃł����
		bool getThrowFlag = rope->GetThrowFlag();
		if (enemy->Danger() && !getThrowFlag && !rFlag)
		{
			enemyAttackFlag = true;
		}

		// �v���C���[�ƓG�̊Ԃ̏�Q�����m
		for (auto& object : jsonObject)
		{
			if (object->GetType() == "box" || object->GetType() == "wall")
			{
				object->Update();
				XMFLOAT3 boxPos = object->GetPosition();
				XMFLOAT3 boxScale = object->GetCollisionScale();

				// ��Q�������m���Ă�����U�����Ă��Ȃ�(�v���C���[���G�ɍU�����邱�Ƃ͂ł��Ȃ�)
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
						targetEnemyPos = ePos;
						minEnemyLength = length;
					}
				}
			}
		}
		enemy->Update();
	}
}

void TutorialScene::RopeUpdate()
{
	rope->Update(pPos);

	// �|�[���ƓG�̋������r���ĒZ��������
	float minLength = (std::min)(minEnemyLength, minPoleLength);

	// �ǂ��������̋����������ꍇ�A�G��D�悷��
	if (minEnemyLength < baseLength && minPoleLength < baseLength)
	{
		minLength = minEnemyLength;
	}

	// �^�[�Q�b�g���G�������ꍇ
	if (minLength == minEnemyLength)
	{
		targetPos = targetEnemyPos;
		playerAttackFlag = true;
	}
	// �^�[�Q�b�g���|�[���������ꍇ
	else
	{
		targetPos = targetPolePos;
		throwFlag = true;
	}

	// ��Q�������m������^�[�Q�b�g���Ȃ�
	for (auto& object : jsonObject)
	{
		if (object->GetType() == "box" || object->GetType() == "wall")
		{
			XMFLOAT3 pos = object->GetPosition();
			XMFLOAT3 scale = object->GetCollisionScale();
			if (GetLength(pPos, pos) >= baseLength)
			{
				continue;
			}

			if (Collision::CollisionRayBox(pPos, targetPos, pos, scale))
			{
				minLength = baseLength;
				rope->SetThrowFlag(false);
				throwFlag = false;
				playerAttackFlag = false;
				break;
			}
		}
	}

	// ��Q�������m���Ă��Ȃ����
	if (minLength < baseLength)
	{
		// �ߋ��Ƀ^�[�Q�b�g�����|�[���ɂ͔������Ȃ�
		if (minLength == minPoleLength && GetLength(targetPos, oldTargetPos) <= 0.0f)
		{
			minLength = minEnemyLength;
			targetPos = targetEnemyPos;
			throwFlag = false;
		}

		// �^�[�Q�b�g���������ɂ���΃^�[�Q�b�g�G�t�F�N�g�𔭐�������
		if (minLength < baseLength)
		{
			effectTarget->TargetEffect(
				targetPos, 3.0f, 1.0f,
				{ 1.0f, 0.0f, 0.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				targetEffectCount
			);

			// �^�[�Q�b�g���Ă�������Ƀv���C���[������
			player->TrackRot(pPos, targetPos);
			camera->SetTarget(targetPos);
		}
		else
		{
			float cameraRot = camera->CameraAngle(pPos);
			// �J�����������Ă�������Ƀv���C���[������
			player->GetObj()->SetRotation({ 0, XMConvertToDegrees(cameraRot), 0 });
			rope->SetThrowFlag(false);
			throwFlag = false;
			playerAttackFlag = false;
		}

		// ���[�v��ڒ�������ːi���J�n����
		if (!rushFlag && rFlag)
		{
			rushFlag = true;
			catchPos = targetPos;
		}

		rope->Throw(pPos, targetPos, minLength);
	}
	else
	{
		float cameraRot = camera->CameraAngle(pPos);
		// �J�����������Ă�������Ƀv���C���[������
		player->GetObj()->SetRotation({ 0, XMConvertToDegrees(cameraRot), 0 });
		rope->SetThrowFlag(false);
		throwFlag = false;
		playerAttackFlag = false;
	}

	// �ߋ��Ƀ^�[�Q�b�g���������I�u�W�F�N�g������̋�������Ă�����
	if (GetLength(pPos, oldTargetPos) > 30.0f)
	{
		// �ߋ��Ƀ^�[�Q�b�g�����I�u�W�F�N�g�̍��W�����Z�b�g����
		oldTargetPos = { 1000.0f, 1000.0f, 1000.0f };
	}

	targetPolePos = {};
	minPoleLength = 15.0f;
	targetEnemyPos = {};
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
			player->Update();
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
			newEnemy->SetRespawnPos(pos);
			newEnemy->Update();
			enemies.push_back(std::move(newEnemy));
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

		// �I�u�W�F�N�g�̃^�C�v���Z�b�g
		newObject->SetType(objectData.objType);
		newObject->Update();

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

		// �^�C�v���Ƃɏ��������s����Update
		if (object->GetType() == "box")
		{
			XMFLOAT3 boxPos = object->GetPosition();
			XMFLOAT3 boxScale = object->GetCollisionScale();
			player->MapCollide(boxPos, boxScale);
			pPos = player->GetObj()->GetPosition();

			for (std::unique_ptr<Enemy>& enemy : enemies)
			{
				enemy->MapCollide(boxPos, boxScale);
			}
		}

		else if (object->GetType() == "wall")
		{
			XMFLOAT3 wallPos = object->GetPosition();
			XMFLOAT3 wallScale = object->GetCollisionScale();
			player->MapCollide(wallPos, wallScale);
			pPos = player->GetObj()->GetPosition();

			for (std::unique_ptr<Enemy>& enemy : enemies)
			{
				enemy->MapCollide(wallPos, wallScale);
			}
		}

		else if (object->GetType() == "stage")
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

			for (std::unique_ptr<Enemy>& enemy : enemies)
			{
				enemy->StageCollide(stagePos, stageScale);
			}
		}

		else if (object->GetType() == "pole")
		{
			pPos = player->GetObj()->GetPosition();
			rFlag = rope->GetrFlag();
			XMFLOAT3 polePos = object->GetPosition();
			XMFLOAT3 poleScale = object->GetCollisionScale();
			float length = GetLength(pPos, polePos);

			// �|�[���̒������ԋ߂������̃|�[�������߂�
			if (length < minPoleLength && length < baseLength)
			{
				minPoleLength = length;
				targetPolePos = polePos;
			}
			
			// �v���C���[���|�[���ɐG�ꂽ�烍�[�v�̐ڒ����Ȃ����ēːi���~�߂�
			if (rushFlag && player->PoleCollide(polePos, poleScale))
			{
				rope->SetrFlag(false);
				rushFlag = false;
				catchPos = {};
				elapsedTime = 0.0f;
				oldTargetPos = polePos;
			}
		}

		object->Update();
	}
}
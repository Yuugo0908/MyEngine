#include "TitleScene.h"
#include <cassert>
#include "SceneManager.h"

void TitleScene::Initialize()
{
	// �^�C�g���摜����
	title = Image2d::Create(Image2d::ImgNumber::titleNum, { 0.0f,0.0f });
	title->SetSize({ 1280.0f,720.0f });

	// ���C�g�̐���
	light = Light::Create();
	// ���C�g�̐F��ݒ�
	light->SetLightColor({ 1.0f, 1.0f, 1.0f });
	// 3D�I�u�W�F�N�g�Ƀ��C�g���Z�b�g
	Object3d::SetLight(light);

	jsonObjectInit("tutorial");
}

void TitleScene::Finalize()
{
	levelData = nullptr;
	safe_delete(title);
	jsonObject.erase(jsonObject.begin(), jsonObject.end());
}

void TitleScene::Update()
{
	camera->TitleSceneCameraRotation();
	if (!changeFlag)
	{
		FadeScene::GetInstance()->FadeOut(1.0f);
	}

	if (FadeScene::fadeOutEnd && keyboard->TriggerKey(DIK_SPACE) || controller->GetPadState(Controller::State::A, Controller::Type::TRIGGER))
	{
		changeFlag = true;
	}

	if (changeFlag)
	{
		FadeScene::GetInstance()->FadeIn(0.0f);
		if (FadeScene::fadeInEnd)
		{
			SceneManager::GetInstance()->ChangeScene("Tutorial");
		}
	}

	jsonObjectUpdate();
}

void TitleScene::Draw()
{
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
	for (auto& object : jsonObject)
	{
		object->Draw();
	}

	// 3D�I�u�W�F�N�g�`��㏈��
	Object3d::PostDraw();
#pragma endregion 3D�I�u�W�F�N�g�`��

#pragma region �O�i�摜�`��
	// �O�i�摜�`��O����
	Image2d::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	// �O�i�摜�̕`��
	title->Draw();

	// �t�F�[�h�̕`��
	FadeScene::GetInstance()->Draw();

	// �f�o�b�O�e�L�X�g�̕`��
	DebugText::GetInstance()->DrawAll(DirectXCommon::GetInstance()->GetCommandList());
	// �摜�`��㏈��
	Image2d::PostDraw();
#pragma endregion �O�i�摜�`��
}

void TitleScene::jsonObjectInit(const std::string sceneName)
{
	// ���x���f�[�^�̓ǂݍ���
	levelData = LevelLoader::LoadFile(sceneName);

	// ���x���f�[�^����I�u�W�F�N�g�𐶐��A�z�u
	for (LevelData::ObjectData& objectData : levelData->objects)
	{
		if (objectData.fileName == "player")
		{
			continue;
		}

		if (objectData.fileName == "enemy")
		{
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

		// �I�u�W�F�N�g�̃^�C�v���Z�b�g
		newObject->SetType(objectData.objType);

		// �z��ɓo�^
		jsonObject.push_back(std::move(newObject));
	}
}

void TitleScene::jsonObjectUpdate()
{
	for (auto& object : jsonObject)
	{
		object->Update();
	}
}

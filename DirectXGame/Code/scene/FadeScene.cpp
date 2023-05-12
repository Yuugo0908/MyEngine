#include "FadeScene.h"

FadeScene::FadeScene()
{
}

FadeScene::~FadeScene()
{
}

FadeScene* FadeScene::GetInstance()
{
	static FadeScene instance;

	return &instance;
}

void FadeScene::Initialize()
{
	if (!Image2d::LoadTexture(fadeNum, L"Resources/fade.png"))
	{
		assert(0);
	}
	fadeTex = Image2d::Create(fadeNum, { 0.0f,0.0f });
	fadeTex->SetSize({ 1280.0f,720.0f });
	fadeTex->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });

	// ���C�g�̐���
	light = Light::Create();
	// ���C�g�̐F��ݒ�
	light->SetLightColor({ 1.0f, 1.0f, 1.0f });
	// 3D�I�u�W�F�N�g�Ƀ��C�g���Z�b�g
	Object3d::SetLight(light);

	jsonObjectInit("fade");
}

void FadeScene::Finalize()
{
	safe_delete(light);
	safe_delete(fadeTex);
}

void FadeScene::Update()
{
	switch (fadeState)
	{
	case None:
		fadeInEnd = false;
		fadeOutEnd = false;
		break;
	case FadeInPlay:
		if (!fadeInEnd && alpha <= 1.0f)
		{
			alpha += 0.02f;
		}
		else
		{
			fadeState = FadeInEnd;
		}
		break;
	case FadeInEnd:
		fadeInEnd = true;
		break;
	case FadeOutPlay:
		if (!fadeOutEnd && alpha > 0.0f)
		{
			alpha -= 0.02f;
		}
		else
		{
			fadeState = FadeOutEnd;
		}
		break;
	case FadeOutEnd:
		fadeOutEnd = true;
		break;
	default:
		break;
	}

	fadeTex->SetColor({ 1.0f, 1.0f, 1.0f, alpha });
}

void FadeScene::Draw()
{
	fadeTex->Draw();
}

void FadeScene::jsonObjectInit(const std::string sceneName)
{
	// ���x���f�[�^�̓ǂݍ���
	levelData = LevelLoader::LoadFile(sceneName);

	// ���x���f�[�^����I�u�W�F�N�g�𐶐��A�z�u
	for (LevelData::ObjectData& objectData : levelData->objects)
	{

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

void FadeScene::FadeIn(float alpha)
{
	if (fadeState != FadeInPlay && fadeState != FadeInEnd)
	{
		this->alpha = alpha;
		fadeInEnd = false;

		// �t�F�[�h�C���J�n
		fadeState = FadeInPlay;
	}
}

void FadeScene::FadeOut(float alpha)
{
	if (fadeState != FadeOutPlay && fadeState != FadeOutEnd)
	{
		this->alpha = alpha;
		fadeOutEnd = false;

		// �t�F�[�h�A�E�g�J�n
		fadeState = FadeOutPlay;
	}
}

void FadeScene::reset()
{
	fadeState = None;
}

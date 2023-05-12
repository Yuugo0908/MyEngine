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

	// ライトの生成
	light = Light::Create();
	// ライトの色を設定
	light->SetLightColor({ 1.0f, 1.0f, 1.0f });
	// 3Dオブジェクトにライトをセット
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
	// レベルデータの読み込み
	levelData = LevelLoader::LoadFile(sceneName);

	// レベルデータからオブジェクトを生成、配置
	for (LevelData::ObjectData& objectData : levelData->objects)
	{

		// 3Dオブジェクトを生成
		std::unique_ptr<Object3d> newObject = Object3d::Create();

		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(levelData->models)::iterator it = levelData->models.find(objectData.fileName);
		if (it != levelData->models.end())
		{
			model = it->second;
		}

		newObject->SetModel(model);

		// 座標
		XMFLOAT3 pos;
		XMStoreFloat3(&pos, objectData.trans);
		newObject->SetPosition(pos);

		// 回転角
		XMFLOAT3 rot;
		XMStoreFloat3(&rot, objectData.rot);
		newObject->SetRotation(rot);

		// 大きさ
		XMFLOAT3 scale;
		XMStoreFloat3(&scale, objectData.scale);
		newObject->SetScale(scale);

		// オブジェクトのタイプをセット
		newObject->SetType(objectData.objType);

		// 配列に登録
		jsonObject.push_back(std::move(newObject));
	}
}

void FadeScene::FadeIn(float alpha)
{
	if (fadeState != FadeInPlay && fadeState != FadeInEnd)
	{
		this->alpha = alpha;
		fadeInEnd = false;

		// フェードイン開始
		fadeState = FadeInPlay;
	}
}

void FadeScene::FadeOut(float alpha)
{
	if (fadeState != FadeOutPlay && fadeState != FadeOutEnd)
	{
		this->alpha = alpha;
		fadeOutEnd = false;

		// フェードアウト開始
		fadeState = FadeOutPlay;
	}
}

void FadeScene::reset()
{
	fadeState = None;
}

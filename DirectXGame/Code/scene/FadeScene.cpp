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
	fade = Image2d::Create(fadeNum, { 0.0f,0.0f });
	fade->SetSize({ 1280.0f,720.0f });
	fade->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
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

	fade->SetColor({ 1.0f, 1.0f, 1.0f, alpha });
}

void FadeScene::Draw()
{
	fade->Draw();
}

void FadeScene::reset()
{
	fadeState = None;
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

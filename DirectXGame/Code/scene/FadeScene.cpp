#include "FadeScene.h"

bool FadeScene::fadeInEnd = false;
bool FadeScene::fadeOutEnd = false;

FadeScene::FadeScene()
{
}

FadeScene::~FadeScene()
{
	fade.erase(fade.begin(), fade.end());
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

	for (int i = 0; i < fadeNum; i++)
	{
		fadeImg = Image2d::Create(fadeNum, { 0.0f,0.0f });
		fadeImg->SetPosition({ 80.0f * i, 0.0f });
		fadeImg->SetSize({ 80.0f ,720.0f });
		fadeImg->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });
		fade.push_back(fadeImg);
	}
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
		if (!fadeInEnd && alpha <= 2.0f && fadeCount <= fadeNum - 1)
		{
			alpha += 0.2f;
		}
		else
		{
			fadeState = FadeInEnd;
		}
		break;
	case FadeInEnd:
		fadeInEnd = true;
		fadeCount = 0;
		break;
	case FadeOutPlay:
		if (!fadeOutEnd && alpha > 0.0f && fadeCount <= fadeNum - 1)
		{
			alpha -= 0.2f;
		}
		else
		{
			fadeState = FadeOutEnd;
		}
		break;
	case FadeOutEnd:
		fadeOutEnd = true;
		fadeCount = 0;
		break;
	default:
		break;
	}

	if (fadeState == FadeInPlay && alpha > 1.0f && fadeCount < fadeNum - 1)
	{
		alpha = 0.0f;
		fadeCount++;
	}
	else if(fadeState == FadeOutPlay && alpha <= 0.0f && fadeCount < fadeNum - 1)
	{
		alpha = 1.0f;
		fadeCount++;
	}

	fade[fadeCount]->SetColor({ 1.0f, 1.0f, 1.0f, alpha });
}

void FadeScene::Draw()
{
	for (int i = 0; i < fade.size(); i++)
	{
		fade[i]->Draw();
	}
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

#include "TitleScene.h"
#include <cassert>
#include <SceneManager.h>

void TitleScene::Initialize() {

	// オーディオの初期化
	if (!audio->Initialize())
	{
		assert(0);
	}

	// デバッグテキスト用テクスチャ読み込み
	if (!Image2d::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png"))
	{
		assert(0);
	}

	// デバッグテキスト初期化
	DebugText::GetInstance()->Initialize(debugTextTexNumber);

	// タイトル画像読み込み
	if (!Image2d::LoadTexture(titleNum, L"Resources/title.png"))
	{
		assert(0);
	}
	title = Image2d::Create(titleNum, { 0.0f,0.0f });
	title->SetSize({ 1280.0f,720.0f });


	if (!Image2d::LoadTexture(fadeNum, L"Resources/fade.png"))
	{
		assert(0);
	}
	fadeTex = Image2d::Create(fadeNum, { 0.0f,0.0f });
	fadeTex->SetSize({ 1280.0f,720.0f });
	fadeTex->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });

	if (!Image2d::LoadTexture(expNum, L"Resources/Operation_Explanation.png"))
	{
		assert(0);
	}
	explanation = Image2d::Create(expNum, { 0.0f,0.0f });
	explanation->SetSize({ 1280.0f,720.0f });
	explanation->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });

	if (!Image2d::LoadTexture(backNum, L"Resources/backGround.png"))
	{
		assert(0);
	}
	backGround = Image2d::Create(backNum, { 0.0f,0.0f });
	backGround->SetSize({ 1280.0f,720.0f });
	backGround->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
}

void TitleScene::Finalize()
{

}

void TitleScene::Update()
{
	if (!fadeIn && keyboard->TriggerKey(DIK_SPACE) || controller->GetPadState(Controller::State::A, Controller::Type::TRIGGER))
	{
		backGround->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		// 操作説明はプレイ中、一度だけ表示する
		if (!expFlag)
		{
			expFlag = true;
			title->SetColor({ 0.0f, 0.0f, 0.0f, 0.0f });
			explanation->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		}

		else if (!fadeIn)
		{
			fadeIn = true;
		}
	}

	if (fadeIn)
	{
		alpha += 0.02f;
		fadeTex->SetColor({ 1.0f, 1.0f, 1.0f, alpha });
		if (alpha >= 1.0f)
		{
			fadeIn = false;
			fadeOut = true;
			explanation->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });
			backGround->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });
			SceneManager::GetInstance()->ChangeScene("Game");
		}
	}
}

void TitleScene::Draw()
{
#pragma region 背景画像描画
	// 背景画像描画前処理
	Image2d::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	backGround->Draw();

	// 画像描画後処理
	Image2d::PostDraw();
	// 深度バッファクリア
	DirectXCommon::GetInstance()->ClearDepthBuffer();
#pragma endregion 背景画像描画

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	// 3Dオブクジェクトの描画

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();
#pragma endregion 3Dオブジェクト描画

#pragma region 前景画像描画
	// 前景画像描画前処理
	Image2d::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	// 前景画像の描画

	title->Draw();
	explanation->Draw();
	fadeTex->Draw();

	// デバッグテキストの描画
	DebugText::GetInstance()->DrawAll(DirectXCommon::GetInstance()->GetCommandList());
	// 画像描画後処理
	Image2d::PostDraw();
#pragma endregion 前景画像描画
}

void TitleScene::LightUpdate()
{
	//光線方向初期値
	static XMVECTOR lightDir = { 5, -5, 5, 0 };

	light->SetLightDir(lightDir);
	light->Update();
}

void TitleScene::CameraUpdate()
{

}
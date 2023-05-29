#include "GameOverScene.h"
#include "SceneManager.h"

void GameOverScene::Initialize()
{
	if (!Image2d::LoadTexture(GameOverNum, L"Resources/GameOver.png"))
	{
		assert(0);
	}
	GameOver = Image2d::Create(GameOverNum, { 0.0f,0.0f });
	GameOver->SetSize({ 1280.0f,720.0f });

	if (!Image2d::LoadTexture(backNum, L"Resources/backGround.png"))
	{
		assert(0);
	}
	backGround = Image2d::Create(backNum, { 0.0f,0.0f });
	backGround->SetSize({ 1280.0f,720.0f });
	backGround->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
}

void GameOverScene::Finalize()
{
	safe_delete(GameOver);
	safe_delete(backGround);
}

void GameOverScene::Update()
{
	ClipCursor(NULL);
	if (!retryFlag)
	{
		FadeScene::GetInstance()->FadeOut(1.0f);
	}

	if (keyboard->TriggerKey(DIK_SPACE) || controller->GetPadState(Controller::State::A, Controller::Type::TRIGGER))
	{
		FadeScene::GetInstance()->reset();
		SceneManager::GetInstance()->ChangeScene("Title");
	}
	else if (keyboard->TriggerKey(DIK_R))
	{
		retryFlag = true;
	}

	if (retryFlag)
	{
		FadeScene::GetInstance()->FadeIn(0.0f);
		bool fadeIn = FadeScene::GetInstance()->GetFadeInEnd();
		if (fadeIn)
		{
			SceneManager::GetInstance()->ChangeScene("Game");
		}
	}
}

void GameOverScene::Draw()
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
	GameOver->Draw();

	// フェードの描画
	FadeScene::GetInstance()->Draw();

	// デバッグテキストの描画
	DebugText::GetInstance()->DrawAll(DirectXCommon::GetInstance()->GetCommandList());
	// 画像描画後処理
	Image2d::PostDraw();
#pragma endregion 前景画像描画
}

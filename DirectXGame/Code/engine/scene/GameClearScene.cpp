#include "GameClearScene.h"
#include "SceneManager.h"

void GameClearScene::Initialize()
{
	// ゲームクリア画像読み込み
	if (!Image2d::LoadTexture(GameClearNum, "GameClear"))
	{
		assert(0);
	}
	GameClear = Image2d::Create(GameClearNum, { 0.0f,0.0f });
	GameClear->SetSize({ 1280.0f,720.0f });

	if (!Image2d::LoadTexture(fadeNum, "fade"))
	{
		assert(0);
	}
	fadeTex = Image2d::Create(fadeNum, { 0.0f,0.0f });
	fadeTex->SetSize({ 1280.0f,720.0f });
	fadeTex->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });

	if (!Image2d::LoadTexture(backNum, "backGround"))
	{
		assert(0);
	}
	backGround = Image2d::Create(backNum, { 0.0f,0.0f });
	backGround->SetSize({ 1280.0f,720.0f });
	backGround->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
}

void GameClearScene::Finalize()
{
	safe_delete(GameClear);
	safe_delete(fadeTex);
	safe_delete(backGround);
}

void GameClearScene::Update()
{
	ClipCursor(NULL);

	FadeScene::GetInstance()->FadeOut(1.0f);

	if (keyboard->TriggerKey(DIK_SPACE) || controller->GetPadState(Controller::State::A, Controller::Type::TRIGGER))
	{
		FadeScene::GetInstance()->reset();
		SceneManager::GetInstance()->ChangeScene("Title");
	}
}

void GameClearScene::Draw()
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
	GameClear->Draw();

	// フェードの描画
	FadeScene::GetInstance()->Draw();

	// デバッグテキストの描画
	DebugText::GetInstance()->DrawAll(DirectXCommon::GetInstance()->GetCommandList());
	// 画像描画後処理
	Image2d::PostDraw();
#pragma endregion 前景画像描画
}

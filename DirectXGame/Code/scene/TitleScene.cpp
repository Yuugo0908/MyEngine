#include "TitleScene.h"
#include "TitleScene.h"
#include <imgui.h>
#include <cassert>

TitleScene::TitleScene() {

}

TitleScene::~TitleScene() {

}

void TitleScene::Initialize() {
	// デバイスのセット
	FbxObject3d::SetDevice(dxCommon->GetDevice());
	// グラフィックスパイプライン生成
	FbxObject3d::CreateGraphicsPipeline();

	// タイトル画像読み込み
	if (!Image2d::LoadTexture(1, L"Resources/title.png"))
	{
		assert(0);
	}
	title = Image2d::Create(1, { 0.0f,0.0f });
	title->SetSize({ 1280.0f,720.0f });

	if (!Image2d::LoadTexture(8, L"Resources/black_backGround.png"))
	{
		assert(0);
	}
	backGround = Image2d::Create(8, { 0.0f,0.0f });
	backGround->SetSize({ 1280.0f,720.0f });
	backGround->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f });

	if (!Image2d::LoadTexture(9, L"Resources/Operation_Explanation.png"))
	{
		assert(0);
	}
	explanation = Image2d::Create(9, { 0.0f,0.0f });
	explanation->SetSize({ 1280.0f,720.0f });
	explanation->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f });
}

void TitleScene::Finalize()
{

}

void TitleScene::Update() {

	if (!fadeIn && keyboard->TriggerKey(DIK_SPACE))
	{

		if (!expFlag)
		{
			expFlag = true;
			explanation->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f });
		}

		else if (!fadeIn)
		{
			fadeIn = true;
		}
	}

	if (fadeIn)
	{
		alpha += 0.02f;
		backGround->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, alpha });
		if (alpha >= 1.0f)
		{
			//nowScene = game_;
			fadeIn = false;
			fadeOut = true;
			explanation->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f });
		}
	}
}

void TitleScene::Draw() {
#pragma region 背景画像描画
	// 背景画像描画前処理
	Image2d::PreDraw(dxCommon->GetCommandList());


	// 画像描画後処理
	Image2d::PostDraw();
	// 深度バッファクリア
	dxCommon->ClearDepthBuffer();
#pragma endregion 背景画像描画

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(dxCommon->GetCommandList());

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();
#pragma endregion 3Dオブジェクト描画

#pragma region 前景画像描画
	// 前景画像描画前処理
	Image2d::PreDraw(dxCommon->GetCommandList());

	// 前景画像の描画

	title->Draw();
	explanation->Draw();
	backGround->Draw();

	// デバッグテキストの描画
	DebugText::GetInstance()->DrawAll(dxCommon->GetCommandList());
	// 画像描画後処理
	Image2d::PostDraw();
#pragma endregion 前景画像描画
}
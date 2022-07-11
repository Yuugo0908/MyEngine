#include "WinApp.h"
#include "DirectXCommon.h"
#include "Audio.h"
#include "GameScene.h"
#include "FbxLoader.h"
#include "PostEffect.h"

//# Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//ポインタ置き場
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	Keyboard* keyboard = nullptr;
	Audio* audio = nullptr;
	GameScene* gameScene = nullptr;
	Camera* camera = nullptr;
	PostEffect* postEffect = nullptr;
	Image2d* image2d = nullptr;

	// ゲームウィンドウの作成
	win = new WinApp();
	win->CreateGameWindow();

	//DirectX初期化処理
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(win);

#pragma region 汎用機能初期化

	//入力の初期化
	keyboard = new Keyboard();
	keyboard->Initialize(win->GetInstance(), win->GetHwnd());

	// カメラ初期化
	camera = new Camera();
	camera->Initialize(WinApp::window_width, WinApp::window_height);

	// FBXの初期化
	FbxLoader::GetInstance()->Initialize(dxCommon->GetDevice());

	// オーディオの初期化
	audio = new Audio();
	if (!audio->Initialize())
	{
		assert(0);
		return 1;
	}
	// 画像静的初期化
	if (!Image2d::StaticInitialize(dxCommon->GetDevice()))
	{
		assert(0);
		return 1;
	}

	// 3Dオブジェクト静的初期化
	if (!Object3d::StaticInitialize(dxCommon->GetDevice()))
	{
		assert(0);
		return 1;
	}

	// ポストエフェクト用のテクスチャ読み込み
	//Image2d::LoadTexture(100, L"Resources/white1x1.png");
	
	Image2d::LoadTexture(1, L"Resources/background.png");
	image2d = Image2d::Create(1, { 0, 0 });
	image2d->SetSize({ 1280.0f, 720.0f });
	// ポストエフェクトの初期化
	postEffect = new PostEffect();
	postEffect->Initialize(dxCommon->GetDevice());

#pragma endregion 汎用機能初期化

	// ゲームシーンの初期化
	gameScene = new GameScene();
	gameScene->Initialize(dxCommon, keyboard, audio);

	while (true)  // ゲームループ
	{
		// メッセージ処理
		if (win->ProcessMessage())
		{
			break;
		}

#pragma region DirectX毎フレーム処理

		// 入力関連の毎フレーム処理
		keyboard->Update();
		// ゲームシーンの毎フレーム処理
		gameScene->Update();

#pragma endregion DirectX毎フレーム処理

#pragma region グラフィックスコマンド

		//レンダーテクスチャへの描画開始
		postEffect->PreDraw(dxCommon->GetCommandList());
		// ゲームシーンの描画
		gameScene->Draw();
		//レンダーテクスチャへの描画終了
		postEffect->PostDraw(dxCommon->GetCommandList());
		
		//// 背景画像描画前処理
		//Image2d::PreDraw(dxCommon->GetCommandList());
		//// 背景画像描画
		//image2d->Draw();
		//// 画像描画後処理
		//Image2d::PostDraw();

		// 描画開始
		dxCommon->PreDraw();
		// ポストエフェクトの描画
		postEffect->Draw(dxCommon->GetCommandList());
		// 描画終了
		dxCommon->PostDraw();
#pragma endregion グラフィックスコマンド
	}

	//解放
	// 各種解放
	safe_delete(gameScene);
	safe_delete(audio);
	safe_delete(keyboard);
	safe_delete(dxCommon);
	safe_delete(camera);
	safe_delete(postEffect);
	safe_delete(image2d);
	FbxLoader::GetInstance()->Finalize();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();
	safe_delete(win);

	return 0;
}
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Audio.h"
#include "GameScene.h"
#include "FbxLoader.h"
#include "PostEffect.h"
#include "MultiRT.h"
#include "MultiTex.h"
#include "Controller.h"
#include "Light.h"
#include "Mouse.h"

//# Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//ポインタ置き場
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	Keyboard* keyboard = nullptr;
	Mouse* mouse = nullptr;
	Audio* audio = nullptr;
	GameScene* gameScene = nullptr;
	Camera* camera = nullptr;
	PostEffect* postEffect = nullptr;
	Image2d* image2d = nullptr;
	MultiTex* multiTex = nullptr;
	MultiRT* multiRT = nullptr;
	Controller* controller = nullptr;
	Light* light = nullptr;

	// ゲームウィンドウの作成
	win = new WinApp();
	win->CreateGameWindow();

	//DirectX初期化処理
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(win);

#pragma region 汎用機能初期化

	//入力の初期化
	keyboard = Keyboard::GetInstance();
	keyboard->Initialize(win);
	controller = Controller::GetInstance();
	controller->Initialize(win);
	mouse = Mouse::GetInstance();
	mouse->Initialize(win);

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
	// ライト静的初期化
	Light::StaticInitialize(dxCommon->GetDevice());
	
	// ポストエフェクトの初期化
	multiTex = new MultiTex();
	multiTex->Initialize(dxCommon->GetDevice());

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
		controller->Update();
		mouse->Update();
		// ゲームシーンの毎フレーム処理
		gameScene->Update();

#pragma endregion DirectX毎フレーム処理

#pragma region グラフィックスコマンド
		//// 背景画像描画前処理
		//Image2d::PreDraw(dxCommon->GetCommandList());
		//// 背景画像描画
		//image2d->Draw();
		//// 画像描画後処理
		//Image2d::PostDraw();

		// 描画開始
		dxCommon->PreDraw();
		// ゲームシーンの描画
		gameScene->Draw();
		// 描画終了
		dxCommon->PostDraw();

		////レンダーテクスチャへの描画開始
		//multiTex->PreDraw(dxCommon->GetCommandList());
		//// ポストエフェクトの描画
		//multiTex->Draw(dxCommon->GetCommandList());
		////レンダーテクスチャへの描画終了
		//multiTex->PostDraw(dxCommon->GetCommandList());
#pragma endregion グラフィックスコマンド
	}

	//解放
	// 各種解放
	safe_delete(gameScene);
	safe_delete(audio);
	//safe_delete(keyboard);
	//safe_delete(controller);
	safe_delete(dxCommon);
	safe_delete(camera);
	safe_delete(postEffect);
	safe_delete(multiTex);
	safe_delete(image2d);
	FbxLoader::GetInstance()->Finalize();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();
	safe_delete(win);

	return 0;
}
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Audio.h"
#include "GameScene.h"
#include "FbxLoader.h"
#include "Controller.h"
#include "Light.h"
#include "Mouse.h"

//# Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	//ポインタ置き場
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	Keyboard* keyboard = nullptr;
	Mouse* mouse = nullptr;
	Audio* audio = nullptr;
	GameScene* gameScene = nullptr;
	Camera* camera = nullptr;
	Image2d* image2d = nullptr;
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
	mouse = Mouse::GetInstance();
	mouse->Initialize(win);

	// カメラ初期化
	camera = Camera::GetInstance();
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
	if (!Object3d::StaticInitialize(dxCommon->GetDevice(), camera))
	{
		assert(0);
		return 1;
	}
	// ライト静的初期化
	if (!Light::StaticInitialize(dxCommon->GetDevice()))
	{
		assert(0);
		return 1;
	}

#pragma endregion 汎用機能初期化

	// ゲームシーンの初期化
	gameScene = new GameScene();
	gameScene->Initialize(dxCommon, audio);

	while (true)  // ゲームループ
	{
		// メッセージ処理
		if (win->ProcessMessage() || keyboard->PushKey(DIK_ESCAPE))
		{
			break;
		}

#pragma region DirectX毎フレーム処理
		if (GetActiveWindow())
		{
			// 入力関連の毎フレーム処理
			keyboard->Update();
			controller->Update();
			mouse->Update();
			// ゲームシーンの毎フレーム処理
			gameScene->Update();
		}

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
	// 終了時にカーソル移動の制限を解除
	ClipCursor(NULL);
	safe_delete(gameScene);
	safe_delete(audio);
	safe_delete(image2d);
	safe_delete(light);
	safe_delete(dxCommon);
	FbxLoader::GetInstance()->Finalize();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();
	safe_delete(win);

	return 0;
}
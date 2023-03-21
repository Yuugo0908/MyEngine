#include "Framework.h"

void Framework::Initialize()
{
	// ゲームウィンドウの作成
	win = new WinApp();
	win->CreateGameWindow();

	//DirectX初期化処理
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

	//入力の初期化
	keyboard = Keyboard::GetInstance();
	keyboard->Initialize(win);
	controller = Controller::GetInstance();
	mouse = Mouse::GetInstance();
	mouse->Initialize(win);

	// カメラ初期化
	camera = Camera::GetInstance();
	camera->Initialize(WinApp::window_width, WinApp::window_height);


	// 画像静的初期化
	if (!Image2d::StaticInitialize(dxCommon->GetDevice()))
	{
		assert(0);
	}

	// 3Dオブジェクト静的初期化
	if (!Object3d::StaticInitialize(dxCommon->GetDevice(), camera))
	{
		assert(0);
	}
	// ライト静的初期化
	if (!Light::StaticInitialize(dxCommon->GetDevice()))
	{
		assert(0);
	}

	// ゲームシーンの初期化
	gameScene = new GameScene();
	gameScene->Initialize();
}

void Framework::Finalize()
{
	// 終了時にカーソル移動の制限を解除
	ClipCursor(NULL);
	//解放
	gameScene->Finalize();
	safe_delete(gameScene);
	safe_delete(audio);
	safe_delete(image2d);
	safe_delete(light);

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();
	safe_delete(win);
}

void Framework::Update()
{
	if (GetActiveWindow())
	{
		// メッセージ処理
		if (win->ProcessMessage() || keyboard->PushKey(DIK_ESCAPE))
		{
			// ゲームループを抜ける
			endRequest_ = true;
			return;
		}

		// 入力関連の毎フレーム処理
		keyboard->Update();
		controller->Update();
		mouse->Update();
		// ゲームシーンの毎フレーム処理
		gameScene->Update();
	}
}

void Framework::Draw()
{
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
}

void Framework::Run()
{
	// ゲームの初期化
	Initialize();

	while (true)
	{
		// 毎フレーム更新
		Update();

		if (IsEndRequest())
		{
			break;
		}

		Draw();
	}

	Finalize();
}

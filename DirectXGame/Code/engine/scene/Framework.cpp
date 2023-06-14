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

	// オーディオの初期化
	audio = Audio::GetInstance();
	audio->Initialize();

	// カメラ初期化
	camera = Camera::GetInstance();
	camera->Initialize(WinApp::window_width, WinApp::window_height);

	// ポストエフェクト初期化
	postEffect = new PostEffect;
	postEffect->Initialize(dxCommon->GetDevice());

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
	if (!Light::GetInstance()->StaticInitialize(dxCommon->GetDevice()))
	{
		assert(0);
	}

	// デバッグテキスト用テクスチャ読み込み
	if (!Image2d::LoadTexture(Image2d::ImgNumber::debugTextNum, "debugfont"))
	{
		assert(0);
	}
	// デバッグテキスト初期化
	DebugText::GetInstance()->Initialize(Image2d::ImgNumber::debugTextNum);
}

void Framework::Finalize()
{
	//解放
	safe_delete(sceneFactory_);
	safe_delete(postEffect);
	firstBootFlag = false;

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();
	safe_delete(win);
}

void Framework::Update()
{
	// メッセージ処理
	if (win->ProcessMessage() || keyboard->TriggerKey(DIK_ESCAPE))
	{
		// ゲームループを抜ける
		endRequest_ = true;
		return;
	}

	// 非アクティブ状態なら更新しない(初回起動は除く)
	if (firstBootFlag && win->GetHwnd() != GetForegroundWindow())
	{
		ClipCursor(NULL);
		return;
	}

	// 入力関連の毎フレーム処理
	keyboard->Update();
	controller->Update();
	mouse->Update();
	// フェードのアップデート
	FadeScene::GetInstance()->Update();
	// ゲームシーンの毎フレーム処理
	SceneManager::GetInstance()->Update();
}

void Framework::Draw()
{
	// 描画開始
	dxCommon->PreDraw();
	// ゲームシーンの描画
	SceneManager::GetInstance()->Draw();
	// 描画終了
	dxCommon->PostDraw();
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
		firstBootFlag = true;
	}

	Finalize();
}

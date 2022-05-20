#include "GameScene.h"
#include <cassert>

GameScene::GameScene() {

}

GameScene::~GameScene() {

}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio) {
	// nullptrチェック
	assert(dxCommon);
	assert(input);
	assert(audio);

	this->dxCommon = dxCommon;
	this->input = input;
	this->playAudio = audio;

	camera = new Camera();
	// デバイスのセット
	FbxObject3d::SetDevice(dxCommon->GetDevice());
	// カメラのセット
	FbxObject3d::SetCamera(camera);

	// デバッグテキスト用テクスチャ読み込み
	if (!Sprite::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png")) {
		assert(0);
	}
	// デバッグテキスト初期化
	debugText.Initialize(debugTextTexNumber);

	// テクスチャ読み込み
	if (!Sprite::LoadTexture(0, L"Resources/title.png")) {
		assert(0);
	}

	if (!Sprite::LoadTexture(1, L"Resources/HPText.png")) {
		assert(0);
	}

	if (!Sprite::LoadTexture(2, L"Resources/HPBar.png")) {
		assert(0);
	}

	if (!Sprite::LoadTexture(3, L"Resources/PlayerHPGauge.png")) {
		assert(0);
	}

	if (!Sprite::LoadTexture(4, L"Resources/TimerGauge.png")) {
		assert(0);
	}

	if (!Sprite::LoadTexture(5, L"Resources/GameClear.png")) {
		assert(0);
	}

	if (!Sprite::LoadTexture(6, L"Resources/GameOver.png")) {
		assert(0);
	}
	//スプライト
	title = Sprite::Create(0, { 0.0f,0.0f });
	title->SetSize({ 1280.0f,720.0f });

	GameClear = Sprite::Create(5, { 0.0f,0.0f });
	GameClear->SetSize({ 1280.0f,720.0f });

	GameOver = Sprite::Create(6, { 0.0f,0.0f });
	GameOver->SetSize({ 1280.0f,720.0f });

<<<<<<< HEAD
	//.objの名前を指定してモデルを読み込む
	playerModel = playerModel->CreateFromObject("RedBox");
	skydomeModel = skydomeModel->CreateFromObject("skydome");

	//モデル名を指定して読み込み
	FbxLoader::GetInstance()->LoadModelFromFile("cube");
=======
	// カメラの設定
	camera->SetTarget({ 0, 1, 0 });
	camera->SetEye({ 0, 0, -90 });

	// .objの名前を指定してモデルを読み込む
	playerModel = playerModel->CreateFromObject("sphere");
	skydomeModel = skydomeModel->CreateFromObject("skydome");

	 //モデル名を指定して読み込み
	fbxModel = FbxLoader::GetInstance()->LoadModelFromFile("cube");
	//FbxLoader::GetInstance()->LoadTexture(fbxModel, "Resources/cube/Create.jpg");
>>>>>>> Error

	// 3Dオブジェクト生成
	playerObj = Object3d::Create();
	skydomeObj = Object3d::Create();

	// 3Dオブジェクトにモデルを割り当てる
	playerObj->SetModel(playerModel);
	skydomeObj->SetModel(skydomeModel);

	playerObj->SetPosition({ 0.0f, 0.0f, 0.0f });
	playerObj->SetScale({ 5.0f,5.0f,5.0f });
	skydomeObj->SetScale({ 1.0f, 1.0f, 1.0f });
	fbxObject->SetPosition({ 0.0f, -20.0f, 0.0f });

	p_pos = playerObj->GetPosition();
}

void GameScene::Update() {
	// 現在の座標を取得
	XMFLOAT3 cameraEye = camera->GetEye();
	XMFLOAT3 cameraTarget = camera->GetTarget();

	if (input->TriggerKey(DIK_SPACE))
	{
		p_flag = true;
	}

	if (p_flag == true) {
		p_pos.y -= p_val;
		p_val += p_gra;
	}

	if (p_pos.y <= -60.0f)
	{
		p_flag = false;
		p_pos.y = 30.0f;
		p_val = 0.0f;
	}

	playerObj->SetPosition(p_pos);
	camera->SetEye(cameraEye);
	camera->SetTarget(cameraTarget);
	playerObj->Update();
	skydomeObj->Update();
}

void GameScene::reset() {

}

void GameScene::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();
#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());
	// 背景スプライト描画
	//backGround->Draw();
	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon->ClearDepthBuffer();
#pragma endregion 背景スプライト描画
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(dxCommon->GetCommandList());
	// 3Dオブクジェクトの描画

	playerObj->Draw();
	skydomeObj->Draw();
<<<<<<< HEAD
=======
	fbxObject->Draw(dxCommon->GetCommandList());
>>>>>>> Error

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();
#pragma endregion 3Dオブジェクト描画
#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	// 前景スプライトの描画

	// デバッグテキストの描画
	debugText.DrawAll(dxCommon->GetCommandList());
	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion 前景スプライト描画
}
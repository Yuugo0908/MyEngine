#include "GameScene.h"
#include "imgui/imgui.h"
#include <cassert>

GameScene::GameScene() {

}

GameScene::~GameScene() {

}

void GameScene::Initialize(DirectXCommon* dxCommon, Keyboard* keyboard, Audio* audio) {
	// nullptrチェック
	assert(dxCommon);
	assert(keyboard);
	assert(audio);

	this->dxCommon = dxCommon;
	this->keyboard = keyboard;
	this->playAudio = audio;

	camera = new Camera();
	// デバイスのセット
	FbxObject3d::SetDevice(dxCommon->GetDevice());
	// カメラのセット
	FbxObject3d::SetCamera(camera);
	// グラフィックスパイプライン生成
	FbxObject3d::CreateGraphicsPipeline();

	// デバッグテキスト用テクスチャ読み込み
	if (!Image2d::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png")) {
		assert(0);
	}
	// デバッグテキスト初期化
	debugText.Initialize(debugTextTexNumber);

	// テクスチャ読み込み
	if (!Image2d::LoadTexture(0, L"Resources/title.png")) {
		assert(0);
	}

	if (!Image2d::LoadTexture(1, L"Resources/background.png")) {
		assert(0);
	}

	if (!Image2d::LoadTexture(2, L"Resources/HPBar.png")) {
		assert(0);
	}

	if (!Image2d::LoadTexture(3, L"Resources/PlayerHPGauge.png")) {
		assert(0);
	}

	if (!Image2d::LoadTexture(4, L"Resources/TimerGauge.png")) {
		assert(0);
	}

	if (!Image2d::LoadTexture(5, L"Resources/GameClear.png")) {
		assert(0);
	}

	if (!Image2d::LoadTexture(6, L"Resources/GameOver.png")) {
		assert(0);
	}
	//画像
	title = Image2d::Create(0, { 0.0f,0.0f });
	title->SetSize({ 1280.0f,720.0f });

	backGround = Image2d::Create(1, { 0, 0 });
	backGround->SetSize({ 1280.0f, 720.0f });

	GameClear = Image2d::Create(5, { 0.0f,0.0f });
	GameClear->SetSize({ 1280.0f,720.0f });

	GameOver = Image2d::Create(6, { 0.0f,0.0f });
	GameOver->SetSize({ 1280.0f,720.0f });

	// カメラの設定
	camera->SetTarget({ 0, 2.5f, 0 });
	camera->SetEye({ 0, 0, -30.0f });

	// .objの名前を指定してモデルを読み込む
	playerModel = playerModel->CreateFromObject("sphere");
	skydomeModel = skydomeModel->CreateFromObject("skydome");

	//モデル名を指定して読み込み
	fbxModel = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");

	// 3Dオブジェクト生成
	playerObj = Object3d::Create();
	skydomeObj = Object3d::Create();

	fbxObject = new FbxObject3d;
	fbxObject->Initialize();
	fbxObject->SetModel(fbxModel);

	// 3Dオブジェクトにモデルを割り当てる
	playerObj->SetModel(playerModel);
	skydomeObj->SetModel(skydomeModel);

	playerObj->SetPosition({ 5.0f, 5.0f, 0.0f });
	playerObj->SetScale({ 1.0f,1.0f,1.0f });
	skydomeObj->SetScale({ 5.0f, 5.0f, 5.0f });
	fbxObject->SetPosition({ 0.0f, 0.0f, 0.0f });
	fbxObject->SetScale({ 1.0f, 1.0f, 1.0f });
	fbxObject->SetRotation({0, 90, 0});

	p_pos = playerObj->GetPosition();
}

void GameScene::Finalize()
{
	safe_delete(fbxModel);
	safe_delete(fbxObject);
}

void GameScene::Update() {
	// 現在の座標を取得
	XMFLOAT3 cameraEye = camera->GetEye();
	XMFLOAT3 cameraTarget = camera->GetTarget();

	Move();

	playerObj->SetPosition(p_pos);
	camera->SetEye(cameraEye);
	camera->SetTarget(cameraTarget);
	fbxObject->Update();
	playerObj->Update();
	skydomeObj->Update();
}

void GameScene::reset() {

}

void GameScene::Draw() {	
	
	//ImGui::Begin("Test");
	//ImGui::SetWindowSize(ImVec2(500, 200));
	//ImGui::SliderFloat("p_pos.x", &p_pos.x, 0.0f, 10.0f);
	//ImGui::SliderFloat("p_pos.y", &p_pos.y, 0.0f, 10.0f);
	//ImGui::SliderFloat("p_pos.z", &p_pos.z, 0.0f, 10.0f);
	//ImGui::End();
	
#pragma region 背景画像描画
	// 背景画像描画前処理
	//Image2d::PreDraw(dxCommon->GetCommandList());
	// 背景画像描画
	//backGround->Draw();
	// 画像描画後処理
	//Image2d::PostDraw();
	// 深度バッファクリア
	//dxCommon->ClearDepthBuffer();
#pragma endregion 背景画像描画

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(dxCommon->GetCommandList());
	// 3Dオブクジェクトの描画

	playerObj->Draw();
	//skydomeObj->Draw();
	fbxObject->Draw(dxCommon->GetCommandList());

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();
#pragma endregion 3Dオブジェクト描画

#pragma region 前景画像描画
	// 前景画像描画前処理
	Image2d::PreDraw(dxCommon->GetCommandList());

	// 前景画像の描画
	
	// デバッグテキストの描画
	debugText.DrawAll(dxCommon->GetCommandList());
	// 画像描画後処理
	Image2d::PostDraw();
#pragma endregion 前景画像描画
}

void GameScene::Move()
{
	if (keyboard->TriggerKey(DIK_SPACE))
	{
		p_flag = true;
	}

	if (p_flag == true) {
		p_pos.y -= p_val;
		p_val += p_gra;
	}

	if (p_pos.y <= -30.0f)
	{
		p_flag = false;
		p_pos.y = 5.0f;
		p_val = 0.0f;
	}
}
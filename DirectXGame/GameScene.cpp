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
	if (!Image2d::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png"))
	{
		assert(0);
	}
	// デバッグテキスト初期化
	debugText.Initialize(debugTextTexNumber);

	// テクスチャ読み込み

	if (!Image2d::LoadTexture(1, L"Resources/background.png")) {
		assert(0);
	}

	// カメラの設定
	camera->SetTarget({ 0, 1.0f, 0 });
	camera->SetEye({ 0.0f, 3.0f, -10.0f });

	// .objの名前を指定してモデルを読み込む
	playerModel = playerModel->CreateFromObject("sphere");
	enemyModel = enemyModel->CreateFromObject("GreenBox");
	skydomeModel = skydomeModel->CreateFromObject("skydome");

	//モデル名を指定して読み込み
	fbxModel = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");

	// 3Dオブジェクト生成
	player = Object3d::Create();
	enemy = Object3d::Create();
	skydomeObj = Object3d::Create();

	fbxObject = new FbxObject3d;
	fbxObject->Initialize();
	fbxObject->SetModel(fbxModel);

	// 3Dオブジェクトにモデルを割り当てる
	player->SetModel(playerModel);
	enemy->SetModel(enemyModel);
	skydomeObj->SetModel(skydomeModel);

	player->SetPosition({ 4.0f, 0.0f, 0.0f });
	player->SetScale({ 1.5f,1.5f,1.5f });
	enemy->SetPosition({ -4.0f, 0.0f, 0.0f });
	enemy->SetScale({ 0.5f, 0.5f, 0.5f });

	skydomeObj->SetScale({ 5.0f, 5.0f, 5.0f });
	fbxObject->SetPosition({ 0.0f, 0.0f, 0.0f });
	fbxObject->SetScale({ 1.0f, 1.0f, 1.0f });
	fbxObject->SetRotation({0, 90, 0});

	p_pos = player->GetPosition();
	p_sca = player->GetScale();
	e_pos = enemy->GetPosition();
	e_sca = enemy->GetScale();

	// ライトの生成
	light = Light::Create();
	// ライトの色を設定
	light->SetLightColor({ 1, 1, 1 });
	// 3Dオブジェクトにライトをセット
	Object3d::SetLight(light);
	FbxObject3d::SetLight(light);
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
	LightUpdate();

	if (Collision::GetInstance()->CollisionObject(player, enemy))
	{
		p_flag = true;
	}

	player->SetPosition(p_pos);
	player->SetScale(p_sca);
	enemy->SetPosition(e_pos);
	enemy->SetScale(e_sca);

	camera->SetEye(cameraEye);
	camera->SetTarget(cameraTarget);
	fbxObject->Update();
	player->Update();
	enemy->Update();
	skydomeObj->Update();
	light->Update();
}

void GameScene::reset() {

}

void GameScene::Draw() {	
	
	//SetImgui();
	
#pragma region 背景画像描画
	// // 背景画像描画前処理
	//Image2d::PreDraw(dxCommon->GetCommandList());
	// // 背景画像描画
	//backGround->Draw();
	// // 画像描画後処理
	//Image2d::PostDraw();
	// // 深度バッファクリア
	//dxCommon->ClearDepthBuffer();
#pragma endregion 背景画像描画

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(dxCommon->GetCommandList());

	// 3Dオブクジェクトの描画
	player->Draw();
	enemy->Draw();
	skydomeObj->Draw();
	//fbxObject->Draw(dxCommon->GetCommandList());

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
	// オブジェクトの回転
	{
		XMFLOAT3 rot = player->GetRotation();
		rot.y += 1.0f;
		player->SetRotation(rot);
		enemy->SetRotation(rot);
		fbxObject->SetRotation(rot);
	}

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
		p_pos.y = 0.0f;
		p_val = 0.0f;
	}

	if (keyboard->PushKey(DIK_D))
	{
		e_pos.x += 0.2f;
	}
	if (keyboard->PushKey(DIK_A))
	{
		e_pos.x -= 0.2f;
	}
}

void GameScene::SetImgui()
{
	ImGui::Begin("Test");
	ImGui::SetWindowSize(ImVec2(500, 200));
	ImGui::SliderFloat("p_pos.x", &p_pos.x, 0.0f, 10.0f);
	ImGui::SliderFloat("p_pos.y", &p_pos.y, 0.0f, 10.0f);
	ImGui::SliderFloat("p_pos.z", &p_pos.z, 0.0f, 10.0f);
	ImGui::SliderFloat("p_sca.x", &p_sca.x, 0.0f, 10.0f);
	ImGui::SliderFloat("p_sca.y", &p_sca.y, 0.0f, 10.0f);
	ImGui::SliderFloat("p_sca.z", &p_sca.z, 0.0f, 10.0f);
	ImGui::End();
}

void GameScene::RopeMove(XMFLOAT3& pos)
{

}

void GameScene::LightUpdate()
{
	//光線方向初期値
	static XMVECTOR lightDir = { 0, 1, 5, 0 };

	if		(keyboard->PushKey(DIK_UP)) { lightDir.m128_f32[1] += 1.0f; }
	else if (keyboard->PushKey(DIK_DOWN)) { lightDir.m128_f32[1] -= 1.0f; }
	if		(keyboard->PushKey(DIK_RIGHT)) { lightDir.m128_f32[0] += 1.0f; }
	else if (keyboard->PushKey(DIK_LEFT)) { lightDir.m128_f32[0] -= 1.0f; }

	light->SetLightDir(lightDir);
}

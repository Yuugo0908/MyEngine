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

	// .objの名前を指定してモデルを読み込む
	playerModel		= playerModel->CreateFromObject("sphere");
	enemyModel		= enemyModel->CreateFromObject("sphere");
	skydomeModel	= skydomeModel->CreateFromObject("skydome");
	stageModel		= stageModel->CreateFromObject("stage");
	ropeModel		= ropeModel->CreateFromObject("rope");

	// 3Dオブジェクト生成
	player = Object3d::Create();
	enemy = Object3d::Create();
	skydome = Object3d::Create();
	stage = Object3d::Create();
	rope = Object3d::Create();

	// 3Dオブジェクトにモデルを割り当てる
	player->SetModel(playerModel);
	enemy->SetModel(enemyModel);
	skydome->SetModel(skydomeModel);
	stage->SetModel(stageModel);
	rope->SetModel(ropeModel);

	player->SetPosition({ 4.0f, 0.0f, 0.0f });
	player->SetScale({ 1.0f,1.0f,1.0f });
	enemy->SetPosition({ -4.0f, 0.0f, 0.0f });
	enemy->SetScale({ 1.0f, 1.0f, 1.0f });

	stage->SetPosition({ 0.0f, -1.5f, 0.0f });
	skydome->SetPosition({ 0.0f, 12.0f, 0.0f });
	skydome->SetScale({ 5.0f, 5.0f, 5.0f });

	p_pos = player->GetPosition();
	e_pos = enemy->GetPosition();

	// カメラの設定
	camera->SetTarget(p_pos);
	camera->SetEye({0, 3.0f, -10.0f});

	// ライトの生成
	light = Light::Create();
	// ライトの色を設定
	light->SetLightColor({ 0.8f, 0.0f, 0.6f });
	// 3Dオブジェクトにライトをセット
	Object3d::SetLight(light);
	FbxObject3d::SetLight(light);
}

void GameScene::Finalize()
{
}

void GameScene::Update() {

	CameraUpdate();
	PlayerMove();
	LightUpdate();

	player->SetPosition(p_pos);
	enemy->SetPosition(e_pos);
	rope->SetPosition(r_pos);

	player->Update();
	enemy->Update();
	skydome->Update();
	stage->Update();
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
	skydome->Draw();
	stage->Draw();

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

void GameScene::PlayerMove()
{
	// ジャンプ
	if (keyboard->TriggerKey(DIK_SPACE) && p_flag == false)
	{
		p_flag = true;
		// 上昇率の更新
		p_val = 1.25f;
	}
	if (p_flag == true) {
		p_val -= gra;
		p_pos.y += p_val;
		if (p_pos.y <= 0.0f)
		{
			p_flag = false;
			p_pos.y = 0.0f;
			p_val = 0.0f;
		}
	}

	// 移動
	if (keyboard->PushKey(DIK_D))
	{
		p_pos.x += 0.2f;
	}
	else if (keyboard->PushKey(DIK_A))
	{
		p_pos.x -= 0.2f;
	}
	if (keyboard->PushKey(DIK_W))
	{
		p_pos.z += 0.2f;
	}
	else if (keyboard->PushKey(DIK_S))
	{
		p_pos.z -= 0.2f;
	}

	// 自機の突進
	if (keyboard->TriggerKey(DIK_Q) && easeFlag == false)
	{
		easeFlag = true;
		startPos = p_pos;
		endPos = e_pos;
	}

	if (easeFlag == true)
	{
		avoidTime += 0.05f;
		avoidTimeRate = min(avoidTime / avoidEndTime, 1);

		p_pos = Easing::easeOut(startPos, endPos, avoidTimeRate);
	}

	if (avoidTimeRate == 1.0f)
	{
		avoidTime = 0.0f;
		avoidTimeRate = 0.0f;
		easeFlag = false;
	}
}

void GameScene::EnemyMove()
{
}

void GameScene::SetImgui()
{
	ImGui::Begin("Test");
	ImGui::SetWindowSize(ImVec2(500, 200));
	ImGui::SliderFloat("p_pos.x", &p_pos.x, 0.0f, 10.0f);
	ImGui::SliderFloat("p_pos.y", &p_pos.y, 0.0f, 10.0f);
	ImGui::SliderFloat("p_pos.z", &p_pos.z, 0.0f, 10.0f);
	ImGui::End();
}

void GameScene::RopeUpdate()
{
	// ロープの位置を更新
	rope->SetPosition({ r_pos });
	rope->Update();

	//ロープの位置を取得
	XMFLOAT3 get_r_pos = rope->GetPosition();

	//プレイヤーとエネミーの距離
	XMFLOAT3 length = { p_pos.x - e_pos.x, p_pos.y - e_pos.y, p_pos.z - e_pos.z };
	float len = GetLength(p_pos, e_pos);

	//最大値より大きいなら
	if (len > max_rope)
	{
		float wq = len / max_rope;
		len = max_rope;
		e_pos = { p_pos.x - length.x / wq, p_pos.y - length.y / wq, p_pos.z - length.z / wq };
	}

	angleX = PosForAngle(p_pos.y, r_pos.z, r_pos.y, p_pos.z);

	rope->SetPosition({ (p_pos.x + e_pos.x) / 2, (p_pos.y + e_pos.y) / 2, (p_pos.z + e_pos.z) / 2});
	rope->SetScale({ 0.1f, len / 2 , 0.1f });
	rope->SetRotation({ 0, XMConvertToDegrees(angleX), 0 });
	rope->Update();
}

void GameScene::LightUpdate()
{
	//光線方向初期値
	static XMVECTOR lightDir = { 5, -5, 5, 0 };

	light->SetLightDir(lightDir);
	light->Update();
}

void GameScene::CameraUpdate()
{
	// 現在の座標を取得
	XMFLOAT3 cameraEye = camera->GetEye();
	XMFLOAT3 cameraTarget = camera->GetTarget();

	camera->SetEye({ p_pos.x, p_pos.y + 3.0f, p_pos.z - 10.0f });
	camera->SetTarget(p_pos);
}

float GameScene::GetLength(XMFLOAT3 pos_a, XMFLOAT3 pos_b)
{
	XMFLOAT3 len = { pos_a.x - pos_b.x, pos_a.y - pos_b.y, pos_a.z - pos_b.z };

	return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
}

float GameScene::PosForAngle(float startPosX, float startPosY, float endPosX, float endPosY)
{
	float angleX = endPosX - startPosX;
	float angleY = endPosY - startPosY;

	float resultAngle = (float)atan2(angleX, angleY);

	return resultAngle;
}

float GameScene::ease_in(float t, float b, float c, float d)
{
	float x = t / d;
	float v = ease_in_cubic(x);
	float ret = c * v + b;
	return ret;
}

float GameScene::ease_in_cubic(float x)
{
	return x * x * x;
}

#include "GameScene.h"
#include <imgui.h>
#include <cassert>

GameScene::GameScene() {

}

GameScene::~GameScene() {

}

void GameScene::Initialize(DirectXCommon* dxCommon, Keyboard* keyboard, Controller* controller, Mouse* mouse, Audio* audio) {
	// nullptrチェック
	assert(dxCommon);
	assert(keyboard);
	assert(controller);
	assert(mouse);
	assert(audio);

	this->dxCommon = dxCommon;
	this->keyboard = keyboard;
	this->controller = controller;
	this->mouse = mouse;
	this->audio = audio;

	camera = new Camera(WinApp::window_width, WinApp::window_height, mouse);

	// デバイスのセット
	FbxObject3d::SetDevice(dxCommon->GetDevice());
	// カメラのセット
	FbxObject3d::SetCamera(camera);
	// グラフィックスパイプライン生成
	FbxObject3d::CreateGraphicsPipeline();

	// 3Dオブジェクトにカメラをセット
	Object3d::SetCamera(camera);

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
	ropeModel		= ropeModel->CreateFromObject("redBox");

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

	player->SetPosition({ 0.0f, 0.0f, -5.0f });
	player->SetScale({ 1.0f,1.0f,1.0f });
	enemy->SetPosition({ 0.0f, 0.0f, 5.0f });
	enemy->SetScale({ 1.0f, 1.0f, 1.0f });

	stage->SetPosition({ 0.0f, -1.0f, 0.0f });
	skydome->SetPosition({ 0.0f, 12.0f, 0.0f });
	skydome->SetScale({ 5.0f, 5.0f, 5.0f });

	p_pos = player->GetPosition();
	e_pos = enemy->GetPosition();

	rope->SetPosition(p_pos);
	rope->SetScale({ 0.05f, 0.05f, 1.0f });
	r_pos = rope->GetPosition();

	// カメラの設定
	camera->SetTarget(p_pos);

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
	PlayerUpdate();
	EnemyUpdate();
	LightUpdate();
	RopeUpdate();

	if (Collision::CollisionObject(player, enemy))
	{
		r_flag = true;
	}

	player->SetPosition(p_pos);
	enemy->SetPosition(e_pos);
	rope->SetPosition(r_pos);

	skydome->Update();
	stage->Update();
}

void GameScene::reset()
{

}

void GameScene::Draw() {	
	
	SetImgui();
	
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
	if (r_flag)
	{
		rope->Draw();
	}

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

void GameScene::PlayerUpdate()
{
	// ジャンプ
	if (keyboard->TriggerKey(DIK_SPACE) && p_flag == false)
	{
		p_flag = true;
		// 上昇率の更新
		p_val = 1.25f;
	}
	if (p_flag == true) {
		p_val -= p_gra;
		p_pos.y += p_val;
		if (p_pos.y <= 0.0f)
		{
			p_flag = false;
			p_pos.y = 0.0f;
			p_val = 0.0f;
		}
	}

	// 移動
	if (!easeFlag)
	{
		float rate = 1.0f;
		// 移動量の倍数計算
		if (keyboard->PushKey(DIK_A) || keyboard->PushKey(DIK_D))
		{
			if (keyboard->PushKey(DIK_W) || keyboard->PushKey(DIK_S))
			{
				rate = 0.71f;
			}
		}

		p_move = 0.2f * rate;

		if (keyboard->PushKey(DIK_D))
		{
			p_pos.x += p_move;
		}
		else if (keyboard->PushKey(DIK_A))
		{
			p_pos.x -= p_move;
		}
		if (keyboard->PushKey(DIK_W))
		{
			p_pos.z += p_move;
		}
		else if (keyboard->PushKey(DIK_S))
		{
			p_pos.z -= p_move;
		}
	}

	PlayerRush();

	player->SetPosition(p_pos);
	player->Update();
}

void GameScene::PlayerRush()
{
	// ロープがついている場合、敵を引き寄せる
	if (r_flag)
	{
		if (keyboard->PushKey(DIK_Q) && easeFlag == false)
		{
			if (keyboard->TriggerKey(DIK_DOWN))
			{
				easeFlag = true;
				startPos = e_pos;
				endPos = p_pos;
			}
		}

		if (easeFlag == true)
		{
			avoidTime += 0.5f;
			avoidTimeRate = min(avoidTime / avoidEndTime, 1);

			e_pos = Easing::easeIn(startPos, endPos, avoidTimeRate);
		}
		if (avoidTimeRate == 1.0f)
		{
			avoidTime = 0.0f;
			avoidTimeRate = 0.0f;
			easeFlag = false;
		}
		return;
	}

	// 自機の突進
	if (keyboard->TriggerKey(DIK_Q) && easeFlag == false)
	{
		easeFlag = true;
		startPos = p_pos;
		endPos = p_pos;
		if (keyboard->PushKey(DIK_D))
		{
			endPos.x += avoidMove;
		}
		else if (keyboard->PushKey(DIK_A))
		{
			endPos.x -= avoidMove;
		}
		if (keyboard->PushKey(DIK_W))
		{
			endPos.z += avoidMove;
		}
		else if (keyboard->PushKey(DIK_S))
		{
			endPos.z -= avoidMove;
		}
	}

	if (easeFlag == true)
	{
		avoidTime += 0.5f;
		avoidTimeRate = min(avoidTime / avoidEndTime, 1);

		p_pos = Easing::easeIn(startPos, endPos, avoidTimeRate);
	}
	if (avoidTimeRate == 1.0f)
	{
		avoidTime = 0.0f;
		avoidTimeRate = 0.0f;
		easeFlag = false;
	}
}

void GameScene::EnemyUpdate()
{
	if (e_pos.y > 0.0f)
	{
		e_val -= e_gra;
		e_pos.y += e_val;
		if(e_pos.y <= 0.0f)
		{
			e_pos.y = 0.0f;
			e_val = 0.0f;
		}
	}
	enemy->Update();
}

void GameScene::SetImgui()
{
	ImGui::Begin("Test");
	ImGui::SetWindowSize(ImVec2(300, 200));
	ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);

	ImGui::Text("cameraPosX: %6.2f", c_pos.x);
	ImGui::Text("cameraPosY: %6.2f", c_pos.y);
	ImGui::Text("cameraPosZ: %6.2f", c_pos.z);
	ImGui::End();
}

void GameScene::RopeUpdate()
{
	if (r_flag == false)
	{
		rope->SetPosition(p_pos);
		rope->SetScale({ 0.05f, 0.05f, 1.0f });
		r_pos = rope->GetPosition();
		return;
	}

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
	
	// Y軸周りの角度
	angleY = (float)atan2(p_pos.x - e_pos.x, p_pos.z - e_pos.z);
	vecXZ = sqrtf((p_pos.x - e_pos.x) * (p_pos.x - e_pos.x) + (p_pos.z - e_pos.z) * (p_pos.z - e_pos.z));
	// X軸周りの角度
	angleX = (float)atan2(e_pos.y - p_pos.y, vecXZ);

	rope->SetPosition({ (p_pos.x + e_pos.x) / 2, (p_pos.y + e_pos.y) / 2, (p_pos.z + e_pos.z) / 2});
	rope->SetScale({ 0.05f, 0.05f , len / 7 });
	rope->SetRotation({ XMConvertToDegrees(angleX), XMConvertToDegrees(angleY), 0});
	rope->Update();

	if (keyboard->PushKey(DIK_E))
	{
		r_flag = false;
	}
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
	camera->SetEye({ p_pos.x, p_pos.y + 3.0f, p_pos.z - 10.0f });
	camera->SetTarget(p_pos);
	c_pos = camera->GetEye();
	camera->Update();
}

float GameScene::GetLength(XMFLOAT3 pos_a, XMFLOAT3 pos_b)
{
	XMFLOAT3 len = { pos_a.x - pos_b.x, pos_a.y - pos_b.y, pos_a.z - pos_b.z };

	return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
}

float GameScene::PosForAngle(float startPos1, float startPos2, float endPos1, float endPos2)
{
	float angle1 = endPos1 - startPos1;
	float angle2 = endPos2 - startPos2;

	float resultangle = atan2(angle2, angle1);

	return resultangle;
}

void GameScene::CircularMotion(XMFLOAT3& pos, const XMFLOAT3 center_pos, const float r, int& angle, const int add)
{
	angle += add;

	pos.x = (cosf(3.14 / 180.0f * angle) * r) + center_pos.x; // 円運動の処理
	pos.y = (sinf(3.14 / 180.0f * angle) * r) + center_pos.y;
	pos.z = (tanf(3.14 / 180.0f * angle) * r) + center_pos.z;
}

XMFLOAT3 GameScene::normalize(XMFLOAT3 p1, XMFLOAT3 p2)
{
	float length = GetLength(p1, p2);

	XMFLOAT3 identity;
	identity.x = p2.x - p1.x / length;
	identity.y = p2.y - p1.y / length;
	identity.z = p2.z - p1.z / length;

	return identity;
}

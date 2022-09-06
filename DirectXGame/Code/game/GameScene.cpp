#include "GameScene.h"
#include <imgui.h>
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

	camera = new Camera(WinApp::window_width, WinApp::window_height);
	mouse = new Mouse();

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

	stage->SetPosition({ 0.0f, -1.5f, 0.0f });
	skydome->SetPosition({ 0.0f, 12.0f, 0.0f });
	skydome->SetScale({ 5.0f, 5.0f, 5.0f });

	p_pos = player->GetPosition();
	e_pos = enemy->GetPosition();

	// カメラの設定
	camera->SetTarget(p_pos);
	camera->SetEye({ p_pos.x, p_pos.y + 3.0f, p_pos.z - 10.0f });

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
	if (Collision::CollisionObject(player, enemy))
	{
		r_flag = true;
	}

	RopeUpdate();

	player->SetPosition(p_pos);
	enemy->SetPosition(e_pos);
	rope->SetPosition(r_pos);

	player->Update();
	enemy->Update();
	skydome->Update();
	stage->Update();
	camera->Update();
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
	if (!easeFlag)
	{
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

	player->SetPosition(p_pos);

	if (easeFlag == true)
	{
		avoidTime += 0.2f;
		avoidTimeRate = min(avoidTime / avoidEndTime, 1);

		p_pos = Easing::easeInOut(startPos, endPos, avoidTimeRate);
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
	ImGui::SetWindowSize(ImVec2(300, 200));
	ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);

	ImGui::Text("playerPosX: %6.2f", p_pos.x);
	ImGui::Text("playerPosY: %6.2f", p_pos.y);
	ImGui::Text("playerPosZ: %6.2f", p_pos.z);
	ImGui::Text("playerPos = cameraPos");
	ImGui::End();
}

void GameScene::RopeUpdate()
{
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
	c_pos = camera->GetEye();

	camera->SetEye({ p_pos.x, p_pos.y + 3.0f, p_pos.z - 10.0f });
	camera->SetTarget(p_pos);

	//bool dirty = false;
	//float angleX = 0;
	//float angleY = 0;

	//// マウスの入力を取得
	//Mouse::MouseMove mouseMove = mouse->GetMouseMove();

	//// マウスの左ボタンが押されていたらカメラを回転させる
	//if (mouse->PushMouseLeft())
	//{
	//	float dy = mouseMove.MouseX * scaleY;
	//	float dx = mouseMove.MouseY * scaleX;

	//	angleX = -dx * XM_PI;
	//	angleY = -dy * XM_PI;
	//	dirty = true;
	//}

	//if (dirty) {
	//	// 追加回転分の回転行列を生成
	//	XMMATRIX matRotNew = XMMatrixIdentity();
	//	matRotNew *= XMMatrixRotationX(-angleX);
	//	matRotNew *= XMMatrixRotationY(-angleY);
	//	// 累積の回転行列を合成
	//	// ※回転行列を累積していくと、誤差でスケーリングがかかる危険がある為
	//	// クォータニオンを使用する方が望ましい
	//	matRot = matRotNew * matRot;

	//	// 注視点から視点へのベクトルと、上方向ベクトル
	//	XMVECTOR vTargetEye = { 0.0f, 0.0f, -distance, 1.0f };
	//	XMVECTOR vUp = { 0.0f, 1.0f, 0.0f, 0.0f };

	//	// ベクトルを回転
	//	vTargetEye = XMVector3Transform(vTargetEye, matRot);
	//	vUp = XMVector3Transform(vUp, matRot);

	//	// 注視点からずらした位置に視点座標を決定
	//	const XMFLOAT3& target = camera->GetTarget();
	//	camera->SetEye({ target.x + vTargetEye.m128_f32[0], target.y + vTargetEye.m128_f32[1], target.z + vTargetEye.m128_f32[2] });
	//	camera->SetUp({ vUp.m128_f32[0], vUp.m128_f32[1], vUp.m128_f32[2] });
	//}
	//camera->UpdateViewMatrix();
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

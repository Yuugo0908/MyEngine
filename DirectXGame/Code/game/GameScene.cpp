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
	easing = new Easing;
	player = new Player;
	enemy = new Enemy;
	rope = new Rope;

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
	DebugText::GetInstance()->Initialize(debugTextTexNumber);

	// タイトル画像読み込み
	if (!Image2d::LoadTexture(1, L"Resources/title.png"))
	{
		assert(0);
	}
	title = Image2d::Create(1, { 0.0f,0.0f });
	title->SetSize({ 1280.0f,720.0f });

	// リザルト画像読み込み
	if (!Image2d::LoadTexture(2, L"Resources/result.png"))
	{
		assert(0);
	}
	result = Image2d::Create(2, { 0.0f,0.0f });
	result->SetSize({ 1280.0f,720.0f });

	rope->Initialize();
	player->Initialize(keyboard);
	enemy->Initialize();

	// .objの名前を指定してモデルを読み込む
	skydomeModel	= skydomeModel->CreateFromObject("skydome");
	stageModel		= stageModel->CreateFromObject("stage");

	// 3Dオブジェクト生成
	skydome = Object3d::Create();
	stage = Object3d::Create();

	// 3Dオブジェクトにモデルを割り当てる
	skydome->SetModel(skydomeModel);
	stage->SetModel(stageModel);

	// 各オブジェクトの位置や大きさを初期化
	stage->SetPosition({ 0.0f, -1.0f, 0.0f });
	stage->SetScale({ 0.5f, 0.5f, 1.0f });
	skydome->SetPosition({ 0.0f, 12.0f, 0.0f });
	skydome->SetScale({ 5.0f, 5.0f, 5.0f });

	// カメラの設定
	camera->SetTarget(pPos);

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
	player->Finalize();
	enemy->Finalize();
	rope->Finalize();
}

void GameScene::Update() {

	if (nowScene == 0)
	{
		if (keyboard->TriggerKey(DIK_SPACE))
		{
			nowScene = 1;
		}
	}

	if (nowScene == 1)
	{
		CollisionUpdate();
		LightUpdate();
		CameraUpdate();
		rFlag = rope->GetrFlag();
		moveFlag = rope->GetmoveFlag();

		player->Update(moveFlag);
		enemy->Update();
		rope->Update(pPos, ePos, enemy->GetObj());

		skydome->Update();
		stage->Update();
		mouse->Update();

		if (enemyCount == 5)
		{
			nowScene = 2;
		}
	}

	if (nowScene == 2)
	{
		if (keyboard->TriggerKey(DIK_SPACE))
		{
			reset();
		}
	}
}

void GameScene::reset()
{
	// プレイヤー
	pPos = { 0.0f, 5.0f, -5.0f };//座標
	jumpFlag = false;//自由落下のフラグ
	pMove = 0.0f;//移動量
	pAcc = 0.2f;//加速
	pVal = 0.2f;//速度
	rate = 1.0f; // 斜め移動時の制限

	// エネミー
	ePos = { 0.0f, 5.0f, 5.0f };
	eFlag = false; // 自由落下のフラグ
	eVal = 0.2f; // 速度
	eGra = 0.1f; // 重力
	eAlive = false;// 生きているかのフラグ
	eAliveCount = 0;
	enemyCount = 0; // 倒した数

	// カメラ
	cPos = {};
	cTarget = {};
	cameraLength = {};

	// 突進用
	startPos = {}; // 開始位置
	endPos = {}; // 終点位置
	avoidMove = 5.0f; // 距離
	easeFlag = false; // イージング開始フラグ

	// シーン管理用
	nowScene = 0;

	// シェイク用
	shakeFlag = false;
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
	if (eAlive)
	{
		enemy->Draw();
	}
	skydome->Draw();
	stage->Draw(); 
	rope->Draw();

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();
#pragma endregion 3Dオブジェクト描画

#pragma region 前景画像描画
	// 前景画像描画前処理
	Image2d::PreDraw(dxCommon->GetCommandList());

	// 前景画像の描画
	if (nowScene == 0)
	{
		title->Draw();
	}

	if (nowScene == 2)
	{
		result->Draw();
		if (keyboard->TriggerKey(DIK_SPACE))
		{
			reset();
		}
	}

	// デバッグテキストの描画
	DebugText::GetInstance()->DrawAll(dxCommon->GetCommandList());
	// 画像描画後処理
	Image2d::PostDraw();
#pragma endregion 前景画像描画
}

void GameScene::SetImgui()
{
	ImGui::Begin("DebugText");
	ImGui::SetWindowSize(ImVec2(300, 200));
	ImGui::Separator();
	ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);

	ImGui::Separator();
	ImGui::Text("playerPosX : %6.2f", pPos.x);
	ImGui::Text("playerPosY : %6.2f", pPos.y);
	ImGui::Text("playerPosZ : %6.2f", pPos.z);

	ImGui::Separator();
	ImGui::Text("cameraLength : %6.2f", cameraLength.m128_f32[0]);
	ImGui::Text("cameraLength : %6.2f", cameraLength.m128_f32[1]);
	ImGui::Text("cameraLength : %6.2f", cameraLength.m128_f32[2]);

	ImGui::Separator();
	ImGui::Text("enemyCount : %d", enemyCount);

	ImGui::End();
}

//void GameScene::PlayerUpdate()
//{
//	if (pPos.y > 0.0f)
//	{
//		jumpFlag = true;
//	}
//	else
//	{
//		pPos.y = 0.0f;
//		pVal = 0.0f;
//		jumpFlag = false;
//	}
//
//	// ジャンプ
//	if (keyboard->TriggerKey(DIK_SPACE) && !jumpFlag)
//	{
//		jumpFlag = true;
//		// 上昇率の更新
//		pVal = 1.25f;
//	}
//	if (jumpFlag && !easeFlag)
//	{
//		pVal -= pGra;
//		pPos.y += pVal;
//	}
//
//	PlayerRush();
//
//	// 移動
//	if(moveFlag) 
//	{
//		rate = 1.0f;
//		// 移動量の倍数計算
//		if (keyboard->PushKey(DIK_A) || keyboard->PushKey(DIK_D))
//		{
//			if (keyboard->PushKey(DIK_W) || keyboard->PushKey(DIK_S))
//			{
//				rate = 0.7f;
//			}
//		}
//
//		pMove = pAcc * rate;
//
//		if (keyboard->PushKey(DIK_D))
//		{
//			pPos.x += pMove;
//		}
//		else if (keyboard->PushKey(DIK_A))
//		{
//			pPos.x -= pMove;
//		}
//		if (keyboard->PushKey(DIK_W))
//		{
//			pPos.z += pMove;
//		}
//		else if (keyboard->PushKey(DIK_S))
//		{
//			pPos.z -= pMove;
//		}
//	}
//
//	player->SetPosition(pPos);
//	player->Update();
//}

//void GameScene::PlayerRush()
//{
//	// 自機の突進
//	pMove = avoidMove * rate;
//	if (!rFlag && keyboard->TriggerKey(DIK_K) && cCount == 0)
//	{
//		easeFlag = true;
//		startPos = pPos;
//		endPos = pPos;
//		if (keyboard->PushKey(DIK_D))
//		{
//			endPos.x += pMove;
//		}
//		else if (keyboard->PushKey(DIK_A))
//		{
//			endPos.x -= pMove;
//		}
//		if (keyboard->PushKey(DIK_W))
//		{
//			endPos.z += pMove;
//		}
//		else if (keyboard->PushKey(DIK_S))
//		{
//			endPos.z -= pMove;
//		}
//	}
//	easing->EaseInUpdate(startPos, endPos, pPos, easeFlag, avoidTime);
//}

//void GameScene::EnemyUpdate()
//{
//	if (!eAlive)
//	{
//		eAliveCount++;
//		enemy->SetPosition({ 0.0f, 100.0f, 0.0f });
//
//		if (eAliveCount == 60)
//		{
//			enemy->SetPosition({ 0.0f, 5.0f, 5.0f });
//			eAlive = true;
//			eAliveCount = 0;
//		}
//
//		ePos = enemy->GetPosition();
//		enemy->Update();
//		return;
//	}
//
//	if (ePos.y > 0.0f)
//	{
//		if (!easeFlag)
//		{
//			eVal -= eGra;
//			ePos.y += eVal;
//		}
//	}
//
//	else if (ePos.y <= 0.0f)
//	{
//		ePos.y = 0.0f;
//		eVal = 0.0f;
//	}
//	enemy->SetPosition(ePos);
//	enemy->Update();
//}

void GameScene::LightUpdate()
{
	//光線方向初期値
	static XMVECTOR lightDir = { 5, -5, 5, 0 };

	light->SetLightDir(lightDir);
	light->Update();
}

void GameScene::CameraUpdate()
{
	pPos = player->GetPos();
	camera->SetTarget(pPos);
	cPos = camera->GetEye();
	cTarget = camera->GetTarget();

	cameraLength = { cPos.x - pPos.x, cPos.y - pPos.y, cPos.z - pPos.z, 1.0f };
	cameraLength = XMVector3Normalize(cameraLength);

	//カメラ更新
	if (shakeFlag == true)
	{
		camera->CameraShake(cPos, cTarget, shakeFlag);
	}
	camera->Update();
}

void GameScene::CollisionUpdate()
{
	if (rFlag && Collision::CollisionObject(player->GetObj(), enemy->GetObj()))
	{
		shakeFlag = true;
		eAlive = false;
		rFlag = false;
		moveFlag = true;
		rope->SetrFlag(rFlag);
		rope->SetmoveFlag(moveFlag);
	}
}

float GameScene::GetLength(XMFLOAT3 posA, XMFLOAT3 posB)
{
	XMFLOAT3 len = { posA.x - posB.x, posA.y - posB.y, posA.z - posB.z };

	return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
}

float GameScene::PosForAngle(float startPos1, float startPos2, float endPos1, float endPos2)
{
	float angle1 = endPos1 - startPos1;
	float angle2 = endPos2 - startPos2;

	float resultangle = (float)atan2(angle2, angle1);

	return resultangle;
}

void GameScene::CircularMotion(XMFLOAT3& pos, const XMFLOAT3 centerPos, const float r, int& angle, const int add)
{
	angle += add;

	pos.x = (cosf(3.14f / 180.0f * angle) * r) + centerPos.x; // 円運動の処理
	pos.y = (sinf(3.14f / 180.0f * angle) * r) + centerPos.y;
	pos.z = (tanf(3.14f / 180.0f * angle) * r) + centerPos.z;
}

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

	// 各オブジェクトの位置や大きさを初期化
	player->SetPosition({ 0.0f, 0.0f, -5.0f });
	player->SetScale({ 1.0f,1.0f,1.0f });
	enemy->SetPosition({ 0.0f, 0.0f, 5.0f });
	enemy->SetScale({ 1.0f, 1.0f, 1.0f });

	stage->SetPosition({ 0.0f, -1.0f, 0.0f });
	skydome->SetPosition({ 0.0f, 12.0f, 0.0f });
	skydome->SetScale({ 5.0f, 5.0f, 5.0f });

	pPos = player->GetPosition();
	ePos = enemy->GetPosition();

	rope->SetPosition(pPos);
	rope->SetScale({ 0.2f, 0.2f, 0.2f });

	rPos = rope->GetPosition();
	rScale = rope->GetScale();

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
}

void GameScene::Update() {

	CollisionUpdate();
	LightUpdate();
	CameraUpdate();
	RopeUpdate();
	PlayerUpdate();
	EnemyUpdate();

	skydome->Update();
	stage->Update();
	mouse->Update();
}

void GameScene::reset()
{
	// 各オブジェクトの位置や大きさを初期化
	//player->SetPosition({ 0.0f, 0.0f, -5.0f });
	//player->SetScale({ 1.0f,1.0f,1.0f });
	//enemy->SetPosition({ 0.0f, 0.0f, 5.0f });
	//enemy->SetScale({ 1.0f, 1.0f, 1.0f });

	//stage->SetPosition({ 0.0f, -1.0f, 0.0f });
	//skydome->SetPosition({ 0.0f, 12.0f, 0.0f });
	//skydome->SetScale({ 5.0f, 5.0f, 5.0f });

	//pPos = player->GetPosition();
	//ePos = enemy->GetPosition();

	//rope->SetPosition(pPos);
	//rope->SetScale({ 0.0f, 0.0f, 0.0f });
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
	rope->Draw();

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

void GameScene::SetImgui()
{
	ImGui::Begin("DebugText");
	ImGui::SetWindowSize(ImVec2(300, 200));
	ImGui::Separator();
	ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);

	ImGui::Separator();
	ImGui::Text("cameraPosX : %6.2f", cPos.x);
	ImGui::Text("cameraPosY : %6.2f", cPos.y);
	ImGui::Text("cameraPosZ : %6.2f", cPos.z);

	ImGui::Separator();
	ImGui::Text("cameraLength : %6.2f", cameraLength.m128_f32[0]);
	ImGui::Text("cameraLength : %6.2f", cameraLength.m128_f32[1]);
	ImGui::Text("cameraLength : %6.2f", cameraLength.m128_f32[2]);
	ImGui::Separator();
	ImGui::Text("cCount : %d", cCount);

	ImGui::End();
}

void GameScene::PlayerUpdate()
{
	pPos = player->GetPosition();
	// ジャンプ
	if (keyboard->TriggerKey(DIK_SPACE) && !pFlag)
	{
		pFlag = true;
		// 上昇率の更新
		pVal = 1.25f;
	}
	if (pFlag) {
		pVal -= pGra;
		pPos.y += pVal;
		if (pPos.y <= 0.0f)
		{
			pPos.y = 0.0f;
			pVal = 0.0f;
			pFlag = false;
		}
	}

	// 移動
	if (!easeFlag && !pEaseFlag && !eEaseFlag)
	{
		rate = 1.0f;
		// 移動量の倍数計算
		if (keyboard->PushKey(DIK_A) || keyboard->PushKey(DIK_D))
		{
			if (keyboard->PushKey(DIK_W) || keyboard->PushKey(DIK_S))
			{
				rate = 0.7f;
			}
		}

		pMove = pAcc * rate;

		if (keyboard->PushKey(DIK_D))
		{
			pPos.x += pMove;
		}
		else if (keyboard->PushKey(DIK_A))
		{
			pPos.x -= pMove;
		}
		if (keyboard->PushKey(DIK_W))
		{
			pPos.z += pMove;
		}
		else if (keyboard->PushKey(DIK_S))
		{
			pPos.z -= pMove;
		}
	}

	PlayerRush();

	player->SetPosition(pPos);
	player->Update();
}

void GameScene::PlayerRush()
{
	// ロープがついている場合、敵を引き寄せる
	if (rFlag)
	{
		//pAcc = 0.05f;
		if (mouse->TriggerMouseLeft() && keyboard->PushKey(DIK_W))
		{
			pEaseFlag = true;
		}
		else if (mouse->TriggerMouseLeft() && keyboard->PushKey(DIK_S))
		{
			eEaseFlag = true;
		}

		if (!easeFlag)
		{
			EaseUpdate(pPos, ePos, pPos, pEaseFlag);
			EaseUpdate(ePos, pPos, ePos, eEaseFlag);
		}
		return;
	}

	// 自機の突進
	pMove = avoidMove * rate;
	if (mouse->TriggerMouseRight() && cCount == 0)
	{
		cCount = 60;
		easeFlag = true;
		startPos = pPos;
		endPos = pPos;
		if (keyboard->PushKey(DIK_D))
		{
			endPos.x += pMove;
		}
		else if (keyboard->PushKey(DIK_A))
		{
			endPos.x -= pMove;
		}
		if (keyboard->PushKey(DIK_W))
		{
			endPos.z += pMove;
		}
		else if (keyboard->PushKey(DIK_S))
		{
			endPos.z -= pMove;
		}
	}

	EaseUpdate(startPos, endPos, pPos, easeFlag);
}

void GameScene::EnemyUpdate()
{
	if (ePos.y > 0.0f)
	{
		eVal -= eGra;
		ePos.y += eVal;
		if(ePos.y <= 0.0f)
		{
			ePos.y = 0.0f;
			eVal = 0.0f;
		}
	}
	enemy->SetPosition(ePos);
	enemy->Update();
}

void GameScene::RopeUpdate()
{
	if (!rFlag)
	{
		rPos = player->GetPosition() + manageRopePos;
		rScale = manageRopeScale;
		rope->SetPosition(rPos);
		rope->SetScale(rScale);
		rope->SetRotation({ 0.0f, 0.0f, 0.0f });
		rope->Update();

		if (mouse->TriggerMouseLeft() && !rThrowFlag && !rBackFlag && cCount == 0)
		{
			cCount = 30;
			rThrowFlag = true;
		}

		RopeThrow(rPos, rScale, rThrowFlag);

		return;
	}

	//プレイヤーとエネミーの距離
	XMFLOAT3 length = { pPos.x - ePos.x, pPos.y - ePos.y, pPos.z - ePos.z };
	float len = GetLength(pPos, ePos);

	//最大値より大きいなら
	if (len > maxRope)
	{
		float wq = len / maxRope;
		len = maxRope;
		ePos = { pPos.x - length.x / wq, pPos.y - length.y / wq, pPos.z - length.z / wq };
	}
	
	// Y軸周りの角度
	angleY = (float)atan2(pPos.x - ePos.x, pPos.z - ePos.z);
	vecXZ = sqrtf((pPos.x - ePos.x) * (pPos.x - ePos.x) + (pPos.z - ePos.z) * (pPos.z - ePos.z));
	// X軸周りの角度
	angleX = (float)atan2(ePos.y - pPos.y, vecXZ);

	rPos = { (pPos.x + ePos.x) / 2, (pPos.y + ePos.y) / 2, (pPos.z + ePos.z) / 2 };
	rScale = { 0.2f, 0.2f , len / 2.0f };
	rope->SetPosition(rPos);
	rope->SetScale(rScale);
	rope->SetRotation({ XMConvertToDegrees(angleX), XMConvertToDegrees(angleY), 0});
	rope->Update();
}

void GameScene::RopeThrow(XMFLOAT3& rPos, XMFLOAT3& rScale, bool& flag)
{
	// フラグがtrueじゃない場合は初期化してリターンする
	if (!flag && !rBackFlag)
	{
		manageRopePos = {};
		manageRopeScale = { 0.2f, 0.2f, 0.0f };
		return;
	}

	if (flag && !rBackFlag)
	{
		manageRopePos.z += 0.5f;
		manageRopeScale.z += 0.5f;

		avoidTime += 0.5f;
		avoidTimeRate = min(avoidTime / avoidEndTime, 1);

		rPos = Easing::easeOut(rPos, manageRopePos, avoidTimeRate);
		rScale = Easing::easeOut(rScale, manageRopeScale, avoidTimeRate);

		if (avoidTimeRate >= 1.0f && flag)
		{
			avoidTime = 0.0f;
			avoidTimeRate = 0.0f;
			rBackFlag = true;
			flag = false;
		}
	}
	else
	{
		manageRopePos.z -= 0.2f;
		manageRopeScale.z -= 0.2f;

		avoidTime += 0.2f;
		avoidTimeRate = min(avoidTime / avoidEndTime, 1);

		rPos = Easing::easeOut(rPos, manageRopePos, avoidTimeRate);
		rScale = Easing::easeOut(rScale, manageRopeScale, avoidTimeRate);

		if (avoidTimeRate >= 1.0f && !flag)
		{
			avoidTime = 0.0f;
			avoidTimeRate = 0.0f;
			rBackFlag = false;
			flag = false;
		}
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
	camera->SetTarget(pPos);
	cPos = camera->GetEye();

	cameraLength = { cPos.x - pPos.x, cPos.y - pPos.y, cPos.z - pPos.z, 1.0f };
	cameraLength = XMVector3Normalize(cameraLength);
	camera->Update();
}

void GameScene::EaseUpdate(const XMFLOAT3 startPos, const XMFLOAT3 endPos, XMFLOAT3& reflectPos, bool& flag)
{
	// フラグがtrueじゃない場合はリターンする
	if (!flag) { return; }

	avoidTime += 0.5f;
	avoidTimeRate = min(avoidTime / avoidEndTime, 1);
	reflectPos = Easing::easeIn(startPos, endPos, avoidTimeRate);

	if (avoidTimeRate >= 1.0f)
	{
		avoidTime = 0.0f;
		avoidTimeRate = 0.0f;
		flag = false;

		rFlag = false;
		cCount = 60;
	}
}

void GameScene::CollisionUpdate()
{
	if (cCount)
	{
		rFlag = false;
		cCount--;
		return;
	}

	if (Collision::CollisionObject(enemy, rope))
	{
		rFlag = true;
		avoidTimeRate = 2.0f;
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

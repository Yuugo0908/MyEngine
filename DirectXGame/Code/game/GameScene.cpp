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
	rope->SetScale({ 0.0f, 0.0f, 0.0f });

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

	CameraUpdate();
	PlayerUpdate();
	EnemyUpdate();
	LightUpdate();
	RopeUpdate();

	if (collisionCount > 0)
	{
		collisionCount -= 1;
	}
	if (Collision::CollisionObject(player, enemy) && collisionCount == 0)
	{
		rFlag = true;
	}

	player->SetPosition(pPos);
	enemy->SetPosition(ePos);

	skydome->Update();
	stage->Update();
	mouse->Update();
}

void GameScene::reset()
{
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
	rope->SetScale({ 0.0f, 0.0f, 0.0f });
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
	if (rFlag)
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
	pPos = player->GetPosition();
	// ジャンプ
	if (keyboard->TriggerKey(DIK_SPACE) && pFlag == false)
	{
		pFlag = true;
		// 上昇率の更新
		pVal = 1.25f;
	}
	if (pFlag == true) {
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
		float rate = 1.0f;
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
		if (keyboard->TriggerKey(DIK_UP))
		{
			pEaseFlag = true;
			startPos = pPos;
			endPos = ePos;
		}
		else if (keyboard->TriggerKey(DIK_DOWN))
		{
			eEaseFlag = true;
			startPos = ePos;
			endPos = pPos;
		}

		EaseUpdate(startPos, endPos, pPos, pEaseFlag);
		EaseUpdate(startPos, endPos, ePos, eEaseFlag);

		return;
	}

	// 自機の突進
	if (mouse->TriggerMouseRight() && easeFlag == false)
	{
		easeFlag = true;
		startPos = pPos;
		endPos = pPos;
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
	enemy->Update();
}

void GameScene::RopeUpdate()
{
	if (rFlag == false)
	{
		rope->SetPosition(pPos);
		rope->SetScale({ 0.0f, 0.0f, 0.0f });
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

	rope->SetPosition({ (pPos.x + ePos.x) / 2, (pPos.y + ePos.y) / 2, (pPos.z + ePos.z) / 2});
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
	camera->SetEye({ pPos.x, pPos.y + 10.0f, pPos.z - 10.0f });
	camera->SetTarget(pPos);
	cPos = camera->GetEye();
	camera->Update();
}

void GameScene::EaseUpdate(const XMFLOAT3 startPos, const XMFLOAT3 endPos, XMFLOAT3& reflectPos, bool& flag)
{
	// フラグがtrueじゃない場合はリターンする
	if (!flag){ return; }

	if (flag == true)
	{
		avoidTime += 0.5f;
		avoidTimeRate = min(avoidTime / avoidEndTime, 1);

		reflectPos = Easing::easeIn(startPos, endPos, avoidTimeRate);
	}
	if (avoidTimeRate >= 1.0f)
	{
		avoidTime = 0.0f;
		avoidTimeRate = 0.0f;
		flag = false;
		// ロープを解除、当たり判定の無効時間をカウント
		rFlag = false;
		collisionCount = 60;
	}
}

void GameScene::SetImgui()
{
	ImGui::Begin("Test");
	ImGui::SetWindowSize(ImVec2(300, 200));
	ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);

	ImGui::Text("cameraPosX: %6.2f", cPos.x);
	ImGui::Text("cameraPosY: %6.2f", cPos.y);
	ImGui::Text("cameraPosZ: %6.2f", cPos.z);
	ImGui::End();
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

XMFLOAT3 GameScene::normalize(XMFLOAT3 p1, XMFLOAT3 p2)
{
	float length = GetLength(p1, p2);

	XMFLOAT3 identity = {};
	identity.x = p2.x - p1.x / length;
	identity.y = p2.y - p1.y / length;
	identity.z = p2.z - p1.z / length;

	return identity;
}

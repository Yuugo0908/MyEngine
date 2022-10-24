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

	// .objの名前を指定してモデルを読み込む
	playerModel		= playerModel->CreateFromObject("cube");
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
	player->SetPosition({ 0.0f, 5.0f, -5.0f });
	player->SetScale({ 0.8f,0.8f,0.8f });
	enemy->SetPosition({ 0.0f, 5.0f, 5.0f });
	enemy->SetScale({ 1.0f, 1.0f, 1.0f });

	stage->SetPosition({ 0.0f, -1.0f, 0.0f });
	stage->SetScale({ 0.5f, 0.5f, 1.0f });
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
	// レイの初期値を設定
	ray.start = XMVectorSet(pPos.x, pPos.y, pPos.z, 1);
	ray.dir = XMVectorSet(0, -1, 0, 0);
	// 平面の初期値を設定
	plane.normal = XMVectorSet(0, 1, 0, 0);
	plane.distance = 0.0f; // 原点からの距離
}

void GameScene::Finalize()
{
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
		RopeUpdate();
		PlayerUpdate();
		EnemyUpdate();

		player->Update();
		enemy->Update();
		rope->Update();
		camera->Update();
		skydome->Update();
		stage->Update();
		mouse->Update();

		if (enemyCount == 5)
		{
			nowScene = 2;
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

	// ロープ
	rPos = {};
	rScale = {};
	angleX = 0.0f; // X軸
	angleY = 0.0f; // Y軸
	vecXZ = 0.0f; // XZ平面上のベクトル
	rFlag = false; // 接触フラグ

	rThrowFlag = false; // ロープを飛ばす
	rBackFlag = false; // ロープを戻す
	manageRopePos = {}; // ロープ位置管理用
	manageRopeScale = {}; // ロープスケール管理用

	// カメラ
	cPos = {};
	cTarget = {};
	cameraLength = {};

	// 突進用
	startPos = {}; // 開始位置
	endPos = {}; // 終点位置
	reflectPos = {};
	avoidMove = 5.0f; // 距離
	avoidTime = 0.0f; // 経過時間
	avoidTimeRate = 0.0f;
	easeFlag = false; // イージング開始フラグ
	pEaseFlag = false;
	eEaseFlag = false;

	// 当たり判定
	cCount = 0;
	cFlag = false;

	// シーン管理用
	nowScene = 0;

	// シェイク用
	shakeFlag = false;
	shakeXY = {};
	shakeTime = 0;
}

void GameScene::Draw() {	
	
	// SetImgui();
	
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
	ImGui::Text("cameraPosX : %6.2f", cPos.x);
	ImGui::Text("cameraPosY : %6.2f", cPos.y);
	ImGui::Text("cameraPosZ : %6.2f", cPos.z);

	ImGui::Separator();
	ImGui::Text("cameraLength : %6.2f", cameraLength.m128_f32[0]);
	ImGui::Text("cameraLength : %6.2f", cameraLength.m128_f32[1]);
	ImGui::Text("cameraLength : %6.2f", cameraLength.m128_f32[2]);
	ImGui::Separator();
	ImGui::Text("cCount : %d", cCount);

	ImGui::Separator();
	ImGui::Text("enemyCount : %d", enemyCount);

	ImGui::End();
}

void GameScene::PlayerUpdate()
{
	pPos = player->GetPosition();
	rPos = pPos + manageRopePos;
	// ジャンプ
	if (keyboard->TriggerKey(DIK_SPACE) && !jumpFlag)
	{
		jumpFlag = true;
		// 上昇率の更新
		pVal = 1.25f;
	}
	if (jumpFlag) {

		if (!easeFlag)
		{
			pVal -= pGra;
			pPos.y += pVal;
		}
	}


	if (!rFlag)
	{
		PlayerRush();
	}

	player->SetPosition(pPos);

	// 移動
	if (rThrowFlag)
	{
		return;
	}
	if (rBackFlag)
	{
		return;
	}


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

	player->SetPosition(pPos);
}

void GameScene::PlayerRush()
{
	// 自機の突進
	pMove = avoidMove * rate;
	if (keyboard->TriggerKey(DIK_K) && cCount == 0)
	{
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
	if (!eAlive)
	{
		eAliveCount++;
		enemy->SetPosition({ 0.0f, 100.0f, 0.0f });

		if (eAliveCount == 60)
		{
			enemy->SetPosition({ 0.0f, 5.0f, 5.0f });
			eAlive = true;
			eAliveCount = 0;
		}

		ePos = enemy->GetPosition();
		enemy->Update();
		return;
	}

	if (ePos.y > 0.0f)
	{
		if (!easeFlag)
		{
			eVal -= eGra;
			ePos.y += eVal;
		}
	}

	else if (ePos.y <= 0.0f)
	{
		ePos.y = 0.0f;
		eVal = 0.0f;
	}
	enemy->SetPosition(ePos);
}

void GameScene::RopeUpdate()
{
	rPos = player->GetPosition() + manageRopePos;
	rScale = manageRopeScale;
	rope->SetPosition(rPos);
	rope->SetScale(rScale);
	rope->Update();
	if (!rFlag)
	{
		if (keyboard->TriggerKey(DIK_J) && !rThrowFlag && !rBackFlag && cCount == 0)
		{
			rThrowFlag = true;
			avoidTime = 0.0f;
			avoidTimeRate = 0.0f;
		}

		RopeThrow(rPos, rScale, rThrowFlag);

		return;
	}

	if (rFlag)
	{
		// ロープがついている場合、敵を引き寄せる
		if (keyboard->TriggerKey(DIK_K) && keyboard->PushKey(DIK_W))
		{
			easeFlag = true;
			pEaseFlag = true;
			avoidTime = 0.0f;
			avoidTimeRate = 0.0f;
		}
		else if (keyboard->TriggerKey(DIK_K) && keyboard->PushKey(DIK_S))
		{
			easeFlag = true;
			eEaseFlag = true;
			avoidTime = 0.0f;
			avoidTimeRate = 0.0f;
		}

		if (pEaseFlag)
		{
			EaseUpdate(pPos, ePos, pPos, easeFlag);
		}
		if (eEaseFlag)
		{
			EaseUpdate(ePos, pPos, ePos, easeFlag);
		}
		player->SetPosition(pPos);
		enemy->SetPosition(ePos);
		player->Update();
		enemy->Update();
	}


	{
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
		rope->SetRotation({ XMConvertToDegrees(angleX), XMConvertToDegrees(angleY), 0 });
	}
}

void GameScene::RopeThrow(XMFLOAT3& rPos, XMFLOAT3& rScale, bool& flag)
{
	// フラグがtrueじゃない場合は初期化してリターンする
	if (!flag && !rBackFlag)
	{
		manageRopePos = {};
		manageRopeScale = {};
		return;
	}

	// Y軸周りの角度
	angleY = (float)atan2(pPos.x - ePos.x, pPos.z - ePos.z);
	vecXZ = sqrtf((pPos.x - ePos.x) * (pPos.x - ePos.x) + (pPos.z - ePos.z) * (pPos.z - ePos.z));
	// X軸周りの角度
	angleX = (float)atan2(ePos.y - pPos.y, vecXZ);
	rope->SetRotation({ XMConvertToDegrees(angleX), XMConvertToDegrees(angleY), 0 });

	XMVECTOR playerPos = { player->GetPosition().x, player->GetPosition().y, player->GetPosition().z, 1};
	XMVECTOR enemyPos = { enemy->GetPosition().x, enemy->GetPosition().y, enemy->GetPosition().z, 1};

	XMVECTOR subPlayerEnemy = XMVectorSubtract(enemyPos, playerPos);
	XMVECTOR NsubPlayerEnemy = XMVector3Normalize(subPlayerEnemy);

	XMFLOAT3 subPE = { NsubPlayerEnemy.m128_f32[0], NsubPlayerEnemy.m128_f32[1], NsubPlayerEnemy.m128_f32[2]};

	if (flag)
	{
		manageRopePos.x += subPE.x;
		manageRopePos.y += subPE.y;
		manageRopePos.z += subPE.z;

		manageRopeScale.x += 0.02f;
		manageRopeScale.y += 0.02f;
		manageRopeScale.z += 0.5f;

		avoidTime += 0.5f;
		avoidTimeRate = min(avoidTime / avoidEndTime, 1);

		cFlag = true;

		rPos = Easing::easeOut(rPos, manageRopePos, avoidTimeRate);
		rScale = Easing::easeOut(rScale, manageRopeScale, avoidTimeRate);

		if (Collision::CollisionObject(enemy, rope))
		{
			manageRopePos = {};
			manageRopeScale = {};
			avoidTime = 0.0f;
			avoidTimeRate = 0.0f;
			flag = false;
			rBackFlag = false;
			rFlag = true;
		}

		if (avoidTimeRate >= 1.0f)
		{
			avoidTime = 0.0f;
			avoidTimeRate = 0.0f;
			flag = false;
			rBackFlag = true;
		}
	}

	if(rBackFlag)
	{
		manageRopePos.x -= subPE.x / 2;
		manageRopePos.y -= subPE.y / 2;
		manageRopePos.z -= subPE.z / 2;

		manageRopeScale.x -= 0.01f;
		manageRopeScale.y -= 0.01f;
		manageRopeScale.z -= 0.25f;

		avoidTime += 0.25f;
		avoidTimeRate = min(avoidTime / avoidEndTime, 1);

		rPos = Easing::easeOut(rPos, manageRopePos, avoidTimeRate);
		rScale = Easing::easeOut(rScale, manageRopeScale, avoidTimeRate);

		if (Collision::CollisionObject(enemy, rope))
		{
			manageRopePos = {};
			manageRopeScale = {};
			avoidTime = 0.0f;
			avoidTimeRate = 0.0f;
			flag = false;
			rBackFlag = false;
			rFlag = true;
		}

		if (avoidTimeRate >= 1.0f)
		{
			avoidTime = 0.0f;
			avoidTimeRate = 0.0f;
			rBackFlag = false;
			cFlag = false;
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
	cTarget = camera->GetTarget();

	cameraLength = { cPos.x - pPos.x, cPos.y - pPos.y, cPos.z - pPos.z, 1.0f };
	cameraLength = XMVector3Normalize(cameraLength);

	//カメラ更新
	if (shakeFlag == true)
	{
		int power = 5;
		shakeXY.x = static_cast<float>(rand() % power) / 10;
		shakeXY.y = static_cast<float>(rand() % power) / 10;
		shakeTime++;
		if (shakeTime > 20)
		{
			shakeXY.x = 0.0f;
			shakeXY.y = 0.0f;
			shakeTime = 0;
			shakeFlag = false;
		}

		XMFLOAT3 cameraPos = cPos;
		XMFLOAT3 cameraTarget = cTarget;
		cameraPos.x += shakeXY.x;
		cameraPos.y += shakeXY.y;
		cameraTarget.x += shakeXY.x;
		cameraTarget.y += shakeXY.y;
		camera->SetTarget(cameraTarget);
		camera->SetEye(cameraPos);
		camera->Update();
	}
}

void GameScene::EaseUpdate(const XMFLOAT3 startPos, const XMFLOAT3 endPos, XMFLOAT3& reflectPos, bool& flag)
{
	// フラグがtrueじゃない場合はリターンする
	if (!flag) { return; }

	avoidTime += 0.5f;
	avoidTimeRate = min(avoidTime / avoidEndTime, 1);
	reflectPos = Easing::easeIn(startPos, endPos, avoidTimeRate);

	if (Collision::CollisionObject(player, enemy))
	{
		rFlag = false;
		pEaseFlag = false;
		eEaseFlag = false;
		avoidTime = 0.0f;
		avoidTimeRate = 0.0f;
		flag = false;
		return;
	}

	if (avoidTimeRate >= 1.0f)
	{
		rFlag = false;
		pEaseFlag = false;
		eEaseFlag = false;
		avoidTime = 0.0f;
		avoidTimeRate = 0.0f;
		flag = false;
	}
}

void GameScene::CollisionUpdate()
{
	CollisionRay();

	if (easeFlag || rThrowFlag || rBackFlag)
	{
		cCount = 30;
	}
	else
	{
		if (cCount > 0)
		{
			cCount--;
		}
	}

	if (rFlag && Collision::CollisionObject(player, enemy))
	{
		shakeFlag = true;
		eAlive = false;
		enemyCount = enemyCount + 1;
	}
}

void GameScene::CollisionRay()
{
	pPos = player->GetPosition();

	ray.start = XMVectorSet(pPos.x, pPos.y + 2.0f, pPos.z, 1);
	ray.dir = XMVectorSet(pPos.x, pPos.y - 2.0f, pPos.z, 1);

	// レイと平面の当たり判定
	XMVECTOR inter;
	float distance;
	bool hit = Collision::CollisionRayPlane(ray, plane, &distance, &inter);

	if (pPos.y <= 0.0f)
	{
		pPos.y = 0.0f;
		rPos.y = 0.0f;
		pVal = 0.0f;
		jumpFlag = false;
	}
	else
	{
		jumpFlag = true;
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

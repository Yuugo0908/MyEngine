#include "GameScene.h"
#include <imgui.h>
#include <cassert>

GameScene::GameScene() {

}

GameScene::~GameScene() {

}


void GameScene::Initialize(DirectXCommon* dxCommon, Controller* controller, Mouse* mouse, Audio* audio) {
	// nullptrチェック
	assert(dxCommon);
	assert(controller);
	assert(mouse);
	assert(audio);

	this->dxCommon = dxCommon;
	this->controller = controller;
	this->mouse = mouse;
	this->audio = audio;

	easing = new Easing;
	rope = new Rope;
	player = new Player;
	enemy = new Enemy;
	bullet = new Bullet;

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

	//rope->Initialize(keyboard, mouse);
	player->Initialize(keyboard, mouse);
	bullet->Initialize();
	for (int i = 0; i < enemyCount; i++)
	{
		std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
		newEnemy->Initialize(bullet, player);

		enemys.push_back(std::move(newEnemy));
		eAlive = true;
	}

	// .objの名前を指定してモデルを読み込む
	skydomeModel = skydomeModel->CreateFromObject("skydome");
	stageModel = stageModel->CreateFromObject("stage");

	// 3Dオブジェクト生成
	skydome = Object3d::Create();
	stage = Object3d::Create();

	// 3Dオブジェクトにモデルを割り当てる
	skydome->SetModel(skydomeModel);
	stage->SetModel(stageModel);

	// 各オブジェクトの位置や大きさを初期化
	stage->SetPosition({ 0.0f, -1.0f, 0.0f });
	stage->SetScale({ 1.0f, 1.0f, 1.0f });
	skydome->SetPosition({ 0.0f, 12.0f, 0.0f });
	skydome->SetScale({ 5.0f, 5.0f, 5.0f });

	// カメラの設定
	camera->SetTarget(pPos);

	// ライトの生成
	light = Light::Create();
	// ライトの色を設定
	//light->SetLightColor({ 0.8f, 0.0f, 0.6f });
	// 3Dオブジェクトにライトをセット
	Object3d::SetLight(light);
	FbxObject3d::SetLight(light);
}

void GameScene::Finalize()
{
	player->Finalize();
	enemy->Finalize();
	//rope->Finalize();
	bullet->Finalize();
}

void GameScene::Update() {

	if (nowScene == 0)
	{
		if (keyboard->TriggerKey(DIK_SPACE))
		{
			for (std::unique_ptr<Enemy>& enemy : enemys)
			{
				oldrandPos = enemy->GetrandPos();
				enemy->Spawn(oldrandPos);
			}
			nowScene = 1;
		}
	}

	if (nowScene == 1)
	{
		//CollisionUpdate();
		LightUpdate();
		CameraUpdate();

		//rFlag = rope->GetrFlag();
		//moveFlag = rope->GetmoveFlag();
		//eAlive = enemy->GetAlive();
		attackFlag = bullet->GetAttackFlag();

		bullet->Update(pPos, ePos);
		player->Update(rFlag, moveFlag);

		if (eAlive)
		{
			for (std::unique_ptr<Enemy>& enemy : enemys)
			{
				enemy->Update(pPos, bPos);
			}
		}

		pPos = player->GetPos();
		//ePos = enemy->GetPos();
		bPos = bullet->GetPos();

		length = GetLength(pPos, ePos);

		//rope->Update(pPos, ePos, enemy->GetObj());

		player->SetPos(pPos);
		//enemy->SetPos(ePos);
		bullet->SetPos(bPos);

		skydome->Update();
		stage->Update();
		mouse->Update();

		if (enemyCount == 0)
		{
			nowScene = 2;
		}
	}
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

	if (eAlive)
	{
		for (std::unique_ptr<Enemy>& enemy : enemys)
		{
			enemy->Draw();
		}
	}

	if (attackFlag)
	{
		bullet->Draw();
	}
	skydome->Draw();
	stage->Draw();
	//rope->Draw();

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
	ImGui::Text("length : %6.2f", length);

	ImGui::End();
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

	cameraLength = { cPos.x - pPos.x, cPos.y - pPos.y, cPos.z - pPos.z, 1.0f };
	cameraLength = XMVector3Normalize(cameraLength);

	//カメラ更新
	if (shakeFlag == true)
	{
		camera->CameraShake(shakeFlag);
	}

	cPos = camera->GetEye();
	cTarget = camera->GetTarget();
	camera->Update();
}

void GameScene::CollisionUpdate()
{
	if (Collision::CollisionObject(player->GetObj(), enemy->GetObj()))
	{
		shakeFlag = true;
		eAlive = false;
		rFlag = false;
		moveFlag = true;
		attackFlag = false;
		bullet->SetAttackFlag(attackFlag);
		//rope->SetrFlag(rFlag);
		//rope->SetmoveFlag(moveFlag);
		enemy->SetAlive(eAlive);
		//enemyCount++;
	}
}

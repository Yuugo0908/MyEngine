#include "GameScene.h"
#include <imgui.h>
#include <cassert>

GameScene::GameScene() {

}

GameScene::~GameScene() {

}

int GameScene::height = 0;
int GameScene::width = 0;


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
	mapchip = new Mapchip;

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

	if (!Image2d::LoadTexture(3, L"Resources/HPText.png")) {
		assert(0);
	}
	HPText = Image2d::Create(3, { 0.0f,0.0f });
	HPText->SetSize({ 1280.0f,720.0f });

	if (!Image2d::LoadTexture(4, L"Resources/HPBar.png")) {
		assert(0);
	}
	PlayerHPBar = Image2d::Create(4, { 0.0f,45.0f });
	PlayerHPBar->SetSize({ 360.0f,60.0f });
	EnemyHPBar = Image2d::Create(4, { 0.0f,160.0f });
	EnemyHPBar->SetSize({ 360.0f,60.0f });

	if (!Image2d::LoadTexture(5, L"Resources/PlayerHPGauge.png")) {
		assert(0);
	}
	PlayerHPGauge = Image2d::Create(5, { 0.0f,45.0f });
	PlayerHPGauge->SetSize({ 30.0f,60.0f });


	if (!Image2d::LoadTexture(6, L"Resources/EnemyHPGauge.png")) {
		assert(0);
	}
	EnemyHPGauge = Image2d::Create(6, { 0.0f,160.0f });
	EnemyHPGauge->SetSize({ 30.0f,60.0f });

	if (!Image2d::LoadTexture(7, L"Resources/GameOver.png")) {
		assert(0);
	}
	GameOver = Image2d::Create(7, { 0.0f,0.0f });
	GameOver->SetSize({ 1280.0f,720.0f });

	if (!Image2d::LoadTexture(8, L"Resources/black_backGround.png"))
	{
		assert(0);
	}
	backGround = Image2d::Create(8, { 0.0f,0.0f });
	backGround->SetSize({ 1280.0f,720.0f });
	backGround->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f });

	if (!Image2d::LoadTexture(9, L"Resources/Operation_Explanation.png"))
	{
		assert(0);
	}
	explanation = Image2d::Create(9, { 0.0f,0.0f });
	explanation->SetSize({ 1280.0f,720.0f });
	explanation->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f });

	// レベルデータの読み込み
	levelData = LevelLoader::LoadFile("gameScene2");

	// レベルデータからオブジェクトを生成、配置
	for (LevelData::ObjectData& objectData : levelData->objects)
	{
		// 3Dオブジェクトを生成
		std::unique_ptr<Object3d> newObject = Object3d::Create();

		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(levelData->models)::iterator it = levelData->models.find(objectData.fileName);
		if (it != levelData->models.end()) {
			model = it->second;
		}

		newObject->SetModel(model);

		// 座標
		DirectX::XMFLOAT3 pos;
		DirectX::XMStoreFloat3(&pos, objectData.trans);
		newObject->SetPosition(pos);

		// 回転角
		DirectX::XMFLOAT3 rot;
		DirectX::XMStoreFloat3(&rot, objectData.rot);
		newObject->SetRotation(rot);

		// 座標
		DirectX::XMFLOAT3 scale;
		DirectX::XMStoreFloat3(&scale, objectData.scale);
		newObject->SetScale(scale);

		if (objectData.fileName == "stage")
		{
			newObject->SetType(stage_);
		}

		if (objectData.fileName == "cube")
		{
			newObject->SetType(box_);
		}

		// 配列に登録
		jsonObject.push_back(std::move(newObject));
	}

	Enemy::StaticInit();

	rope->Initialize(keyboard, mouse);
	player->Initialize(keyboard, mouse);
	pPos = player->GetObj()->GetPosition();

	for (int i = 0; i < enemyCount; i++)
	{
		std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
		newEnemy->Initialize(player);
		enemys.push_back(std::move(newEnemy));
	}

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
	rope->Finalize();
}

void GameScene::Update() {

	if (nowScene == title_)
	{
		if (!fadeIn && keyboard->TriggerKey(DIK_SPACE))
		{
			//マップチップ用のCSV読み込み
			//(map, "Resource/scv/なんたら.csv")で追加可能
			//if (!mapLoadFlag)
			//{
			//	//Mapchip::CsvToVector(map, "Resources/csv/tutorial.csv");
			//	Mapchip::CsvToVector(map, "Resources/csv/stage1.csv");
			//	MapCreate(0);
			//	mapLoadFlag = true;
			//}

			if (!expFlag)
			{
				expFlag = true;
				explanation->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f });
			}

			else if (!fadeIn)
			{
				fadeIn = true;
			}
		}

		if(fadeIn)
		{
			alpha += 0.02f;
			backGround->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, alpha });
			if (alpha >= 1.0f)
			{
				nowScene = game_;
				fadeIn = false;
				fadeOut = true; 
				explanation->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f });
			}
		}
	}

	// タイトルから移行後の更新
	if (nowScene == game_)
	{
		//	プレイヤーの座標、半径の設定
		rope->Update(pPos);
		player->Update(rFlag, moveFlag);
		pPos = player->GetObj()->GetPosition();
		pScale = player->GetObj()->GetScale();
		if (attackFlag)
		{
			player->Attack(catchPos, attackFlag, avoidTime);
		}

		// jsonファイルから読み込んだオブジェクトの更新
		for (auto& object : jsonObject)
		{
			object->Update();

			if (object->GetType() == box_)
			{
				XMFLOAT3 boxPos = object->GetPosition();
				XMFLOAT3 boxScale = object->GetScale();
				player->MapCollide(boxPos, boxScale);

				for (std::unique_ptr<Enemy>& enemy : enemys)
				{
					enemy->MapCollide(boxPos, boxScale);
				}
			}
			else if (object->GetType() == stage_)
			{
				XMFLOAT3 stagePos = object->GetPosition();
				XMFLOAT3 stageScale = object->GetScale();

				if (player->StageCollide(stagePos, stageScale))
				{
					player->SetOnGround(true);
				}

				for (std::unique_ptr<Enemy>& enemy : enemys)
				{
					eAlive = enemy->GetAlive();

					if (eAlive && enemy->StageCollide(stagePos, stageScale))
					{
						enemy->SetOnGround(true);
					}
				}
			}
		}

		CollisionUpdate();
		LightUpdate();
		CameraUpdate();

		// フェードアウトが終わるまではここでリターン
		if (fadeOut)
		{
			alpha -= 0.02f;
			backGround->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, alpha });			
			if (alpha <= 0.0f)
			{
				fadeOut = false;
			}
		}
		

		if (!fadeIn && !fadeOut)
		{
			// フラグの取得
			rFlag = rope->GetrFlag();
			moveFlag = rope->GetmoveFlag();
			avoidFlag = player->GetAvoidFlag();
			onGround = player->GetOnGround();

			if (throwCount < 30)
			{
				throwCount++;
			}

			for (std::unique_ptr<Enemy>& enemy : enemys)
			{
				pPos = player->GetObj()->GetPosition();
				ePos = enemy->GetObj()->GetPosition();
				eScale = enemy->GetObj()->GetScale();
				PElength = GetLength(pPos, ePos);

				if (PElength < minLength)
				{
					minLength = PElength;
					ePosSave = ePos;
				}

				if (RayCollision())
				{
					enemy->SetAttackFlag(false);
				}
				else
				{
					enemy->SetAttackFlag(true);
				}
				enemy->Update();
			}


			if (throwCount == 30)
			{
				rope->Throw(pPos, ePosSave, minLength);
				ePosSave = {};
				minLength = 15.0f;
			}

			player->GetObj()->SetPosition(pPos);
			mouse->Update();
		}

		if (enemyHp <= 0)
		{
			fadeIn = true;
			if (fadeIn)
			{
				alpha += 0.02f;
				backGround->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, alpha });
				if (alpha >= 1.0)
				{
					nowScene = clear_;
					fadeIn = false;
					fadeOut = true;
				}
			}
		}

		if (playerHp <= 0)
		{
			fadeIn = true;
			if (fadeIn)
			{
				alpha += 0.02f;
				backGround->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, alpha });
				if (alpha >= 1.0f)
				{
					nowScene = failure_;
					fadeIn = false;
					fadeOut = true;
				}
			}
		}
	}

	if (nowScene == clear_ || nowScene == failure_)
	{
		if (fadeOut)
		{
			alpha -= 0.02f;
			backGround->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, alpha });
			if (alpha <= 0.0f)
			{
				fadeOut = false;
			}
		}

		if (alpha <= 0.0f && keyboard->TriggerKey(DIK_SPACE))
		{
			player->Reset();
			for (std::unique_ptr<Enemy>& enemy : enemys)
			{
				enemy->Reset();
			}
			rope->Reset();
			camera->Reset();
			reset();
			nowScene = title_;
		}
	}
}

void GameScene::reset()
{
	// プレイヤー
	pPos = { 0.0f, 10.0f, 0.0f };//座標
	pPosOld = { 0.0f, 10.0f, 0.0f };
	pScale = {};
	onGround = false;//自由落下のフラグ
	moveFlag = false;//移動管理フラグ
	avoidFlag = false;//回避管理フラグ
	playerHp = 360;

	// エネミー
	enemyHp = 360;
	ePos = {};
	ePosOld = {};
	eScale = {};

	// ロープ
	rFlag = false;

	// カメラ
	cPos = {};
	cTarget = {};
	cameraLength = {};

	// シェイク用
	shakeFlag = false;
}

void GameScene::Draw() {

	SetImgui();

#pragma region 背景画像描画
	// 背景画像描画前処理
	Image2d::PreDraw(dxCommon->GetCommandList());


	// 画像描画後処理
	Image2d::PostDraw();
	// 深度バッファクリア
	dxCommon->ClearDepthBuffer();
#pragma endregion 背景画像描画

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(dxCommon->GetCommandList());

	// 3Dオブクジェクトの描画
	if (nowScene == game_)
	{
		player->GetObj()->Draw();

		for (std::unique_ptr<Enemy>& enemy : enemys)
		{
			enemy->Draw();
		}
		rope->Draw();
		//MapDraw(0);

		for (auto& object : jsonObject)
		{
			object->Draw();
		}
	}

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();
#pragma endregion 3Dオブジェクト描画

#pragma region 前景画像描画
	// 前景画像描画前処理
	Image2d::PreDraw(dxCommon->GetCommandList());

	// 前景画像の描画

	if (nowScene == title_)
	{
		title->Draw();
	}

	if (nowScene == game_)
	{
		HPText->Draw();
		PlayerHPBar->Draw();
		EnemyHPBar->Draw();

		PlayerHPGauge->SetSize({ playerHp,60 });
		PlayerHPGauge->Draw();

		EnemyHPGauge->SetSize({ enemyHp,60 });
		EnemyHPGauge->Draw();
	}

	if (nowScene == clear_)
	{
		result->Draw();
	}

	if (nowScene == failure_)
	{
		GameOver->Draw();
	}

	explanation->Draw();
	backGround->Draw();

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
	ImGui::Text("cameraLength : %6.2f", ePos.x);
	ImGui::Text("cameraLength : %6.2f", ePos.y);
	ImGui::Text("cameraLength : %6.2f", ePos.z);

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
	//DebugText::GetInstance()->Print(30, 30 * 15, 2, "%f", cPos.x);
	//DebugText::GetInstance()->Print(30, 30 * 16, 2, "%f", cPos.y);
	//DebugText::GetInstance()->Print(30, 30 * 17, 2, "%f", cPos.z);
	camera->Update();
}

void GameScene::CollisionUpdate()
{
	//for (XMFLOAT3& pos : stagePos)
	//{
	//	for (XMFLOAT3& scale : stageScale)
	//	{
	//		if (player->StageCollide(pos, scale))
	//		{
	//			player->SetOnGround(true);
	//		}
	//		for (std::unique_ptr<Enemy>& enemy : enemys)
	//		{
	//			if (enemy->StageCollide(pos, scale))
	//			{
	//				enemy->SetOnGround(true);
	//			}
	//		}
	//	}
	//}

	//if (player->StageCollide(stagePos, stageScale))
	//{
	//	player->SetOnGround(true);
	//}

	//for (std::unique_ptr<Enemy>& enemy : enemys)
	//{
	//	eAlive = enemy->GetAlive();

	//	if (eAlive && enemy->StageCollide(stagePos, stageScale))
	//	{
	//		enemy->SetOnGround(true);
	//	}
	//}

	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		eAlive = enemy->GetAlive();
		if (eAlive)
		{
			pPos = player->GetObj()->GetPosition();
			if (rope->Collision(enemy->GetObj(), pPos))
			{
				attackFlag = true;
				catchPos = enemy->GetObj()->GetPosition();
				enemy->SetCatchFlag(true);
			}
			else
			{
				enemy->SetCatchFlag(false);
			}
		}
	}

	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		eAlive = enemy->GetAlive();
		if (eAlive && enemy->EnemyCollision(player->GetObj()))
		{
			shakeFlag = true;
			moveFlag = true;
			rope->SetmoveFlag(moveFlag);

			if (rFlag)
			{
				rope->SetrFlag(false);
				attackFlag = false;
				avoidTime = 0.0f;
				enemyHp -= 72;
				break;
			}
			else
			{
				playerHp -= 18;
			}
		}
	}

	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		eAlive = enemy->GetAlive();
		if (eAlive && enemy->BulletCollision())
		{
			if (!rFlag)
			{
				shakeFlag = true;
				playerHp -= 18;
			}
		}
	}

	camera->SetTarget(pPos);
}

bool GameScene::RayCollision()
{
	for (auto& object : jsonObject)
	{
		if (object->GetType() == box_)
		{
			XMFLOAT3 boxPos =object->GetPosition();
			XMFLOAT3 boxScale = object->GetScale();
			// エネミーからプレイヤーへのレイ
			XMFLOAT3 layStart = ePos;

			XMVECTOR playerPos = { pPos.x, pPos.y, pPos.z, 1 };
			XMVECTOR enemyPos = { ePos.x, ePos.y, ePos.z, 1 };
			XMVECTOR subPlayerEnemy = XMVectorSubtract(playerPos, enemyPos);
			XMVECTOR NsubPlayerEnemy = XMVector3Normalize(subPlayerEnemy);
			XMFLOAT3 subPE = { NsubPlayerEnemy.m128_f32[0], NsubPlayerEnemy.m128_f32[1], NsubPlayerEnemy.m128_f32[2] };

			XMFLOAT3 layVec = subPE;

			if(GetLength(pPos, ePos) >= GetLength(boxPos, ePos))
			{
				// マップオブジェクトの座標と大きさ
				XMFLOAT3 bCenter = boxPos;
				XMFLOAT3 bRadius = { boxScale.x * 2.0f, boxScale.y, boxScale.z * 2.0f };

				bCenter = bCenter - layStart;

				float bA1 = layVec.x * layVec.x + layVec.y * layVec.y + layVec.z * layVec.z;
				if (bA1 == 0.0f)
				{
					continue;
				}

				float bB1 = layVec.x * bCenter.x + layVec.y * bCenter.y + layVec.z * bCenter.z;
				
				float bC1 = bCenter.x * bCenter.x + bCenter.y * bCenter.y + bCenter.z * bCenter.z - bRadius.x * bRadius.x;
				float bC2 = bCenter.x * bCenter.x + bCenter.y * bCenter.y + bCenter.z * bCenter.z - bRadius.y * bRadius.y;
				float bC3 = bCenter.x * bCenter.x + bCenter.y * bCenter.y + bCenter.z * bCenter.z - bRadius.z * bRadius.z;

				float s1 = bB1 * bB1 - bA1 * bC1;
				float s2 = bB1 * bB1 - bA1 * bC2;
				float s3 = bB1 * bB1 - bA1 * bC3;
				if (s1 < 0.0f || s2 < 0.0f || s3 < 0.0f)
				{
					continue;
				}

				s1 = sqrtf(s1);
				s2 = sqrtf(s2);
				s3 = sqrtf(s3);
				float a1 = (bB1 - s1) / bA1;
				float a2 = (bB1 + s1) / bA1;
				float a3 = (bB1 - s2) / bA1;
				float a4 = (bB1 + s2) / bA1;
				float a5 = (bB1 - s3) / bA1;
				float a6 = (bB1 + s3) / bA1;

				if (a1 < 0.0f || a2 < 0.0f || a3 < 0.0f || a4 < 0.0f || a5 < 0.0f || a6 < 0.0f)
				{
					continue;
				}

				return true;
			}
			else
			{
				continue;
			}
		}
	}
	return false;
}

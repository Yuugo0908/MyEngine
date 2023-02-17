#include "GameScene.h"
#include <imgui.h>
#include <cassert>

GameScene::GameScene() {

}

GameScene::~GameScene() {

}

void GameScene::Initialize(DirectXCommon* dxCommon, Audio* audio) {
	// nullptrチェック
	assert(dxCommon);
	assert(audio);

	this->dxCommon = dxCommon;
	this->audio = audio;

	easing = new Easing;
	rope = new Rope;
	player = new Player;
	enemy = new Enemy;

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
	if (!Image2d::LoadTexture(titleNum, L"Resources/title.png"))
	{
		assert(0);
	}
	title = Image2d::Create(titleNum, { 0.0f,0.0f });
	title->SetSize({ 1280.0f,720.0f });

	// リザルト画像読み込み
	if (!Image2d::LoadTexture(resultNum, L"Resources/result.png"))
	{
		assert(0);
	}
	result = Image2d::Create(resultNum, { 0.0f,0.0f });
	result->SetSize({ 1280.0f,720.0f });

	if (!Image2d::LoadTexture(HPTextNum, L"Resources/HPText.png"))
	{
		assert(0);
	}
	HPText = Image2d::Create(HPTextNum, { 0.0f,0.0f });
	HPText->SetSize({ 1280.0f,720.0f });

	if (!Image2d::LoadTexture(HPBarNum, L"Resources/HPBar.png"))
	{
		assert(0);
	}
	PlayerHPBar = Image2d::Create(HPBarNum, { 0.0f,45.0f });
	PlayerHPBar->SetSize({ 360.0f,60.0f });

	if (!Image2d::LoadTexture(HPGaugeNum, L"Resources/PlayerHPGauge.png"))
	{
		assert(0);
	}
	PlayerHPGauge = Image2d::Create(HPGaugeNum, { 0.0f,45.0f });
	PlayerHPGauge->SetSize({ 30.0f,60.0f });

	if (!Image2d::LoadTexture(GameOverNum, L"Resources/GameOver.png"))
	{
		assert(0);
	}
	GameOver = Image2d::Create(GameOverNum, { 0.0f,0.0f });
	GameOver->SetSize({ 1280.0f,720.0f });

	if (!Image2d::LoadTexture(backNum, L"Resources/black_backGround.png"))
	{
		assert(0);
	}
	backGround = Image2d::Create(backNum, { 0.0f,0.0f });
	backGround->SetSize({ 1280.0f,720.0f });
	backGround->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f });

	if (!Image2d::LoadTexture(expNum, L"Resources/Operation_Explanation.png"))
	{
		assert(0);
	}
	explanation = Image2d::Create(expNum, { 0.0f,0.0f });
	explanation->SetSize({ 1280.0f,720.0f });
	explanation->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f });


	// パーティクルマネージャ生成
	particle = Particle::Create(dxCommon->GetDevice(), camera);

	// レベルデータの読み込み
	levelData = LevelLoader::LoadFile("gameScene");

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

		if (objectData.fileName == "wall")
		{
			newObject->SetType(wall_);
		}

		// 配列に登録
		jsonObject.push_back(std::move(newObject));
	}

	Enemy::StaticInit();

	rope->Initialize();
	player->Initialize();
	pPos = player->GetObj()->GetPosition();

	for (int i = 0; i < enemyCount; i++)
	{
		std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
		newEnemy->Initialize(player);
		enemys.push_back(std::move(newEnemy));
	}

	// カメラの設定
	camera->SetTarget(pPos);
	camera->Update();

	// ライトの生成
	light = Light::Create();
	// ライトの色を設定
	//light->SetLightColor({ 0.8f, 0.6f, 0.6f });
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
		if (!fadeIn && keyboard->TriggerKey(DIK_SPACE) || controller->GetPadState(controller->A, controller->TRIGGER))
		{
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
		jsonObjectUpdate();

		CollisionUpdate();
		LightUpdate();
		CameraUpdate();
		particle->Update();

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

				//if (PElength <= 15.0f)
				//{
					if (RayCollision())
					{
						enemy->SetAttackFlag(false);
					}
					else
					{
						enemy->SetAttackFlag(true);
					}
				//}

				enemy->Update();
			}


			if (throwCount == 30)
			{
				rope->Throw(pPos, ePosSave, minLength);
				ePosSave = {};
				minLength = 15.0f;
			}

			player->GetObj()->SetPosition(pPos);
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

		if (alpha <= 0.0f && keyboard->TriggerKey(DIK_SPACE) || controller->GetPadState(controller->A, controller->TRIGGER))
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

		for (auto& object : jsonObject)
		{
			object->Draw();
		}

		// パーティクルの描画
		particle->Draw(dxCommon->GetCommandList());
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

		PlayerHPGauge->SetSize({ playerHp,60 });
		PlayerHPGauge->Draw();
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
	ImGui::Text("check : %d", check);

	ImGui::End();
}

void GameScene::CreateParticles(XMFLOAT3 setPos)
{
	for (int i = 0; i < 20; i++) {
		// X,Y,Z全て[-5.0f,+5.0f]でランダムに分布
		XMFLOAT3 pos{};
		pos.x = setPos.x;
		pos.y = setPos.y;
		pos.z = setPos.z;

		const float rnd_vel = 0.1f;
		XMFLOAT3 vel{};
		vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.y = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;

		XMFLOAT3 acc{};
		const float rnd_acc = 0.001f;
		acc.y = -(float)rand() / RAND_MAX * rnd_acc;

		// 追加
		particle->Add(60, pos, vel, acc, 5.0f, 0.0f);
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

	cameraLength = { cPos.x - pPos.x, cPos.y - pPos.y, cPos.z - pPos.z, 1.0f };
	cameraLength = XMVector3Normalize(cameraLength);

	//カメラ更新
	if (shakeFlag == true)
	{
		camera->CameraShake(shakeFlag);
	}

	if (moveFlag)
	{
		cPos = camera->GetEye();
		cTarget = camera->GetTarget();
		camera->Update();
	}
}

void GameScene::jsonObjectUpdate()
{
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
}

void GameScene::CollisionUpdate()
{
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
				// パーティクル生成
				CreateParticles(enemy->GetObj()->GetPosition());
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
			// ワールド空間での光線の基点
			XMFLOAT3 layStart = ePos;
			// ワールド空間での光線の方向
			XMVECTOR playerPos = { pPos.x, pPos.y, pPos.z, 1 };
			XMVECTOR enemyPos = { ePos.x, ePos.y, ePos.z, 1 };
			XMVECTOR subPlayerEnemy = XMVectorSubtract(playerPos, enemyPos);
			XMVECTOR NsubPlayerEnemy = XMVector3Normalize(subPlayerEnemy);
			XMFLOAT3 subPE = { NsubPlayerEnemy.m128_f32[0], NsubPlayerEnemy.m128_f32[1], NsubPlayerEnemy.m128_f32[2] };
			XMFLOAT3 layVec = subPE;
			//境界BOX(ローカル)
			XMFLOAT3 boxPos = object->GetPosition();
			XMFLOAT3 boxScale = object->GetScale();

			check = 1;

			float t_min = -FLT_MAX;
			float t_max = FLT_MAX;
			float max[3] = { boxPos.x + boxScale.x, boxPos.y + boxScale.y, boxPos.z + boxScale.z };
			float min[3] = { boxPos.x - boxScale.x, boxPos.y - boxScale.y, boxPos.z - boxScale.z };
			float p[3] = { layStart.x, layStart.y, layStart.z };
			float d[3] = { layVec.x, layVec.y, layVec.z };

			// マップオブジェクトの座標と大きさ
			XMFLOAT3 boxMax = boxPos + boxScale;
			XMFLOAT3 bCenter = boxPos;
			float bScale = GetLength(boxPos, boxMax);
			bCenter = bCenter - layStart;
			float bA1 = layVec.x * layVec.x + layVec.y * layVec.y + layVec.z * layVec.z;
			if (bA1 == 0.0f)// レイの長さが0
			{
				check = 0;
				continue;
			}
			float bB1 = layVec.x * bCenter.x + layVec.y * bCenter.y + layVec.z * bCenter.z;
			float bC1 = bCenter.x * bCenter.x + bCenter.y * bCenter.y + bCenter.z * bCenter.z - bScale * bScale;
			float s1 = bB1 * bB1 - bA1 * bC1;
			s1 = sqrtf(s1);
			float a1 = (bB1 - s1) / bA1;
			float a2 = (bB1 + s1) / bA1;
			if (a1 < 0.0f || a2 < 0.0f)
			{
				check = 0;
				continue;
			}

			for (int i = 0; i < 3; ++i)
			{
				if (abs(d[i]) < FLT_EPSILON)
				{
					if (p[i] < min[i] || p[i] > max[i])
					{
						check = 0; // 交差していない
						break;
					}
				}
				else
				{
					// スラブとの距離を算出
					// t1が近スラブ、t2が遠スラブとの距離
					float odd = 1.0f / d[i];
					float t1 = (min[i] - p[i]) * odd;
					float t2 = (max[i] - p[i]) * odd;

					if (t1 > t2)
					{
						float tmp = t1;
						t1 = t2;
						t2 = tmp;
					}

					if (t1 > t_min) t_min = t1;
					if (t2 < t_max) t_max = t2;

					// スラブ交差チェック
					if (t_min >= t_max)
					{
						check = 0;
						break;
					}
				}
			}


			if (check == 0)
			{
				continue;
			}

			XMFLOAT3 colPosMin = layStart + (layVec * t_min);
			XMFLOAT3 colPosMax = layStart + (layVec * t_max);

			DebugText::GetInstance()->Print(30, 30 * 16, 2, "min : %f, %f, %f", colPosMin.x, colPosMin.y, colPosMin.z);
			DebugText::GetInstance()->Print(30, 30 * 17, 2, "max : %f, %f, %f", colPosMax.x, colPosMax.y, colPosMax.z);
			return true;
		}
	}
	return false;
}
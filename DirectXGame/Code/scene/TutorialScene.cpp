#include "TutorialScene.h"
#include <imgui.h>
#include <cassert>
#include "SceneManager.h"

void TutorialScene::Initialize() {
	rope = new Rope;
	player = new Player;
	enemy = new Enemy;

	// 3Dオブジェクトにカメラをセット
	Object3d::SetCamera(camera);

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

	if (!Image2d::LoadTexture(fadeNum, L"Resources/fade.png"))
	{
		assert(0);
	}
	fadeTex = Image2d::Create(fadeNum, { 0.0f,0.0f });
	fadeTex->SetSize({ 1280.0f,720.0f });
	fadeTex->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });

	// パーティクル生成
	effectBox = Particle::Create(L"Resources/effectBox.png");
	effectCircle = Particle::Create(L"Resources/effectCircle.png");
	effectTarget = Particle::Create(L"Resources/effectTarget.png");

	enemy->ModelInit();
	rope->Initialize();

	// ライトの生成
	light = Light::Create();
	// ライトの色を設定
	light->SetLightColor({ 1.0f, 1.0f, 1.0f });
	// 3Dオブジェクトにライトをセット
	Object3d::SetLight(light);

	//audio->PlayWave("Resources/BGM/bgm.wav", 255, 0.08f);
	jsonObjectInit("tutorial");

	// マウスカーソルを非表示
	ShowCursor(false);
}

void TutorialScene::Finalize()
{
	//audio->Stop();
	
	// マウスカーソルを表示
	ShowCursor(true);
	safe_delete(player);
	safe_delete(enemy);
	safe_delete(rope);
	safe_delete(light);
	safe_delete(levelData);
	safe_delete(HPText);
	safe_delete(PlayerHPBar);
	safe_delete(PlayerHPGauge);
	safe_delete(fadeTex);
	safe_delete(effectBox);
	safe_delete(effectCircle);
	safe_delete(effectTarget);
}

void TutorialScene::Update()
{
	// マウスを非表示、移動範囲の制限
	mouse->CursorLimit();

	// タイトルから移行後の更新
	//	プレイヤーの座標、半径の設定
	player->Update(rFlag, moveFlag);
	pPos = player->GetObj()->GetPosition();
	pScale = player->GetObj()->GetScale();
	if (rushFlag)
	{
		player->Rush(catchPos, rushFlag, elapsedTime);
	}
	rope->Update(pPos);

	// jsonファイルから読み込んだオブジェクトの更新
	jsonObjectUpdate();
	CollisionUpdate();
	LightUpdate();
	CameraUpdate();
	effectBox->Update();
	effectCircle->Update();
	effectTarget->Update();

	// フラグの取得
	rFlag = rope->GetrFlag();
	moveFlag = rope->GetmoveFlag();
	avoidFlag = player->GetAvoidFlag();
	onGround = player->GetOnGround();

	if (throwCount < 60)
	{
		throwCount++;
	}

	// ポールと敵の距離を比較して短い方を代入
	float minLength = (std::min)(minEnemyLength, minPoleLength);

	// 距離に応じて代入する座標を変更(敵を優先する)

	if (minLength < 15.0f)
	{
		if (minLength == minEnemyLength)
		{
			posSave = posEnemySave;
		}
		else
		{
			posSave = posPoleSave;
		}

		effectTarget->TargetEffect(
			posSave, 3.0f, 1.0f,
			{ 1.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f, 1.0f },
			throwCount
		);
	}

	rope->Throw(pPos, posSave, minLength);

	posPoleSave = {};
	posEnemySave = {};
	minPoleLength = 15.0f;
	minEnemyLength = 15.0f;

	player->GetObj()->SetPosition(pPos);

	if (enemyHp <= 0)
	{
		fadeFlag = true;
		gameClearFlag = true;
	}
	else if (playerHp <= 0)
	{
		fadeFlag = true;
		reStartFlag = true;
	}

	if (fadeFlag == false && alpha > 0.0f)
	{
		alpha -= 0.02f;
	}
	else
	{
		alpha += 0.02f;
		if (alpha >= 1.0f)
		{
			reset();
		}
	}
	fadeTex->SetColor({ 1.0f, 1.0f, 1.0f, alpha });
}

void TutorialScene::Draw()
{
	//SetImgui();

#pragma region 背景画像描画
	// 背景画像描画前処理
	Image2d::PreDraw(DirectXCommon::GetInstance()->GetCommandList());


	// 画像描画後処理
	Image2d::PostDraw();
	// 深度バッファクリア
	DirectXCommon::GetInstance()->ClearDepthBuffer();
#pragma endregion 背景画像描画

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	// 3Dオブクジェクトの描画
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
	effectBox->Draw(DirectXCommon::GetInstance()->GetCommandList());
	effectCircle->Draw(DirectXCommon::GetInstance()->GetCommandList());
	effectTarget->Draw(DirectXCommon::GetInstance()->GetCommandList());

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();
#pragma endregion 3Dオブジェクト描画

#pragma region 前景画像描画
	// 前景画像描画前処理
	Image2d::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	// 前景画像の描画
	HPText->Draw();
	PlayerHPBar->Draw();

	PlayerHPGauge->SetSize({ playerHp,60 });
	PlayerHPGauge->Draw();

	fadeTex->Draw();

	// デバッグテキストの描画
	DebugText::GetInstance()->DrawAll(DirectXCommon::GetInstance()->GetCommandList());
	// 画像描画後処理
	Image2d::PostDraw();
#pragma endregion 前景画像描画
}

void TutorialScene::reset()
{
	player->Reset();
	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		enemy->Reset();
	}
	enemys.erase(enemys.begin(), enemys.end());
	rope->Reset();
	jsonObject.erase(jsonObject.begin(), jsonObject.end());

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
	enemyCount = 0;

	// ロープ
	rFlag = false;

	// カメラ
	cPos = {};
	cTarget = {};
	cameraLength = {};

	// シェイク用
	shakeFlag = false;

	if (gameClearFlag)
	{
		SceneManager::GetInstance()->ChangeScene("Game");
	}
	else if (reStartFlag)
	{
		SceneManager::GetInstance()->ChangeScene("Tutorial");
	}
}

void TutorialScene::SetImgui()
{
	ImGui::Begin("DebugText");
	ImGui::SetWindowSize(ImVec2(300, 200));
	ImGui::Separator();
	ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);

	ImGui::Separator();
	ImGui::Text("playerPosX : %6.2f", pPos.x);
	ImGui::Text("playerPosY : %6.2f", pPos.y);
	ImGui::Text("playerPosZ : %6.2f", pPos.z);

	ImGui::End();
}

void TutorialScene::LightUpdate()
{
	//光線方向初期値
	static XMVECTOR lightDir = { 5, -5, 5, 0 };

	light->SetLightDir(lightDir);
	light->Update();
}

void TutorialScene::CameraUpdate()
{
	if (pPos.y >= trackLimit)
	{
		camera->SetTarget(pPos);
	}
	else
	{
		playerHp = 0;
	}

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

void TutorialScene::CollisionUpdate()
{
	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		pPos = player->GetObj()->GetPosition();
		eAlive = enemy->GetAlive();

		if (eAlive)
		{
			if (rope->Collision(enemy->GetObj(), pPos))
			{
				rushFlag = true;
				catchPos = enemy->GetObj()->GetPosition();
			}

			if (rFlag && enemy->EnemyCollision(player->GetObj()))
			{
				shakeFlag = true;
				moveFlag = true;
				rope->SetmoveFlag(moveFlag);
				// パーティクル生成
				effectBox->CreateParticles(
					enemy->GetObj()->GetPosition(),
					2.0f, 5.0f,
					{ 1.0f, 0.0f, 0.0f, 1.0f },
					{ 0.0f, 1.0f, 0.0f, 1.0f },
					5
				);
				rope->SetrFlag(false);
				rushFlag = false;
				catchPos = {};
				elapsedTime = 0.0f;
				enemyHp -= (360.0f / enemyCount);
				controller->Vibration();
				enemys.remove(enemy);
				break;
			}
			else if (!rFlag && player->Damage(enemy->GetObj()))
			{
				shakeFlag = true;
				moveFlag = true;
				rope->SetmoveFlag(moveFlag);
				playerHp -= 36;

				// パーティクル生成
				effectCircle->CreateParticles(
					player->GetObj()->GetPosition(),
					1.0f, 2.0f,
					{ 0.0f, 1.0f, 0.0f, 1.0f },
					{ 1.0f, 0.0f, 0.0f, 1.0f },
					3
				);
			}

			if (!avoidFlag && enemy->BulletCollision())
			{
				if (!rFlag)
				{
					shakeFlag = true;
					playerHp -= 36;

					// パーティクル生成
					effectCircle->CreateParticles(
						player->GetObj()->GetPosition(),
						1.0f, 2.0f,
						{ 0.0f, 1.0f, 0.0f, 1.0f },
						{ 1.0f, 0.0f, 0.0f, 1.0f },
						3
					);
				}
			}

			for (auto& object : jsonObject)
			{
				if (object->GetType() == box_ || object->GetType() == wall_)
				{
					object->Update();
					XMFLOAT3 boxPos = object->GetPosition();
					XMFLOAT3 boxScale = object->GetCollisionScale();

					// 障害物を検知していたら攻撃してこない
					if (enemy->ObstacleDetection(pPos, boxPos, boxScale))
					{
						minEnemyLength = 15.0f;
						posEnemySave = {};
						break;
					}
					else
					{
						pPos = player->GetObj()->GetPosition();
						ePos = enemy->GetObj()->GetPosition();

						float length = GetLength(pPos, ePos);

						if (length < minEnemyLength)
						{
							minEnemyLength = length;
							posEnemySave = ePos;
						}
					}
				}
			}
		}
		enemy->Update();
	}
}

void TutorialScene::jsonObjectInit(const std::string sceneName)
{
	// レベルデータの読み込み
	levelData = LevelLoader::LoadFile(sceneName);

	// レベルデータからオブジェクトを生成、配置
	for (LevelData::ObjectData& objectData : levelData->objects)
	{
		if (objectData.fileName == "player")
		{
			// 座標
			XMFLOAT3 pos;
			XMStoreFloat3(&pos, objectData.trans);
			// 大きさ
			XMFLOAT3 scale;
			XMStoreFloat3(&scale, objectData.scale);
			// 当たり判定
			XMFLOAT3 size;
			XMStoreFloat3(&size, objectData.size);

			// プレイヤーを生成
			player->Initialize(pos, scale);
			pPos = player->GetObj()->GetPosition();
			player->GetObj()->SetCollisionScale(size);

			// リスポーンする座標を設定
			reSpawnPos = pos;

			// カメラの設定
			camera->Reset();
			camera->SetTarget(pPos);
			camera->Update();
			continue;
		}

		if (objectData.fileName == "enemy")
		{
			// 座標
			XMFLOAT3 pos;
			XMStoreFloat3(&pos, objectData.trans);
			// 大きさ
			XMFLOAT3 scale;
			XMStoreFloat3(&scale, objectData.scale);
			// 当たり判定
			XMFLOAT3 size;
			XMStoreFloat3(&size, objectData.size);

			// エネミーを生成
			std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
			newEnemy->Initialize(player);
			newEnemy->GetObj()->SetPosition(pos);
			newEnemy->GetObj()->SetScale(scale);
			newEnemy->GetObj()->SetCollisionScale(size);
			enemys.push_back(std::move(newEnemy));
			enemyCount++;
			continue;
		}

		// 3Dオブジェクトを生成
		std::unique_ptr<Object3d> newObject = Object3d::Create();

		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(levelData->models)::iterator it = levelData->models.find(objectData.fileName);
		if (it != levelData->models.end())
		{
			model = it->second;
		}

		newObject->SetModel(model);

		// 座標
		XMFLOAT3 pos;
		XMStoreFloat3(&pos, objectData.trans);
		newObject->SetPosition(pos);

		// 回転角
		XMFLOAT3 rot;
		XMStoreFloat3(&rot, objectData.rot);
		newObject->SetRotation(rot);

		// 大きさ
		XMFLOAT3 scale;
		XMStoreFloat3(&scale, objectData.scale);
		newObject->SetScale(scale);

		// 当たり判定
		XMFLOAT3 colScale;
		XMStoreFloat3(&colScale, objectData.size);
		newObject->SetCollisionScale(colScale);

		if (objectData.fileName == "stage")
		{
			newObject->SetType(stage_);
		}
		else if (objectData.fileName == "box")
		{
			newObject->SetType(box_);
		}
		else if (objectData.fileName == "wall")
		{
			newObject->SetType(wall_);
		}
		else if (objectData.fileName == "pole")
		{
			newObject->SetColor({ 1.0f, 0.1f, 0.1f, 1.0f });
			newObject->SetType(pole_);
		}
		else if (objectData.fileName == "skydome")
		{
			newObject->SetType(skydome_);
		}

		// 配列に登録
		jsonObject.push_back(std::move(newObject));
	}
}

void TutorialScene::jsonObjectUpdate()
{
	for (auto& object : jsonObject)
	{
		object->Update();

		if (object->GetType() == box_)
		{
			XMFLOAT3 boxPos = object->GetPosition();
			XMFLOAT3 boxScale = object->GetCollisionScale();
			player->MapCollide(boxPos, boxScale);
			pPos = player->GetObj()->GetPosition();

			if (rope->Collision(object, pPos))
			{
				rope->SetrFlag(false);
				rushFlag = false;
				catchPos = {};
				elapsedTime = 0.0f;
			}

			for (std::unique_ptr<Enemy>& enemy : enemys)
			{
				enemy->MapCollide(boxPos, boxScale);
			}
		}

		else if (object->GetType() == wall_)
		{
			XMFLOAT3 wallPos = object->GetPosition();
			XMFLOAT3 wallScale = object->GetCollisionScale();
			player->MapCollide(wallPos, wallScale);
			pPos = player->GetObj()->GetPosition();

			if (rope->Collision(object, pPos))
			{
				rope->SetrFlag(false);
				rushFlag = false;
				catchPos = {};
				elapsedTime = 0.0f;
			}

			for (std::unique_ptr<Enemy>& enemy : enemys)
			{
				enemy->MapCollide(wallPos, wallScale);
			}
		}

		else if (object->GetType() == stage_)
		{
			XMFLOAT3 stagePos = object->GetPosition();
			XMFLOAT3 stageScale = object->GetCollisionScale();
			player->StageCollide(stagePos, stageScale);
			pPos = player->GetObj()->GetPosition();

			for (std::unique_ptr<Enemy>& enemy : enemys)
			{
				enemy->StageCollide(stagePos, stageScale);
			}
		}

		else if (object->GetType() == pole_)
		{
			pPos = player->GetObj()->GetPosition();
			XMFLOAT3 polePos = object->GetPosition();
			XMFLOAT3 poleScale = object->GetCollisionScale();
			float length = GetLength(pPos, polePos);

			if (length < minPoleLength)
			{
				minPoleLength = length;
				posPoleSave = polePos;
			}

			if (rope->Collision(object, pPos))
			{
				rushFlag = true;
				catchPos = object->GetPosition();
				posPoleSave = {};
				minPoleLength = 10.0f;
			}

			if (rushFlag && player->PoleCollide(polePos, poleScale))
			{
				rope->SetrFlag(false);
				rushFlag = false;
				catchPos = {};
				elapsedTime = 0.0f;
				throwCount = 0;
			}
		}
	}
}

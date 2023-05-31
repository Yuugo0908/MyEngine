#include "GameScene.h"
#include <imgui.h>
#include <cassert>
#include "SceneManager.h"

void GameScene::Initialize()
{
	rope = Rope::GetInstance();
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

	// パーティクル生成
	effectBox = Particle::Create(L"Resources/effectBox.png");
	effectCircle = Particle::Create(L"Resources/effectCircle.png");
	effectCircle2 = Particle::Create(L"Resources/effectCircle2.png");
	effectTarget = Particle::Create(L"Resources/effectBox2.png");
	effectAvoid = Particle::Create(L"Resources/effectBox2.png");

	enemy->ModelInit();
	rope->Initialize();

	// ライトの生成
	light = Light::Create();
	// ライトの色を設定
	light->SetLightColor({ 1.0f, 1.0f, 1.0f });
	// 3Dオブジェクトにライトをセット
	Object3d::SetLight(light);

	//Bgm->PlayWave("Resources/BGM/bgm.wav", 255, 0.08f);
	jsonObjectInit("stage1");

	// マウスカーソルを非表示
	ShowCursor(false);
}

void GameScene::Finalize()
{
	//Bgm->Stop();

	// マウスカーソルを表示
	ShowCursor(true);

	player->Reset();
	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		enemy->Reset();
	}

	enemys.erase(enemys.begin(), enemys.end());
	rope->Reset();
	jsonObject.erase(jsonObject.begin(), jsonObject.end());

	safe_delete(player);
	safe_delete(enemy);
	safe_delete(light);
	safe_delete(levelData);
	safe_delete(HPText);
	safe_delete(PlayerHPBar);
	safe_delete(PlayerHPGauge);
	safe_delete(effectBox);
	safe_delete(effectCircle);
	safe_delete(effectCircle2);
	safe_delete(effectTarget);
	safe_delete(effectAvoid);
}

void GameScene::Update()
{
	// マウスの移動範囲の制限
	mouse->CursorLimit();

	// フラグの取得
	rFlag = rope->GetrFlag();
	avoidFlag = player->GetAvoidFlag();

#pragma region フェードアウト
	if (!gameClearFlag && !gameOverFlag)
	{
		FadeScene::GetInstance()->FadeOut(1.0f);
	}

	if (gameClearFlag)
	{
		FadeScene::GetInstance()->FadeIn(0.0f);
		if (FadeScene::fadeInEnd)
		{
			reset();
		}
	}
	else if (gameOverFlag)
	{
		FadeScene::GetInstance()->FadeIn(-10.0f);
		if (FadeScene::fadeInEnd)
		{
			reset();
		}
	}
#pragma endregion

#pragma region クリアorゲームオーバー
	// 敵をすべて倒せばクリア
	if (enemyCount <= 0)
	{
		fadeFlag = true;
		gameClearFlag = true;
	}
	// プレイヤーのHPが0になったらゲームオーバー
	else if (playerHp <= 0)
	{
		if (!gameOverFlag)
		{
			// パーティクル生成
			effectBox->CreateParticles(
				pPos,
				2.0f, 5.0f,
				{ 0.0f, 0.0f, 1.0f, 1.0f },
				{ 1.0f, 0.0f, 0.0f, 1.0f },
				5, 60, true, true
			);
			effectCircle2->CreateParticles(
				pPos,
				1.0f, 10.0f,
				{ 0.0f, 0.0f, 1.0f, 1.0f },
				{ 0.0f, 0.0f, 1.0f, 1.0f },
				5, 10, false, false
			);
			alpha = -1.0f;
		}

		// ゲームオーバーフラグ
		fadeFlag = true;
		gameOverFlag = true;
		return;
	}
#pragma endregion

	if (FadeScene::fadeOutEnd)
	{
		// プレイヤーの突進
		player->Rush(catchPos, rushFlag, elapsedTime);
		// タイトルから移行後の更新
		player->Update();
	}

	// プレイヤーの座標、半径の設定
	pPos = player->GetObj()->GetPosition();
	pScale = player->GetObj()->GetScale();

	if (FadeScene::fadeOutEnd)
	{
		// ロープの更新
		RopeUpdate();
		// 敵の更新
		EnemyUpdate();
	}

	// jsonファイルから読み込んだオブジェクトの更新
	jsonObjectUpdate();
	// ライトの更新
	LightUpdate();
	// カメラの更新
	CameraUpdate();

	// 回避した際にエフェクトが発生
	if (avoidFlag)
	{
		effectAvoid->CreateParticles(
			pPos, 1.0f, 2.0f,
			{ 0.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, 0.0f, 0.0f, 1.0f },
			1, 10, false, false
		);
	}
	effectAvoid->Update();
}

void GameScene::Draw()
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
	if (playerHp > 0.0f)
	{
		player->GetObj()->Draw();
	}

	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		enemy->Draw();
	}
	rope->GetObj()->Draw();

	for (auto& object : jsonObject)
	{
		if (object->GetType() == "stage")
		{
			object->Draw();
		}
		else if (object->GetDrawFlag())
		{
			object->Draw();
		}
	}

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();
#pragma endregion 3Dオブジェクト描画

#pragma region パーティクル描画

	// パーティクル描画前処理
	Particle::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	// パーティクルの描画
	effectBox->Draw();
	effectCircle->Draw();
	effectCircle2->Draw();
	effectTarget->Draw();
	effectAvoid->Draw();

	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		enemy->reactionDraw();
	}

	// パーティクル描画後処理
	Particle::PostDraw();

#pragma endregion パーティクル描画

#pragma region 前景画像描画
	// 前景画像描画前処理
	Image2d::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	// 前景画像の描画
	HPText->Draw();
	PlayerHPBar->Draw();

	PlayerHPGauge->SetSize({ playerHp,60 });
	PlayerHPGauge->Draw();

	// フェードの描画
	FadeScene::GetInstance()->Draw();

	// デバッグテキストの描画
	DebugText::GetInstance()->DrawAll(DirectXCommon::GetInstance()->GetCommandList());
	// 画像描画後処理
	Image2d::PostDraw();
#pragma endregion 前景画像描画
}

void GameScene::reset()
{
	if (gameClearFlag)
	{
		// 1ステージ目をクリアしたら次のステージへ
		if (stageClearCount == 0)
		{
			jsonObject.erase(jsonObject.begin(), jsonObject.end());
			stageClearCount++;
			fadeFlag = false;
			gameClearFlag = false;
			levelData = nullptr;
			jsonObjectInit("stage2");
		}
		// すべてのステージをクリアしたらゲームクリア
		else if (stageClearCount == 1)
		{
			SceneManager::GetInstance()->ChangeScene("GameClear");
		}
	}
	// ゲームオーバーシーンに移行
	else if (gameOverFlag)
	{
		SceneManager::GetInstance()->ChangeScene("GameOver");
	}
}

void GameScene::SetImgui()
{
	ImGui::Begin("DebugText");
	ImGui::SetWindowSize(ImVec2(300, 200));
	ImGui::Separator();
	ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);

	ImGui::Separator();

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
	cPos = camera->GetEye();
	cTarget = camera->GetTarget();

	if (gameClearFlag)
	{
		camera->SetTarget(pPos);
		if (pPos.y <= trackLimit)
		{
			player->ReSpawn();
		}
	}
	else
	{
		if (pPos.y >= trackLimit)
		{
			camera->SetTarget(pPos);
		}
		else
		{
			playerHp = 0;
		}
	}

	//カメラ更新
	if (shakeFlag == true)
	{
		camera->CameraShake(shakeFlag);
	}

	camera->Update();
}

void GameScene::EnemyUpdate()
{
	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		pPos = player->GetObj()->GetPosition();
		getEnemyAlive = enemy->GetAlive();

		if (!getEnemyAlive)
		{
			enemy->Update();
			continue;
		}

		if (!rushFlag && rope->GetrFlag())
		{
			rushFlag = true;
			catchPos = posSave;
		}

		if (rFlag && enemy->EnemyCollision(player->GetObj()))
		{
			// カメラシェイクの発生
			shakeFlag = true;

			// パーティクル生成
			effectBox->CreateParticles(
				enemy->GetObj()->GetPosition(),
				2.0f, 5.0f,
				{ 1.0f, 0.0f, 0.0f, 1.0f },
				{ 0.0f, 1.0f, 0.0f, 1.0f },
				5, 60, true, true
			);
			effectCircle2->CreateParticles(
				enemy->GetObj()->GetPosition(),
				1.0f, 10.0f,
				{ 1.0f, 0.0f, 0.0f, 1.0f },
				{ 1.0f, 0.0f, 0.0f, 1.0f },
				5, 10, false, false
			);

			rope->SetrFlag(false);
			rushFlag = false;
			catchPos = {};
			elapsedTime = 0.0f;
			enemyCount--;
			controller->Vibration();
			enemys.remove(enemy);
			break;
		}
		else if (!rFlag && player->Damage(enemy->GetObj()))
		{
			shakeFlag = true;
			playerHp -= 18;

			// パーティクル生成
			effectCircle->CreateParticles(
				player->GetObj()->GetPosition(),
				1.0f, 2.0f,
				{ 0.0f, 1.0f, 0.0f, 1.0f },
				{ 1.0f, 0.0f, 0.0f, 1.0f },
				3, 60, true, true
			);
		}

		if (!avoidFlag && enemy->BulletCollision())
		{
			if (!rFlag)
			{
				shakeFlag = true;
				playerHp -= 18;

				// パーティクル生成
				effectCircle->CreateParticles(
					player->GetObj()->GetPosition(),
					1.0f, 2.0f,
					{ 0.0f, 1.0f, 0.0f, 1.0f },
					{ 1.0f, 0.0f, 0.0f, 1.0f },
					3, 60, true, true
				);
			}
		}

		for (auto& object : jsonObject)
		{
			if (object->GetType() == "box" || object->GetType() == "wall")
			{
				object->Update();
				XMFLOAT3 boxPos = object->GetPosition();
				XMFLOAT3 boxScale = object->GetCollisionScale();

				// 障害物を検知していたら攻撃してこない
				if (enemy->ObstacleDetection(pPos, boxPos, boxScale))
				{
					break;
				}
				else
				{
					// プレイヤーと敵の座標と距離を取得
					pPos = player->GetObj()->GetPosition();
					ePos = enemy->GetObj()->GetPosition();
					float length = GetLength(pPos, ePos);

					// ロープが届く距離にいた場合、その敵の座標と距離を保存
					if (length < minEnemyLength)
					{
						posEnemySave = ePos;
						minEnemyLength = length;
					}
				}
			}
		}
		enemy->Update();
	}
}

void GameScene::RopeUpdate()
{
	rope->Update(pPos);

	// ポールと敵の距離を比較して短い方を代入
	float minLength = (std::min)(minEnemyLength, minPoleLength);

	// どちらも基準内の距離だった場合、敵を優先する
	if (minEnemyLength < baseLength && minPoleLength < baseLength)
	{
		minLength = minEnemyLength;
	}

	// 距離に応じて代入する座標を変更(敵を優先する)
	if (minLength < baseLength)
	{
		if (minLength == minEnemyLength)
		{
			posSave = posEnemySave;
		}
		else
		{
			posSave = posPoleSave;
		}

		for (auto& object : jsonObject)
		{
			if (object->GetType() == "box" || object->GetType() == "wall")
			{
				XMFLOAT3 pos = object->GetPosition();
				XMFLOAT3 scale = object->GetCollisionScale();
				if (GetLength(pPos, pos) >= baseLength)
				{
					continue;
				}
				else if (Collision::CollisionRayBox(pPos, posSave, pos, scale))
				{
					minLength = baseLength;
					rope->SetThrowFlag(false);
					break;
				}
			}
		}

		// 周辺にあるポールには反応しない
		if (GetLength(posSave, oldPosSave) <= 0.0f && minLength == minPoleLength)
		{
			minLength = minEnemyLength;
			posSave = posEnemySave;
		}
		
		if (minLength < baseLength)
		{
			effectTarget->TargetEffect(
				posSave, 3.0f, 1.0f,
				{ 1.0f, 0.0f, 0.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				targetEffectCount
			);
		}
	}
	else
	{
		rope->SetThrowFlag(false);
	}

	if (GetLength(pPos, oldPosSave) > 30.0f)
	{
		oldPosSave = {};
	}

	if (minLength < baseLength)
	{
		rope->Throw(pPos, posSave, minLength);
		// ロープを飛ばした方向にプレイヤーも向く
		player->TrackRot(pPos, posSave);
	}
	else
	{
		float cameraRot = camera->CameraAngle(pPos);
		// カメラが向いている方向にプレイヤーも向く
		player->GetObj()->SetRotation({ 0, XMConvertToDegrees(cameraRot), 0 });
		rope->SetThrowFlag(false);
	}

	posPoleSave = {};
	posEnemySave = {};
	minPoleLength = 15.0f;
	minEnemyLength = 15.0f;
}

void GameScene::jsonObjectInit(const std::string sceneName)
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
			player->Update();
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
			newEnemy->SetRespawnPos(pos);
			newEnemy->Update();
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

		// オブジェクトのタイプをセット
		newObject->SetType(objectData.objType);
		newObject->Update();

		// 配列に登録
		jsonObject.push_back(std::move(newObject));
	}
}

void GameScene::jsonObjectUpdate()
{
	for (auto& object : jsonObject)
	{
		XMFLOAT3 pos = object->GetPosition();
		XMFLOAT3 scale = object->GetCollisionScale();

		// プレイヤーとカメラの間にオブジェクトがあった時、オブジェクトを描画しない
		if (Collision::CollisionRayBox(cPos, pPos, pos, scale))
		{
			object->SetDrawFlag(false);
		}
		else
		{
			object->SetDrawFlag(true);
		}

		if (object->GetType() == "box")
		{
			XMFLOAT3 boxPos = object->GetPosition();
			XMFLOAT3 boxScale = object->GetCollisionScale();
			player->MapCollide(boxPos, boxScale);
			pPos = player->GetObj()->GetPosition();

			for (std::unique_ptr<Enemy>& enemy : enemys)
			{
				enemy->MapCollide(boxPos, boxScale);
			}
		}

		else if (object->GetType() == "wall")
		{
			XMFLOAT3 wallPos = object->GetPosition();
			XMFLOAT3 wallScale = object->GetCollisionScale();
			player->MapCollide(wallPos, wallScale);
			pPos = player->GetObj()->GetPosition();

			for (std::unique_ptr<Enemy>& enemy : enemys)
			{
				enemy->MapCollide(wallPos, wallScale);
			}
		}

		else if (object->GetType() == "stage")
		{
			bool reverseFlag = false;
			XMFLOAT3 stagePos = object->GetPosition();
			XMFLOAT3 stageScale = object->GetCollisionScale();
			player->StageCollide(stagePos, stageScale, reverseFlag);
			pPos = player->GetObj()->GetPosition();

			// 背面に当たったら、ロープの接着を解除する
			if (reverseFlag)
			{
				rope->SetrFlag(false);
				rushFlag = false;
				catchPos = {};
				elapsedTime = 0.0f;
			}

			for (std::unique_ptr<Enemy>& enemy : enemys)
			{
				enemy->StageCollide(stagePos, stageScale);
			}
		}

		else if (object->GetType() == "pole")
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

			if (!rushFlag && rope->GetrFlag())
			{
				rushFlag = true;
				catchPos = posSave;
			}

			if (rushFlag && player->PoleCollide(polePos, poleScale))
			{
				rope->SetrFlag(false);
				rushFlag = false;
				catchPos = {};
				elapsedTime = 0.0f;
				oldPosSave = posPoleSave;
			}
		}

		object->Update();
	}
}
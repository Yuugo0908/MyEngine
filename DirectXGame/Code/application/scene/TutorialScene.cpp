#include "TutorialScene.h"
#include <imgui.h>
#include <cassert>
#include "SceneManager.h"

void TutorialScene::Initialize()
{
	rope = Rope::GetInstance();
	player = new Player;
	enemy = new Enemy;

	// 3Dオブジェクトにカメラをセット
	Object3d::SetCamera(camera);

	// プレイヤーHP表示用画像の生成
	HPText = Image2d::Create(Image2d::ImgNumber::HPTextNum, { 0.0f,0.0f });
	HPText->SetSize({ 1280.0f,720.0f });
	PlayerHPBar = Image2d::Create(Image2d::ImgNumber::HPBarNum, { 0.0f,45.0f });
	PlayerHPBar->SetSize({ 360.0f,60.0f });
	PlayerHPGauge = Image2d::Create(Image2d::ImgNumber::HPGaugeNum, { 0.0f,45.0f });
	PlayerHPGauge->SetSize({ 30.0f,60.0f });

	// チュートリアル用画像の生成
	wasdKey = Image2d::Create(Image2d::ImgNumber::wasdNum, { 0.0f, 0.0f });
	wasdKey->SetPosition({50.0f, 400.0f});
	wasdKey->SetSize({ 256.0f,256.0f });
	spaceKey = Image2d::Create(Image2d::ImgNumber::spaceNum, { 0.0f, 0.0f });
	spaceKey->SetPosition({ 512.0f, 500.0f });
	spaceKey->SetSize({ 256.0f,256.0f });
	mouseImg = Image2d::Create(Image2d::ImgNumber::mouseNum, { 0.0f, 0.0f });
	mouseImg->SetPosition({ 1000.0f, 400.0f });
	mouseImg->SetSize({ 256.0f,256.0f });
	mouseLeftImg = Image2d::Create(Image2d::ImgNumber::mouseLeftNum, { 0.0f, 0.0f });
	mouseLeftImg->SetPosition({ 1000.0f, 400.0f });
	mouseLeftImg->SetSize({ 256.0f,256.0f });
	mouseRightImg = Image2d::Create(Image2d::ImgNumber::mouseRightNum, { 0.0f, 0.0f });
	mouseRightImg->SetPosition({ 1000.0f, 400.0f });
	mouseRightImg->SetSize({ 256.0f,256.0f });

	// パーティクル生成
	effectBox = Particle::Create("effectBox");
	effectCircle = Particle::Create("effectCircle");
	effectCircle2 = Particle::Create("effectCircle2");
	effectTarget = Particle::Create("effectBox2");
	effectAvoid = Particle::Create("effectBox2");

	enemy->ModelInit();
	rope->Initialize();

	// ライトの生成
	light = Light::Create();
	// ライトの色を設定
	light->SetLightColor({ 1.0f, 1.0f, 1.0f });
	// 3Dオブジェクトにライトをセット
	Object3d::SetLight(light);

	//Bgm->PlayWave("Resources/BGM/bgm.wav", 255, 0.08f);
	jsonObjectInit("test");

	// マウスカーソルを非表示
	ShowCursor(false);
}

void TutorialScene::Finalize()
{
	//Bgm->Stop();

	// マウスカーソルを表示
	ShowCursor(true);

	player->Reset();
	for (std::unique_ptr<Enemy>& enemy : enemies)
	{
		enemy->Reset();
	}
	enemies.erase(enemies.begin(), enemies.end());
	rope->Reset();
	jsonObject.erase(jsonObject.begin(), jsonObject.end());
	jsonObject.shrink_to_fit();

	safe_delete(player);
	safe_delete(enemy);
	safe_delete(levelData);
	safe_delete(HPText);
	safe_delete(PlayerHPBar);
	safe_delete(PlayerHPGauge);
	safe_delete(wasdKey);
	safe_delete(spaceKey);
	safe_delete(mouseImg);
	safe_delete(mouseLeftImg);
	safe_delete(mouseRightImg);
	safe_delete(effectBox);
	safe_delete(effectCircle);
	safe_delete(effectCircle2);
	safe_delete(effectTarget);
	safe_delete(effectAvoid);
}

void TutorialScene::Update()
{
	// マウスの移動範囲の制限
	mouse->CursorLimit();

	// フラグの取得
	rFlag = rope->GetrFlag();
	avoidFlag = player->GetAvoidFlag();

#pragma region フェード
	if (!gameClearFlag && !gameOverFlag)
	{
		FadeScene::GetInstance()->FadeOut(1.0f);
	}

	if (gameClearFlag)
	{
		FadeScene::GetInstance()->FadeIn(0.0f);
		if (FadeScene::fadeInEnd)
		{
			Reset();
		}
	}
	else if (gameOverFlag)
	{
		FadeScene::GetInstance()->FadeIn(-1.0f);
		if (FadeScene::fadeInEnd)
		{
			gameOverFlag = false;
			playerHp = 360;
			player->ReSpawn();
		}
	}
#pragma endregion

#pragma region クリアorゲームオーバー
	// 敵をすべて倒せばクリア
	if (enemyCount <= 0)
	{
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
				5, 20, false, false
			);
		}

		// ゲームオーバーフラグ
		gameOverFlag = true;
		return;
	}
#pragma endregion

#pragma region チュートリアル
	switch (tutorialState)
	{
	case ropeThrow:
		// 初めてポールにロープを飛ばす場合
		if (throwFlag && !avoidFlag)
		{
			if (controller->GetPadState(Controller::State::X, Controller::Type::TRIGGER) || mouse->TriggerMouseLeft())
			{
				// ロープを発射させ、次のチュートリアルに進む
				rope->SetThrowFlag(true);

				// ステージオブジェクトの色を元に戻す
				for (auto& object : jsonObject)
				{
					object->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
				}
				tutorialState = Avoid;
			}
			else
			{
				// ターゲットエフェクトの発生
				effectTarget->TargetEffect(
					targetPos, 3.0f, 1.0f,
					{ 1.0f, 0.0f, 0.0f, 1.0f },
					{ 1.0f, 1.0f, 1.0f, 1.0f },
					targetEffectCount
				);

				// ポール以外のステージオブジェクトの色を暗くする
				for (auto& object : jsonObject)
				{
					if (object->GetType() != "pole")
					{
						object->SetColor({ 0.3f, 0.3f, 0.3f, 1.0f });
					}
					object->Update();
				}
				return;
			}
		}

		break;
	case Avoid:
		// 初めて回避を使う場合
		if (enemyAttackFlag && !avoidFlag)
		{
			if (controller->GetPadState(Controller::State::RT, Controller::Type::TRIGGER) || mouse->TriggerMouseRight())
			{
				// 回避を発生させ、次のチュートリアルに進む
				player->SetAvoidFlag(true);
				player->Avoid();

				// ステージオブジェクトの色を元に戻す
				for (auto& object : jsonObject)
				{
					object->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
				}
				tutorialState = Attack;
			}
			else
			{
				// ステージオブジェクトの色を暗くする
				for (auto& object : jsonObject)
				{
					object->SetColor({ 0.3f, 0.3f, 0.3f, 1.0f });
					object->Update();
				}
				return;
			}
		}
		break;
	case Attack:
		// 初めて敵を攻撃する場合
		if (playerAttackFlag && !avoidFlag)
		{
			if (controller->GetPadState(Controller::State::X, Controller::Type::TRIGGER) || mouse->TriggerMouseLeft())
			{
				// ロープを発射させる(チュートリアルは終了)
				rope->SetThrowFlag(true);

				// ステージオブジェクトの色を元に戻す
				for (auto& object : jsonObject)
				{
					object->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
				}
				tutorialState = None;
			}
			else
			{
				// ターゲットエフェクトの発生
				effectTarget->TargetEffect(
					targetPos, 3.0f, 1.0f,
					{ 1.0f, 0.0f, 0.0f, 1.0f },
					{ 1.0f, 1.0f, 1.0f, 1.0f },
					targetEffectCount
				);

				// ステージオブジェクトの色を暗くする
				for (auto& object : jsonObject)
				{
					object->SetColor({ 0.3f, 0.3f, 0.3f, 1.0f });
					object->Update();
				}
				return;
			}
		}
		break;
	default:
		break;
	}
# pragma endregion

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
		// 敵の更新
		EnemyUpdate();
	}
	// jsonファイルから読み込んだオブジェクトの更新
	jsonObjectUpdate();
	if (FadeScene::fadeOutEnd)
	{
		// ロープの更新
		RopeUpdate();
	}
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
	if (playerHp > 0.0f)
	{
		player->GetObj()->Draw();
	}

	for (std::unique_ptr<Enemy>& enemy : enemies)
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

	for (std::unique_ptr<Enemy>& enemy : enemies)
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

	wasdKey->Draw();
	spaceKey->Draw();
	mouseImg->Draw();


	if ((playerAttackFlag || throwFlag) && (tutorialState == ropeThrow || tutorialState == Attack))
	{
		imgShowCount++;
		if (imgShowCount >= 30)
		{
			mouseLeftImg->Draw();
			if (imgShowCount >= 60)
			{
				imgShowCount = 0;
			}
		}
	}
	else if (enemyAttackFlag && tutorialState == Avoid)
	{
		imgShowCount++;
		if (imgShowCount >= 30)
		{
			mouseRightImg->Draw();
			if (imgShowCount >= 60)
			{
				imgShowCount = 0;
			}
		}
	}

	// フェードの描画
	FadeScene::GetInstance()->Draw();

	// デバッグテキストの描画
	DebugText::GetInstance()->DrawAll(DirectXCommon::GetInstance()->GetCommandList());
	// 画像描画後処理
	Image2d::PostDraw();
#pragma endregion 前景画像描画
}

void TutorialScene::Reset()
{
	// ゲーム、ステージ1からスタート
	if (gameClearFlag)
	{
		SceneManager::GetInstance()->ChangeScene("Game");
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
	ImGui::Separator();
	ImGui::Text("cameraPosX : %6.2f", cPos.x);
	ImGui::Text("cameraPosY : %6.2f", cPos.y);
	ImGui::Text("cameraPosZ : %6.2f", cPos.z);

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

void TutorialScene::EnemyUpdate()
{
	for (std::unique_ptr<Enemy>& enemy : enemies)
	{
		pPos = player->GetObj()->GetPosition();
		rFlag = rope->GetrFlag();
		getEnemyAlive = enemy->GetAlive();

		// 敵が死んでいたら次の敵の処理に進む
		if (!getEnemyAlive)
		{
			enemy->Update();
			continue;
		}

		// 突進した状態で敵に触れたら
		if (rushFlag && enemy->EnemyCollision(player->GetObj()))
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
			enemies.remove(enemy);
			break;
		}

		// 突進せずに敵に触れたら
		if (!rushFlag && player->Damage(enemy->GetObj()))
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

		// プレイヤーに敵の弾が当たった際のダメージ処理
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

		// ロープを発射または接着していない状態であれば
		bool getThrowFlag = rope->GetThrowFlag();
		if (enemy->Danger() && !getThrowFlag && !rFlag)
		{
			enemyAttackFlag = true;
		}

		// プレイヤーと敵の間の障害物検知
		for (auto& object : jsonObject)
		{
			if (object->GetType() == "box" || object->GetType() == "wall")
			{
				object->Update();
				XMFLOAT3 boxPos = object->GetPosition();
				XMFLOAT3 boxScale = object->GetCollisionScale();

				// 障害物を検知していたら攻撃してこない(プレイヤーも敵に攻撃することはできない)
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
						targetEnemyPos = ePos;
						minEnemyLength = length;
					}
				}
			}
		}
		enemy->Update();
	}
}

void TutorialScene::RopeUpdate()
{
	rope->Update(pPos);

	// ポールと敵の距離を比較して短い方を代入
	float minLength = (std::min)(minEnemyLength, minPoleLength);

	// どちらも基準内の距離だった場合、敵を優先する
	if (minEnemyLength < baseLength && minPoleLength < baseLength)
	{
		minLength = minEnemyLength;
	}

	// ターゲットが敵だった場合
	if (minLength == minEnemyLength)
	{
		targetPos = targetEnemyPos;
		playerAttackFlag = true;
	}
	// ターゲットがポールだった場合
	else
	{
		targetPos = targetPolePos;
		throwFlag = true;
	}

	// 障害物を検知したらターゲットしない
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

			if (Collision::CollisionRayBox(pPos, targetPos, pos, scale))
			{
				minLength = baseLength;
				rope->SetThrowFlag(false);
				throwFlag = false;
				playerAttackFlag = false;
				break;
			}
		}
	}

	// 障害物を検知していなければ
	if (minLength < baseLength)
	{
		// 過去にターゲットしたポールには反応しない
		if (minLength == minPoleLength && GetLength(targetPos, oldTargetPos) <= 0.0f)
		{
			minLength = minEnemyLength;
			targetPos = targetEnemyPos;
			throwFlag = false;
		}

		// ターゲットが距離内にいればターゲットエフェクトを発生させる
		if (minLength < baseLength)
		{
			effectTarget->TargetEffect(
				targetPos, 3.0f, 1.0f,
				{ 1.0f, 0.0f, 0.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				targetEffectCount
			);

			// ターゲットしている方向にプレイヤーも向く
			player->TrackRot(pPos, targetPos);
			camera->SetTarget(targetPos);
		}
		else
		{
			float cameraRot = camera->CameraAngle(pPos);
			// カメラが向いている方向にプレイヤーも向く
			player->GetObj()->SetRotation({ 0, XMConvertToDegrees(cameraRot), 0 });
			rope->SetThrowFlag(false);
			throwFlag = false;
			playerAttackFlag = false;
		}

		// ロープを接着したら突進を開始する
		if (!rushFlag && rFlag)
		{
			rushFlag = true;
			catchPos = targetPos;
		}

		rope->Throw(pPos, targetPos, minLength);
	}
	else
	{
		float cameraRot = camera->CameraAngle(pPos);
		// カメラが向いている方向にプレイヤーも向く
		player->GetObj()->SetRotation({ 0, XMConvertToDegrees(cameraRot), 0 });
		rope->SetThrowFlag(false);
		throwFlag = false;
		playerAttackFlag = false;
	}

	// 過去にターゲットしたしたオブジェクトから一定の距離離れていたら
	if (GetLength(pPos, oldTargetPos) > 30.0f)
	{
		// 過去にターゲットしたオブジェクトの座標をリセットする
		oldTargetPos = { 1000.0f, 1000.0f, 1000.0f };
	}

	targetPolePos = {};
	minPoleLength = 15.0f;
	targetEnemyPos = {};
	minEnemyLength = 15.0f;
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
			enemies.push_back(std::move(newEnemy));
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

void TutorialScene::jsonObjectUpdate()
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

		// タイプごとに処理を実行してUpdate
		if (object->GetType() == "box")
		{
			XMFLOAT3 boxPos = object->GetPosition();
			XMFLOAT3 boxScale = object->GetCollisionScale();
			player->MapCollide(boxPos, boxScale);
			pPos = player->GetObj()->GetPosition();

			for (std::unique_ptr<Enemy>& enemy : enemies)
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

			for (std::unique_ptr<Enemy>& enemy : enemies)
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

			if (reverseFlag)
			{
				rope->SetrFlag(false);
				rushFlag = false;
				catchPos = {};
				elapsedTime = 0.0f;
			}

			for (std::unique_ptr<Enemy>& enemy : enemies)
			{
				enemy->StageCollide(stagePos, stageScale);
			}
		}

		else if (object->GetType() == "pole")
		{
			pPos = player->GetObj()->GetPosition();
			rFlag = rope->GetrFlag();
			XMFLOAT3 polePos = object->GetPosition();
			XMFLOAT3 poleScale = object->GetCollisionScale();
			float length = GetLength(pPos, polePos);

			// ポールの中から一番近い距離のポールを決める
			if (length < minPoleLength && length < baseLength)
			{
				minPoleLength = length;
				targetPolePos = polePos;
			}
			
			// プレイヤーがポールに触れたらロープの接着をなくして突進を止める
			if (rushFlag && player->PoleCollide(polePos, poleScale))
			{
				rope->SetrFlag(false);
				rushFlag = false;
				catchPos = {};
				elapsedTime = 0.0f;
				oldTargetPos = polePos;
			}
		}

		object->Update();
	}
}
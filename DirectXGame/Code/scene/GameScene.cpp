﻿#include "GameScene.h"
#include <imgui.h>
#include <cassert>
#include "SceneManager.h"

void GameScene::Initialize() {
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
	box_effect = Particle::Create(L"Resources/box_effect.png");

	enemy->ModelInit();
	rope->Initialize();

	// ライトの生成
	light = Light::Create();
	// ライトの色を設定
	light->SetLightColor({ 1.0f, 1.0f, 1.0f });
	// 3Dオブジェクトにライトをセット
	Object3d::SetLight(light);

	jsonObjectInit("gameScene");
}

void GameScene::Finalize()
{
	safe_delete(player);
	safe_delete(enemy);
	safe_delete(rope);
	safe_delete(light);
	safe_delete(levelData);
	safe_delete(HPText);
	safe_delete(PlayerHPBar);
	safe_delete(PlayerHPGauge);
	safe_delete(fadeTex);
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
			XMFLOAT3 size;
			XMStoreFloat3(&size, objectData.size);

			// プレイヤーを生成
			player->Initialize(pos);
			pPos = player->GetObj()->GetPosition();
			player->GetObj()->SetCollisionScale(size);
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
			XMFLOAT3 size;
			XMStoreFloat3(&size, objectData.size);

			// エネミーを生成
			std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
			newEnemy->Initialize(player);
			newEnemy->GetObj()->SetPosition(pos);
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
		if (it != levelData->models.end()) {
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

void GameScene::Update()
{
	// タイトルから移行後の更新
	//	プレイヤーの座標、半径の設定
	player->Update(rFlag, moveFlag);
	pPos = player->GetObj()->GetPosition();
	pScale = player->GetObj()->GetScale();
	if (attackFlag)
	{
		player->Rush(catchPos, attackFlag, avoidTime);
	}
	rope->Update(pPos);

	// jsonファイルから読み込んだオブジェクトの更新
	jsonObjectUpdate();
	CollisionUpdate();
	LightUpdate();
	CameraUpdate();
	box_effect->Update();

	// フラグの取得
	rFlag = rope->GetrFlag();
	moveFlag = rope->GetmoveFlag();
	avoidFlag = player->GetAvoidFlag();
	onGround = player->GetOnGround();

	if (throwCount < 30)
	{
		throwCount++;
	}

	if (throwCount == 30)
	{
		rope->Throw(pPos, posSave, minLength);
		posSave = {};
		minLength = 15.0f;
	}
	player->GetObj()->SetPosition(pPos);

	if (enemyHp <= 0)
	{
		fadeFlag = true;
		gameClearFlag = true;
	}
	else if (playerHp <= 0)
	{
		fadeFlag = true;
		gameOverFlag = true;
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

void GameScene::reset()
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
		SceneManager::GetInstance()->ChangeScene("GameClear");
	}
	else if (gameOverFlag)
	{
		SceneManager::GetInstance()->ChangeScene("GameOver");
	}
}

void GameScene::Draw() {

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
	box_effect->Draw(DirectXCommon::GetInstance()->GetCommandList());

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

	ImGui::End();
}

void GameScene::CreateParticles(XMFLOAT3 setPos, float startScale, float endScale, XMFLOAT4 startColor, XMFLOAT4 endColor, int count)
{
	for (int i = 0; i < count; i++) {
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
		box_effect->Add(60, pos, vel, acc, startScale, endScale, startColor, endColor);
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

void GameScene::jsonObjectUpdate()
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
				attackFlag = false;
				catchPos = {};
				avoidTime = 0.0f;
			}

			for (std::unique_ptr<Enemy>& enemy : enemys)
			{
				enemy->MapCollide(boxPos, boxScale);
				pPos = player->GetObj()->GetPosition();
				ePos = enemy->GetObj()->GetPosition();
				eScale = enemy->GetObj()->GetScale();
				float length = GetLength(pPos, ePos);

				if (length < minLength)
				{
					minLength = length;
					posSave = ePos;
				}

				if (length <= 15.0f)
				{
					if (RayCollision(pPos, ePos, boxPos, boxScale))
					{
						enemy->SetAttackFlag(false);
						posSave = {};
						minLength = 15.0f;
					}
					else
					{
						enemy->SetAttackFlag(true);
					}
				}
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
				attackFlag = false;
				catchPos = {};
				avoidTime = 0.0f;
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

			if (length < 15.0f && length < minLength)
			{
				minLength = length;
				posSave = polePos;
			}

			if (rope->Collision(object, pPos))
			{
				attackFlag = true;
				catchPos = object->GetPosition();
			}

			if (attackFlag && player->PoleCollide(polePos, poleScale))
			{
				rope->SetrFlag(false);
				attackFlag = false;
				catchPos = {};
				avoidTime = 0.0f; 
				throwCount = 0;
			}
		}
	}
}

void GameScene::CollisionUpdate()
{
	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		pPos = player->GetObj()->GetPosition();
		eAlive = enemy->GetAlive();

		if (eAlive)
		{
			if (rope->Collision(enemy->GetObj(), pPos))
			{
				attackFlag = true;
				catchPos = enemy->GetObj()->GetPosition();
			}

			if (rFlag && enemy->EnemyCollision(player->GetObj()))
			{
				shakeFlag = true;
				moveFlag = true;
				rope->SetmoveFlag(moveFlag);
				// パーティクル生成
				CreateParticles(
					enemy->GetObj()->GetPosition(),
					2.0f, 5.0f,
					{ 1.0f, 0.0f, 0.0f, 1.0f },
					{ 0.0f, 1.0f, 0.0f, 1.0f },
					5
				);
				rope->SetrFlag(false);
				attackFlag = false;
				catchPos = {};
				avoidTime = 0.0f;
				enemyHp -= (360.0f / enemyCount);
				controller->Vibration();
				enemys.remove(enemy);
				break;
			}

			if (player->Damage(enemy->GetObj()))
			{
				shakeFlag = true;
				moveFlag = true;
				rope->SetmoveFlag(moveFlag);
				playerHp -= 36;
			}

			if (enemy->BulletCollision())
			{
				if (!rFlag)
				{
					shakeFlag = true;
					playerHp -= 18;
				}
			}
		}

		enemy->Update();
	}
}

bool GameScene::RayCollision(const XMFLOAT3 startPos, const XMFLOAT3 endPos, const XMFLOAT3 boxPos, const XMFLOAT3 boxScale)
{
	// ワールド空間での光線の基点
	XMFLOAT3 layStart = startPos;
	// ワールド空間での光線の方向
	XMVECTOR playerPos = { startPos.x, startPos.y, startPos.z, 0 };
	XMVECTOR enemyPos = { endPos.x, endPos.y, endPos.z, 0 };
	XMVECTOR subPlayerEnemy = XMVectorSubtract(playerPos, enemyPos);
	XMVECTOR NsubPlayerEnemy = XMVector3Normalize(subPlayerEnemy);
	XMFLOAT3 layVec = { NsubPlayerEnemy.m128_f32[0], NsubPlayerEnemy.m128_f32[1], NsubPlayerEnemy.m128_f32[2] };

	bool crossFlag = true;

	float t_min = -FLT_MAX;
	float t_max = FLT_MAX;
	float max[3] = { boxPos.x + boxScale.x, boxPos.y + boxScale.y, boxPos.z + boxScale.z };
	float min[3] = { boxPos.x - boxScale.x, boxPos.y - boxScale.y, boxPos.z - boxScale.z };
	float p[3] = { layStart.x, layStart.y, layStart.z };
	float d[3] = { layVec.x, layVec.y, layVec.z };

	for (int i = 0; i < 3; i++)
	{
		if (abs(d[i]) < FLT_EPSILON)
		{
			if (p[i] < min[i] || p[i] > max[i])
			{
				crossFlag = false; // 交差していない
				continue;
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
				crossFlag = false;
				continue;
			}
		}
	}

	if (!crossFlag)
	{
		return false;
	}
	else
	{
		XMFLOAT3 colPosMin = layStart + (layVec * t_min);
		XMFLOAT3 colPosMax = layStart + (layVec * t_max);

		if (GetLength(startPos, colPosMin) >= GetLength(startPos, colPosMax) && GetLength(endPos, colPosMin) <= GetLength(endPos, colPosMax))
		{
			CreateParticles(colPosMin, 0.0f, 5.0f, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, 1);
			CreateParticles(colPosMax, 0.0f, 5.0f, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, 1);
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}
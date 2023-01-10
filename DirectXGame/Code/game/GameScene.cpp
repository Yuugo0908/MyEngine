#include "GameScene.h"
#include <imgui.h>
#include <cassert>

GameScene::GameScene() {

}

GameScene::~GameScene() {

}

// 静的メンバ変数の実体
Model* GameScene::blockModel;
std::vector<std::vector<int>> GameScene::map;
std::unique_ptr<Object3d> GameScene::blockObj[map_max_y][map_max_x];

int GameScene::height = 0;
int GameScene::width = 0;
const float GameScene::LAND_SCALE = 5.0f;


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
			stagePos = pos;
			stageScale = scale;
		}

		// 配列に登録
		objects.push_back(std::move(newObject));
	}

	Enemy::StaticInit();

	rope->Initialize(keyboard, mouse);
	player->Initialize(keyboard, mouse);
	pPos = player->GetPos();
	pPosOld = pPos;
	pRadius = player->GetScale();

	for (int i = 0; i < enemyCount; i++)
	{
		std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
		newEnemy->Initialize(player);
		enemys.push_back(std::move(newEnemy));
	}

	// オブジェクト生成
	blockModel = blockModel->CreateFromObject("block");

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
	//enemy->Finalize();
	rope->Finalize();
}

void GameScene::Update() {

	if (nowScene == 0)
	{
		if (keyboard->TriggerKey(DIK_SPACE))
		{
			//マップチップ用のCSV読み込み
			//(map, "Resource/scv/なんたら.csv")で追加可能
			if (!mapLoadFlag)
			{
				//Mapchip::CsvToVector(map, "Resources/csv/tutorial.csv");//mapNum=0
				Mapchip::CsvToVector(map, "Resources/csv/stage1.csv");//mapNum=0
				MapCreate(0);
				mapLoadFlag = true;
			}
			if (!fadeIn)
			{
				fadeIn = true;
			}
		}

		if(fadeIn)
		{
			alpha += 0.01f;
			backGround->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, alpha });
			if (alpha >= 1.0f)
			{
				nowScene = 1;
				fadeIn = false;
				fadeOut = true;
			}
		}
	}

	if (nowScene == 1)
	{
		// タイトルから移行後の更新
		{
			CollisionUpdate();
			LightUpdate();
			CameraUpdate();
			// jsonファイルから読み込んだオブジェクトの更新
			for (auto& object : objects)
			{
				object->Update();
			}
			// マップオブジェクトの更新
			MapUpdate(0);

			player->SetPos(pPos);
			//	プレイヤーの座標、半径の設定
			player->Update(rFlag, moveFlag);
			pPosOld = pPos;
			pPos = player->GetPos();
			pRadius = player->GetObj()->GetScale();

			rPos = rope->GetObj()->GetPosition();
			rRadius = rope->GetObj()->GetScale();

			// フェードアウトが終わるまではここでリターン
			if (fadeOut)
			{
				alpha -= 0.01f;
				backGround->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, alpha });			
				if (alpha <= 0.0f)
				{
					fadeOut = false;
				}
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

			rope->Update(pPos);

			for (std::unique_ptr<Enemy>& enemy : enemys)
			{
				ePos = enemy->GetPos();
				PElength = GetLength(pPos, ePos);

				if (PElength < minLength && ePos.y > 0.0f)
				{
					ePosSave = ePos;
				}
			}

			if (throwCount == 30)
			{
				rope->Throw(pPos, ePosSave, PElength);
			}

			for (std::unique_ptr<Enemy>& enemy : enemys)
			{
				enemy->Update();
			}

			rPos = rope->GetObj()->GetPosition();
			rRadius = rope->GetObj()->GetScale();

			player->SetPos(pPos);
			mouse->Update();
		}

		if (enemyHp <= 0)
		{
			fadeIn = true;
			if (fadeIn)
			{
				alpha += 0.01f;
				backGround->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, alpha });
				if (alpha >= 1.0)
				{
					nowScene = 2;
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
				alpha += 0.01f;
				backGround->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, alpha });
				if (alpha >= 1.0f)
				{
					nowScene = 3;
					fadeIn = false;
					fadeOut = true;
				}
			}
		}
	}

	if (nowScene == 2 || nowScene == 3)
	{
		if (fadeOut)
		{
			alpha -= 0.01f;
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
			nowScene = 0;
		}
	}
}

void GameScene::reset()
{
	// プレイヤー
	pPos = { 0.0f, 10.0f, 0.0f };//座標
	player->GetObj()->SetPosition(pPos);
	pPosOld = { 0.0f, 10.0f, 0.0f };
	pRadius = {};
	onGround = false;//自由落下のフラグ
	moveFlag = false;//移動管理フラグ
	avoidFlag = false;//回避管理フラグ
	playerHp = 360;

	// エネミー
	enemyCount = 4;
	enemyHp = 360;
	ePos = {};
	ePosOld = {};
	eRadius = {};

	// ロープ
	rPos = {};
	rRadius = {};
	rFlag = false;

	// カメラ
	cPos = {};
	cTarget = {};
	cameraLength = {};

	// 突進用
	startPos = {}; // 開始位置
	endPos = {}; // 終点位置
	avoidMove = 5.0f; // 距離
	avoidTime = 0.0f; // 経過時間
	easeFlag = false; // イージング開始フラグ

	// シェイク用
	shakeFlag = false;
}

void GameScene::Draw() {

	//SetImgui();

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
	if (nowScene == 1)
	{
		player->Draw();

		for (std::unique_ptr<Enemy>& enemy : enemys)
		{
			enemy->Draw();
		}
		rope->Draw();
		MapDraw(0);

		for (auto& object : objects)
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

	if (nowScene == 0)
	{
		title->Draw();
	}

	if (nowScene == 1)
	{
		HPText->Draw();
		PlayerHPBar->Draw();
		EnemyHPBar->Draw();

		PlayerHPGauge->SetSize({ playerHp,60 });
		PlayerHPGauge->Draw();

		EnemyHPGauge->SetSize({ enemyHp,60 });
		EnemyHPGauge->Draw();
	}

	if (nowScene == 2)
	{
		result->Draw();
	}

	if (nowScene == 3)
	{
		GameOver->Draw();
	}

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
	ImGui::Text("cameraLength : %6.2f", cPos.x);
	ImGui::Text("cameraLength : %6.2f", cPos.y);
	ImGui::Text("cameraLength : %6.2f", cPos.z);

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
	if (CollisionStage(pPos, pRadius, pPosOld))
	{
		player->SetOnGround(true);
	}

	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		if (enemy->StageCollide(stagePos, stageScale))
		{
			enemy->SetOnGround(true);
		}
	}

	for (int y = 0; y < map_max_y; y++)
	{
		for (int x = 0; x < map_max_x; x++)
		{
			if (Mapchip::GetChipNum(x, y, map[0]) == blocks_ || Mapchip::GetChipNum(x, y, map[0]) == walls_)
			{
				XMFLOAT3 boxPos = blockObj[y][x]->GetPosition();
				XMFLOAT3 boxRadius = blockObj[y][x]->GetScale() * (LAND_SCALE / 2);
				player->MapCollide(boxPos, boxRadius, pPos, pRadius, 0, pPosOld);
				for (std::unique_ptr<Enemy>& enemy : enemys)
				{
					enemy->MapCollide(boxPos, boxRadius);
				}
			}
		}
	}

	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		//rPos = rope->GetObj()->GetPosition();
		//rRadius = rope->GetObj()->GetScale();
		eAlive = enemy->GetAlive();
		if (eAlive && !rFlag && enemy->RopeCollide(rPos, rRadius))
		{
			ePos = enemy->GetPos();
			rope->Collision(pPos, ePos);
			enemy->SetAttackFlag(false);
		}

		if (eAlive && Collision::CollisionObject(player->GetObj(), enemy->GetObj()))
		{
			shakeFlag = true;
			moveFlag = true;
			rope->SetmoveFlag(moveFlag);

			if (rFlag)
			{
				enemyHp -= 72;
				minLength = 15.0f;
				ePosSave = { 0.0f, 0.0f, 0.0f };
			}
			else
			{
				playerHp -= 18;
			}

			rope->SetrFlag(false);
			enemy->EnemyCollision();
		}

		if (!rFlag && enemy->BulletCollision())
		{
			shakeFlag = true;
			playerHp -= 18;
		}
	}

	camera->SetTarget(pPos);
}

bool GameScene::CollisionStage(XMFLOAT3& pos, const XMFLOAT3 radius, const XMFLOAT3 oldPos)
{
	// 判定
	float maxX = stagePos.x + stageScale.x;
	float maxY = stagePos.y + stageScale.y;
	float maxZ = stagePos.z + stageScale.z;
	float minX = stagePos.x - stageScale.x;
	float minY = stagePos.y - stageScale.y;
	float minZ = stagePos.z - stageScale.z;

	bool hitFlag = false;

	if ((pos.x < maxX && pos.x > minX) &&
		(pos.z < maxZ && pos.z > minZ))
	{
		if (maxY + radius.y > pos.y && stagePos.y < oldPos.y)
		{
			if (stagePos.y + radius.y >= pos.y)
			{
				pos.y = oldPos.y;
			}
			hitFlag = true;
		}
	}

	return hitFlag;
}

void GameScene::MapCreate(int mapNumber)
{
	for (int y = 0; y < map_max_y; y++) {//(yが25)
		for (int x = 0; x < map_max_x; x++) {//(xが25)

			if (Mapchip::GetChipNum(x, y, map[mapNumber]) == blocks_)
			{
				blockObj[y][x] = Object3d::Create();
				blockObj[y][x]->SetModel(blockModel);
				//位置と大きさの変更(今は大きさは変更しないで)
				blockObj[y][x]->SetPosition({ ((float)x - ((float)map_max_x / 2)) * LAND_SCALE, 2.5f , ((float)y - ((float)map_max_y / 2)) * LAND_SCALE });
				blockObj[y][x]->SetScale({ 1.0f,1.0f,1.0f });
			}

			if (Mapchip::GetChipNum(x, y, map[mapNumber]) == walls_)
			{
				blockObj[y][x] = Object3d::Create();
				blockObj[y][x]->SetModel(blockModel);
				//位置と大きさの変更(今は大きさは変更しないで)
				blockObj[y][x]->SetPosition({ ((float)x - ((float)map_max_x / 2)) * LAND_SCALE, 5.0f , ((float)y - ((float)map_max_y / 2)) * LAND_SCALE });
				blockObj[y][x]->SetScale({ 1.0f,2.0f,1.0f });
			}

		}
	}
}

void GameScene::MapUpdate(int mapNumber)
{
	for (int y = 0; y < map_max_y; y++)
	{
		for (int x = 0; x < map_max_x; x++)
		{
			if (Mapchip::GetChipNum(x, y, map[mapNumber]) == blocks_ || Mapchip::GetChipNum(x, y, map[mapNumber]) == walls_)
			{
				blockObj[y][x]->Update();
			}
		}
	}
}

void GameScene::MapDraw(int mapNumber)
{
	//マップチップの描画
	for (int y = 0; y < map_max_y; y++)
	{
		for (int x = 0; x < map_max_x; x++)
		{
			if (Mapchip::GetChipNum(x, y, map[mapNumber]) == blocks_ || Mapchip::GetChipNum(x, y, map[mapNumber]) == walls_)
			{
				blockObj[y][x]->Draw();
			}
		}
	}
}

bool GameScene::MapCollide(XMFLOAT3& pos, XMFLOAT3 radius, int mapNumber, const XMFLOAT3 oldPos)
{
	//マップチップ
	//X, Y
	XMFLOAT3 mapPos = {};
	//Radius
	XMFLOAT3 mapRadius = {};

	//フラグ
	bool hitFlag = false;

	for (int y = 0; y < map_max_y; y++)
	{
		for (int x = 0; x < map_max_x; x++)
		{
			if (Mapchip::GetChipNum(x, y, map[mapNumber]) == blocks_ || Mapchip::GetChipNum(x, y, map[mapNumber]) == walls_)
			{
				mapPos = blockObj[y][x]->GetPosition();
				mapRadius = blockObj[y][x]->GetScale() * (LAND_SCALE / 2);

				// 判定
				float maxMapX = mapPos.x + mapRadius.x;
				float minMapX = mapPos.x - mapRadius.x;
				float maxMapY = mapPos.y + mapRadius.y;
				float minMapY = mapPos.y - mapRadius.y;
				float maxMapZ = mapPos.z + mapRadius.z;
				float minMapZ = mapPos.z - mapRadius.z;

				if ((pos.x <= maxMapX && pos.x >= minMapX) &&
					(pos.z <= maxMapZ && pos.z >= minMapZ))
				{
					if (maxMapY + radius.y > pos.y && mapPos.y < oldPos.y)
					{
						pos.y = maxMapY + radius.y;
						hitFlag = true;
					}
					else if (minMapY - radius.y < pos.y && mapPos.y > oldPos.y)
					{
						pos.y = minMapY - radius.y;
						hitFlag = true;
					}
				}

				if ((pos.x <= maxMapX && pos.x >= minMapX) &&
					(pos.y <= maxMapY && pos.y >= minMapY))
				{
					if (maxMapZ + radius.z > pos.z && mapPos.z < oldPos.z)
					{
						pos.z = maxMapZ + radius.z;
						hitFlag = true;
					}
					else if (minMapZ - radius.z < pos.z && mapPos.z > oldPos.z)
					{
						pos.z = minMapZ - radius.z;
						hitFlag = true;
					}
				}

				if ((pos.z <= maxMapZ && pos.z >= minMapZ) && 
					(pos.y <= maxMapY && pos.y >= minMapY))
				{
					if (maxMapX + radius.x > pos.x && mapPos.x < oldPos.x)
					{
						pos.x = maxMapX + radius.x;
						hitFlag = true;
					}
					else if (minMapX - radius.x < pos.x && mapPos.x > oldPos.x)
					{
						pos.x = minMapX - radius.x;
						hitFlag = true;
					}
				}


			}
		}
	}

	return hitFlag;
}

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
Collision::Sphere GameScene::sphere[map_max_y][map_max_x];

bool GameScene::hitFlag = false;
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
	bullet = new Bullet;
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

	//item->Initialize();
	rope->Initialize(keyboard, mouse);
	player->Initialize(keyboard, mouse);
	bullet->Initialize();
	enemy->Initialize(player, bullet);


	// 3Dオブジェクト生成
	//skydome = Object3d::Create();
	//stage = Object3d::Create();

	// 3Dオブジェクトにモデルを割り当てる
	//skydome->SetModel(skydomeModel);
	//stage->SetModel(stageModel);

	// 各オブジェクトの位置や大きさを初期化
	//stage->SetPosition({ 0.0f, -1.0f, 0.0f });
	//stage->SetScale({ 60.0f, 1.0f, 60.0f });
	//skydome->SetPosition({ 0.0f, -30.0f, 0.0f });
	//skydome->SetScale({ 5.0f, 5.0f, 5.0f });

	// オブジェクト生成
	blockModel = blockModel->CreateFromObject("block");
	//マップチップ用のCSV読み込み
	//(map, "Resource/scv/なんたら.csv")で追加可能
	Mapchip::CsvToVector(map, "Resources/csv/tutorial.csv");//mapNum=0

	//マップチップ用のオブジェクトの初期化
	for (int y = 0; y < map_max_y; y++)
	{
		for (int x = 0; x < map_max_x; x++)
		{
			if (Mapchip::GetChipNum(x, y, map[0]) == block)
			{
				blockObj[y][x] = Object3d::Create();
				blockObj[y][x]->SetModel(blockModel);
				blockObj[y][x]->SetScale({ 1.0f,1.0f,1.0f });
				blockObj[y][x]->SetPosition({ 1000.0f,1000.0f,0.0f });
			}
		}
	}

	// レベルデータの読み込み
	levelData = LevelLoader::LoadFile("test");

	// .objの名前を指定してモデルを読み込む
	cubeModel = cubeModel->CreateFromObject("cube");
	sphereModel = sphereModel->CreateFromObject("sphere");
	stageModel = stageModel->CreateFromObject("stage");
	skydomeModel = skydomeModel->CreateFromObject("skydome");
	models.insert(std::make_pair("cube", cubeModel));
	models.insert(std::make_pair("sphere", sphereModel));
	models.insert(std::make_pair("stage", stageModel));
	models.insert(std::make_pair("skydome", skydomeModel));
	models.insert(std::make_pair("block", blockModel));

	// レベルデータからオブジェクトを生成、配置
	for (LevelData::ObjectData& objectData : levelData->objects) 
	{
		// 3Dオブジェクトを生成
		Object3d* newObject = Object3d::arrayCreate();
		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
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

		// 配列に登録
		objects.push_back(newObject);
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
	enemy->Finalize();
	rope->Finalize();
	bullet->Finalize();
}

void GameScene::Update() {

	if (nowScene == 0)
	{
		MapCreate(0);
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

		rFlag = rope->GetrFlag();
		moveFlag = rope->GetmoveFlag();
		avoidFlag = player->GetAvoidFlag();
		eAlive = enemy->GetAlive();
		attackFlag = bullet->GetAttackFlag();

		player->Update(rFlag, moveFlag);
		enemy->Update();

		pPos = player->GetPos();
		pPosOld = pPos;
		pRadius = player->GetScale();
		MapUpdate(0);
		if (MapCollide(pPos, pRadius, 0, pPosOld))
		{
			DebugText::GetInstance()->Print(50, 30 * 10, 2, "ObjectHit");
		}

		for (auto& object : objects)
		{
			object->Update();
		}

		ePos = enemy->GetPos();
		bPos = bullet->GetPos();

		length = GetLength(pPos, ePos);

		rope->Update(pPos, ePos, enemy->GetObj());
		//item->Update();

		player->SetPos(pPos);
		enemy->SetPos(ePos);
		mouse->Update();

		if (enemyHp == 0)
		{
			nowScene = 2;
		}

		if (playerHp == 0)
		{
			nowScene = 3;
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
		enemy->Draw();
	}
	if (attackFlag)
	{
		bullet->Draw();
	}
	rope->Draw();
	//item->Draw();
	MapDraw(0);

	for (auto& object : objects)
	{
		object->Draw();
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
		moveFlag = true;
		attackFlag = false;
		bullet->SetAttackFlag(attackFlag);
		rope->SetmoveFlag(moveFlag);
		enemy->SetAlive(eAlive);
		if (rFlag)
		{
			enemyHp -= 72;
		}
		else
		{
			playerHp -= 72;
		}

		rFlag = false;
		rope->SetrFlag(rFlag);
	}
	if (Collision::CollisionObject(bullet->GetObj(), player->GetObj()))
	{
		bPos = ePos;
		bullet->SetPos(bPos);
		bullet->Collision();
		if (enemy->GetAlive())
		{
			playerHp -= 72;
		}
	}

	player->SetPos(pPos);
	camera->SetTarget(pPos);
}

void GameScene::MapCreate(int mapNumber)
{
	for (int y = 0; y < map_max_y; y++) {//(yが25)
		for (int x = 0; x < map_max_x; x++) {//(xが25)

			if (Mapchip::GetChipNum(x, y, map[mapNumber]) == block)
			{
				//位置と大きさの変更(今は大きさは変更しないで)
				blockObj[y][x]->SetPosition({ ((float)x - ((float)map_max_x / 2)) * LAND_SCALE, (float)mapNumber * LAND_SCALE , ((float)y - ((float)map_max_y / 2)) * LAND_SCALE });
				sphere[y][x].center = XMVectorSet(((float)x - ((float)map_max_x / 2)) * LAND_SCALE, (float)mapNumber * LAND_SCALE, ((float)y - ((float)map_max_y / 2)) * LAND_SCALE, 0.0f);
				sphere[y][x].radius = blockObj[y][x]->GetScale().x;
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
			if (Mapchip::GetChipNum(x, y, map[mapNumber]) == block)
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
			if (Mapchip::GetChipNum(x, y, map[mapNumber]) == block)
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
			if (Mapchip::GetChipNum(x, y, map[mapNumber]) == block)
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

				//オブジェクト同士の距離
				float l_x = sqrtf(powf(mapPos.x - pos.x, 2));
				float l_y = sqrtf(powf(mapPos.y - pos.y, 2));
				float l_z = sqrtf(powf(mapPos.z - pos.z, 2));
				// オブジェクト同士の半径
				float r_x = sqrtf(powf(radius.x + mapRadius.x, 2));
				float r_y = sqrtf(powf(radius.y + mapRadius.y, 2));
				float r_z = sqrtf(powf(radius.z + mapRadius.z, 2));

				if (pos.x <= maxMapX && pos.x >= minMapX)
				{
					if (maxMapZ + radius.z > pos.z && mapPos.z < oldPos.z)
					{
						pos.z = maxMapZ + radius.z;
						hitFlag = true;
						avoidFlag = false;
						player->SetAvoidFlag(avoidFlag);
						height = y;
						width = x;
					}
					else if (minMapZ - radius.z < pos.z && mapPos.z > oldPos.z)
					{
						pos.z = minMapZ - radius.z;
						hitFlag = true;
						avoidFlag = false;
						player->SetAvoidFlag(avoidFlag);
						height = y;
						width = x;
					}
				}

				if (pos.z <= maxMapZ && pos.z >= minMapZ)
				{
					if (maxMapX + radius.x > pos.x && mapPos.x < oldPos.x)
					{
						pos.x = maxMapX + radius.x;
						hitFlag = true;
						avoidFlag = false;
						player->SetAvoidFlag(avoidFlag);
						height = y;
						width = x;
					}
					else if (minMapX - radius.x < pos.x && mapPos.x > oldPos.x)
					{
						pos.x = minMapX - radius.x;
						hitFlag = true;
						avoidFlag = false;
						player->SetAvoidFlag(avoidFlag);
						height = y;
						width = x;
					}
				}
			}
		}
	}
	return hitFlag;
}

int GameScene::GetLeftMapChip(XMFLOAT3 position)
{
	int chip = Mapchip::GetChipNum(static_cast<int>((position.x + LAND_SCALE / 2) / LAND_SCALE - 1), -static_cast<int>((position.y - LAND_SCALE / 2) / LAND_SCALE), map[0]);
	return chip;
}

int GameScene::GetRightMapChip(XMFLOAT3 position)
{
	int chip = Mapchip::GetChipNum(static_cast<int>((position.x + LAND_SCALE / 2) / LAND_SCALE + 1), -static_cast<int>((position.y - LAND_SCALE / 2) / LAND_SCALE), map[0]);
	return chip;
}

int GameScene::GetUpMapChip(XMFLOAT3 position)
{
	int chip = Mapchip::GetChipNum(static_cast<int>((position.x + LAND_SCALE / 2) / LAND_SCALE), -static_cast<int>((position.y - LAND_SCALE / 2) / LAND_SCALE + 1), map[0]);
	return chip;
}

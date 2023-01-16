#pragma once

#include "DirectXCommon.h"
#include "Keyboard.h"
#include "Controller.h"
#include "Mouse.h"
#include "Image2d.h"
#include "DebugText.h"
#include "Object3d.h"
#include "Model.h"
#include "FbxLoader.h"
#include "FbxObject3d.h"
#include "Operator.h"

#include "Camera.h"
#include "Audio.h"
#include "Rope.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Item.h"
#include "Collision.h"
#include "Light.h"
#include "Easing.h"
#include "Mapchip.h"
#include "LevelLoader.h"

#include <SafeDelete.h>
#include <stdlib.h>
#include <time.h>

class GameScene {
private: // 静的メンバ変数
	static const int debugTextTexNumber = 0;

public: // メンバ関数

	// コンストクラタ
	GameScene();
	// デストラクタ
	~GameScene();
	// 初期化
	void Initialize(DirectXCommon* dxCommon, Controller* controller, Mouse* mouse, Audio* audio);
	// 解放
	void Finalize();
	// 毎フレーム処理
	void Update();
	//pos初期化
	void reset();
	// 描画
	void Draw();
	// Imguiの設定
	void SetImgui();
	// ライトの更新
	void LightUpdate();
	// カメラの更新
	void CameraUpdate();
	// 当たり判定
	void CollisionUpdate();

	// マップチップ用
	// マップチップ生成
	static void MapCreate(int mapNumber);
	// マップチップ更新
	static void MapUpdate(int mapNumber);
	// マップチップ描画
	static void MapDraw(int mapNumber);

	float GetLength(XMFLOAT3 posA, XMFLOAT3 posB)
	{
		XMFLOAT3 len = { posA.x - posB.x, posA.y - posB.y, posA.z - posB.z };
		return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
	}

private: // メンバ変数
	DirectXCommon* dxCommon = nullptr;
	Keyboard* keyboard = Keyboard::GetInstance();
	Controller* controller = nullptr;
	Mouse* mouse = nullptr;
	Audio* audio = nullptr;
	Camera* camera = Camera::GetInstance();
	Collision* collision = nullptr;
	Light* light = nullptr;
	Easing* easing = nullptr;
	Mapchip* mapchip = nullptr;

	Rope* rope = nullptr;
	Player* player = nullptr;
	Enemy* enemy = nullptr;
	// 複数生成用リスト
	std::list<std::unique_ptr<Enemy>> enemys;
	Bullet* bullet = nullptr;
	Item* item = Item::GetInstance();
	LevelData* levelData = nullptr;

	// マップ
	//マップチップ1つの大きさ
	static const float LAND_SCALE;
	//マップチップの番号
	enum MapNumber
	{
		None, blocks_, walls_
	};

	//mapchipオブジェクト
	static Model* blockModel;
	static std::vector<std::vector<int>> map; //マップチップ
	static std::unique_ptr<Object3d> blockObj[map_max_y][map_max_x]; // ステージブロック
	bool mapLoadFlag = false;
	// jsonオブジェクト
	std::vector<std::unique_ptr<Object3d>> objects{};
	XMFLOAT3 stagePos = {};
	XMFLOAT3 stageScale = {};

	// マップ番号
	static int height;
	static int width;

	// 画像
	Image2d* title = nullptr;
	Image2d* result = nullptr;
	Image2d* GameOver = nullptr;
	Image2d* HPText = nullptr;
	Image2d* PlayerHPBar = nullptr;
	Image2d* PlayerHPGauge = nullptr;
	Image2d* EnemyHPBar = nullptr;
	Image2d* EnemyHPGauge = nullptr;
	Image2d* backGround = nullptr;
	float alpha = 0.0f;
	Image2d* explanation = nullptr;

	// プレイヤー
	XMFLOAT3 pPos = {};//座標
	XMFLOAT3 pPosOld = {};
	XMFLOAT3 pRadius = {};
	bool onGround = false;//自由落下のフラグ
	bool moveFlag = false;//移動管理フラグ
	bool avoidFlag = false;//回避管理フラグ
	float playerHp = 360;

	// エネミー
	static const int enemyCount = 5;
	float enemyHp = 360;
	bool eAlive = false;
	XMFLOAT3 ePos = {};
	XMFLOAT3 ePosOld = {};
	XMFLOAT3 eRadius = {};
	XMFLOAT3 ePosSave = {};
	float minLength = 10.0f;

	// ロープ
	bool rFlag = false;
	int throwCount = 0;
	float PElength = 0.0f;

	// カメラ
	XMFLOAT3 cPos = {};
	XMFLOAT3 cTarget = {};
	XMVECTOR cameraLength = {};

	// シーン管理用
	int nowScene = 0;
	bool fadeIn = false;
	bool fadeOut = false;
	bool expFlag = false;

	// シェイク用
	bool shakeFlag = false;
};
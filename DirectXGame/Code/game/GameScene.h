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
	// ステージの当たり判定
	bool CollisionStage(XMFLOAT3& pos, const XMFLOAT3 radius, const XMFLOAT3 oldPos);

	// マップチップ用
	// マップチップ生成
	static void MapCreate(int mapNumber);
	// マップチップ更新
	static void MapUpdate(int mapNumber);
	// マップチップ描画
	static void MapDraw(int mapNumber);
	// マップチップ当たり判定
	bool MapCollide(XMFLOAT3& pos, XMFLOAT3 radius, int mapNumber, const XMFLOAT3 oldPos);

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
	Bullet* bullet = nullptr;
	Item* item = Item::GetInstance();
	LevelData* levelData = nullptr;

	// ゲームシーン用
	// 3dモデル
	//std::unique_ptr<Object3d> skydome = nullptr;
	//std::unique_ptr<Object3d> stage = nullptr;

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

	// プレイヤー
	XMFLOAT3 pPos = {};//座標
	XMFLOAT3 pPosOld = {};
	XMFLOAT3 pRadius = {};
	bool onGround = false;//自由落下のフラグ
	bool moveFlag = false;//移動管理フラグ
	bool avoidFlag = false;//回避管理フラグ
	float playerHp = 360;

	// エネミー
	XMFLOAT3 ePos = {};
	XMFLOAT3 ePosOld = {};
	XMFLOAT3 eRadius = {};
	bool eFlag = false; // 自由落下のフラグ
	bool eAlive = false;// 生きているかのフラグ
	float enemyHp = 360;

	// バレット
	XMFLOAT3 bPos = {};
	bool attackFlag = false;

	// ロープ
	XMFLOAT3 rPos = {};
	bool rFlag = false;

	// カメラ
	XMFLOAT3 cPos = {};
	XMFLOAT3 cTarget = {};
	XMVECTOR cameraLength = {};

	// 突進用
	XMFLOAT3 startPos = {}; // 開始位置
	XMFLOAT3 endPos = {}; // 終点位置
	float avoidMove = 5.0f; // 距離
	float avoidTime = 0.0f; // 経過時間
	bool easeFlag = false; // イージング開始フラグ

	// 当たり判定
	int cCount = 0;
	bool cFlag = false;

	// シーン管理用
	int nowScene = 0;

	// シェイク用
	bool shakeFlag = false;
	XMFLOAT2 shakeXY = {};
	int shakeTime = 0;

	float length = 0.0f;

	bool poseFlag = false;
};
#pragma once

#include "DirectXCommon.h"
#include "Keyboard.h"
#include "Controller.h"
#include "Mouse.h"
#include "Image2d.h"
#include "Object3d.h"
#include "DebugText.h"
#include "Audio.h"
#include "Model.h"
#include "FbxLoader.h"
#include "FbxObject3d.h"
#include "Camera.h"
#include "Easing.h"
#include "Collision.h"
#include "Light.h"
#include "Rope.h"
#include "Player.h"
#include "Enemy.h"

#include <SafeDelete.h>
#include <DirectXMath.h>
#include <stdlib.h>
#include <time.h>

using namespace DirectX;

class GameScene {
private: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;

private: // 静的メンバ変数
	static const int debugTextTexNumber = 0;

public: // メンバ関数
	// コンストクラタ
	GameScene();
	// デストラクタ
	~GameScene();
	// 初期化
	void Initialize(DirectXCommon* dxCommon, Keyboard* keyboard, Controller* controller, Mouse* mouse, Audio* audio);
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

private: // メンバ変数
	DirectXCommon* dxCommon = nullptr;
	Keyboard* keyboard = nullptr;
	Controller* controller = nullptr;
	Mouse* mouse = nullptr;
	Audio* audio = nullptr;
	Camera* camera = nullptr;
	Collision* collision = nullptr;
	Light* light = nullptr;
	Easing* easing = nullptr;
	Operator* ope = nullptr;

	// ゲームシーン用
	// 3dモデル
	Model* skydomeModel = nullptr;
	std::unique_ptr<Object3d> skydome = nullptr;
	Model* stageModel = nullptr;
	std::unique_ptr<Object3d> stage = nullptr;

	// 画像
	Image2d* title = nullptr;
	Image2d* result = nullptr;

	// プレイヤー
	XMFLOAT3 pPos = {};//座標
	bool jumpFlag = false;//自由落下のフラグ
	bool moveFlag = false;//移動管理フラグ

	// エネミー
	XMFLOAT3 ePos = {};
	bool eFlag = false; // 自由落下のフラグ
	bool eAlive = false;// 生きているかのフラグ
	int enemyCount = 0; // 倒した数

	// ロープ管理フラグ
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
};
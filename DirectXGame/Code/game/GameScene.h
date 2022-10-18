#pragma once

#include "SafeDelete.h"
#include "DirectXCommon.h"
#include "Keyboard.h"
#include "Controller.h"
#include "Mouse.h"
#include "Image2d.h"
#include "Object3d.h"
#include "DebugText.h"
#include "Audio.h"
#include "Model.h"
#include "stdlib.h"
#include "time.h"
#include "FbxLoader.h"
#include "FbxObject3d.h"
#include "Camera.h"
#include "Easing.h"
#include "Collision.h"
#include "Light.h"

#include <DirectXMath.h>
#include <Collision.h>

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

	// 自機の移動
	void PlayerUpdate();
	// 自機の突進
	void PlayerRush();
	// 敵の移動
	void EnemyUpdate();
	// ロープの更新
	void RopeUpdate();
	// ロープを投げる
	void RopeThrow(XMFLOAT3& rPos, XMFLOAT3& rScale, bool& flag);
	// ライトの更新
	void LightUpdate();
	// カメラの更新
	void CameraUpdate();
	// イージングの更新
	void EaseUpdate(const XMFLOAT3 startPos, const XMFLOAT3 endPos, XMFLOAT3& reflectPos, bool& flag);
	// 当たり判定
	void CollisionUpdate();
	// レイの当たり判定
	void CollisionRay();

	// オブジェクト同士の距離を取得
	float GetLength(XMFLOAT3 posA, XMFLOAT3 posB);
	// ロープの角度を変更
	float PosForAngle(float startPos1, float startPos2, float endPos1, float endPos2);
	// 円運動
	void CircularMotion(XMFLOAT3& pos, const XMFLOAT3 centerPos, const float r, int& angle, const int add);

private: // メンバ変数
	DirectXCommon* dxCommon = nullptr;
	Keyboard* keyboard = nullptr;
	Controller* controller = nullptr;
	Mouse* mouse = nullptr;
	Audio* audio = nullptr;
	DebugText debugText;
	Camera* camera = nullptr;
	Collision* collision = nullptr;
	Light* light = nullptr;
	Easing* easing = nullptr;
	Operator* ope = nullptr;

	Collision::Ray ray;
	Collision::Plane plane;

	// ゲームシーン用
	Model* playerModel = nullptr;
	std::unique_ptr<Object3d> player = nullptr;
	Model* enemyModel = nullptr;
	std::unique_ptr<Object3d> enemy = nullptr;
	Model* skydomeModel = nullptr;
	std::unique_ptr<Object3d> skydome = nullptr;
	Model* stageModel = nullptr;
	std::unique_ptr<Object3d> stage = nullptr;
	Model* ropeModel = nullptr;
	std::unique_ptr<Object3d> rope = nullptr;

	// プレイヤー
	XMFLOAT3 pPos = {};//座標
	bool pFlag = false;//自由落下のフラグ
	float pMove = 0.0f;//移動量
	float pAcc = 0.2f;//加速
	float pVal = 0.2f;//速度
	float pGra = 0.1f;//重力
	float rate = 1.0f; // 斜め移動時の制限

	// エネミー
	XMFLOAT3 ePos = {};
	bool eFlag = false;//自由落下のフラグ
	float eVal = 0.2f;//速度
	float eGra = 0.1f;//重力

	// ロープ
	XMFLOAT3 rPos = {};
	XMFLOAT3 rScale = {};
	float angleX = 0.0f; // X軸
	float angleY = 0.0f; // Y軸
	float vecXZ = 0.0f; // XZ平面上のベクトル
	const float maxRope = 10.0f; // ロープの最大
	bool rFlag = false; // 接触フラグ

	bool rThrowFlag = false; // ロープを飛ばす
	bool rBackFlag = false; // ロープを戻す
	XMFLOAT3 manageRopePos = {}; // ロープ位置管理用
	XMFLOAT3 manageRopeScale = {}; // ロープスケール管理用

	// カメラ
	XMFLOAT3 cPos = {};
	XMVECTOR cameraLength = {};

	// 突進用
	XMFLOAT3 startPos = {}; // 開始位置
	XMFLOAT3 endPos = {}; // 終点位置
	XMFLOAT3 reflectPos = {};
	float avoidMove = 5.0f; // 距離
	float avoidTime = 0.0f; // 経過時間
	const float avoidEndTime = 5.0f; // かかる時間
	float avoidTimeRate = 0.0f;
	bool easeFlag = false; // イージング開始フラグ
	bool pEaseFlag = false;
	bool eEaseFlag = false;

	// 当たり判定
	int cCount = 0;
	bool cFlag = false;

};
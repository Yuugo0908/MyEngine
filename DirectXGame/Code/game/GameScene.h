#pragma once

#include "SafeDelete.h"
#include "DirectXCommon.h"
#include <DirectXMath.h>
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
	// ライトの更新
	void LightUpdate();
	// カメラの更新
	void CameraUpdate();

	// オブジェクト同士の距離を取得
	float GetLength(XMFLOAT3 pos_a, XMFLOAT3 pos_b);
	// ロープの角度を変更
	float PosForAngle(float startPos1, float startPos2, float endPos1, float endPos2);
	// 円運動
	void CircularMotion(XMFLOAT3& pos, const XMFLOAT3 center_pos, const float r, int& angle, const int add);
	// 正規化
	XMFLOAT3 normalize(XMFLOAT3 p1, XMFLOAT3 p2);

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
	XMFLOAT3 p_pos = {};//座標
	bool p_flag = false;//自由落下のフラグ
	float p_move = 0.0f;
	float p_val = 0.2f;//速度
	float p_gra = 0.1f;//重力

	// エネミー
	XMFLOAT3 e_pos = {};
	bool e_flag = false;//自由落下のフラグ
	float e_val = 0.2f;//速度
	float e_gra = 0.1f;//重力

	// ロープ
	XMFLOAT3 r_pos = {};
	float angleX; // X軸
	float angleY; // Y軸
	float vecXZ; // XZ平面上のベクトル
	const float max_rope = 10.0f; // ロープの最大
	bool r_flag = false; // 接触フラグ

	// カメラ
	XMFLOAT3 c_pos = {};


	// 突進用
	XMFLOAT3 startPos = {};
	XMFLOAT3 endPos = {};
	float avoidMove = 10.0f;
	float avoidTime = 0.0f;
	const float avoidEndTime = 5.0f;
	float avoidTimeRate = 0.0f;
	bool easeFlag = false;
};
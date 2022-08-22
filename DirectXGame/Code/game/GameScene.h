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
protected: // エイリアス
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
	void Initialize(DirectXCommon* dxCommon, Keyboard* keyboard, Audio* audio);
	// 解放
	void Finalize();
	// 毎フレーム処理
	void Update();
	//pos初期化
	void reset();
	// 描画
	void Draw();

	// 自機の移動
	void PlayerMove();
	// 敵の移動
	void EnemyMove();
	// Imguiの設定
	void SetImgui();
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

private: // メンバ変数
	DirectXCommon* dxCommon = nullptr;
	Keyboard* keyboard = nullptr;
	Controller* controller = nullptr;
	Mouse* mouse = nullptr;
	Audio* playAudio = nullptr;
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

	XMFLOAT3 p_pos = {};
	XMFLOAT3 e_pos = {};
	XMFLOAT3 r_pos = {};

	bool p_flag = false;//自由落下のフラグ
	float p_val = 0.2f;//速度

	float gra = 0.1f;//重力
	float angleX;
	float angleY;
	float vecXZ;
	float max_rope = 15.0f;
	bool r_flag = false;

	// カメラ
	XMFLOAT3 c_pos = {};
	// カメラ注視点までの距離
	float distance = 20;
	// スケーリング
	const float scaleX = 1.0f / WinApp::window_width;
	const float scaleY = 1.0f / WinApp::window_height;
	// 回転行列
	XMMATRIX matRot = DirectX::XMMatrixIdentity();

	// 突進用
	XMFLOAT3 startPos = {};
	XMFLOAT3 endPos = {};
	float avoidMove = 10.0f;
	float avoidTime = 0.0f;
	const float avoidEndTime = 5.0f;
	float avoidTimeRate = 0.0f;
	bool easeFlag = false;
};
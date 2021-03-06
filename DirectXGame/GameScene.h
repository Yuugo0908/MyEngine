#pragma once

#include "SafeDelete.h"
#include "DirectXCommon.h"
#include <DirectXMath.h>
#include "Keyboard.h"
#include "Controller.h"
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
	void Move();
	// Imguiの設定
	void SetImgui();
	// ロープのセット
	void RopeMove(XMFLOAT3& pos);
	// ライトの更新
	void LightUpdate();

private: // メンバ変数
	DirectXCommon* dxCommon = nullptr;
	Keyboard* keyboard = nullptr;
	Controller* controller = nullptr;
	Audio* playAudio = nullptr;
	DebugText debugText;
	Camera* camera = nullptr;
	Collision* collision = nullptr;
	Light* light = nullptr;

	// ゲームシーン用
	Image2d* title = nullptr;
	Image2d* GameClear = nullptr;
	Image2d* GameOver = nullptr;
	Image2d* backGround = nullptr;

	Model* playerModel = nullptr;
	std::unique_ptr<Object3d> player = nullptr;
	Model* enemyModel = nullptr;
	std::unique_ptr<Object3d> enemy = nullptr;

	Model* skydomeModel = nullptr;
	std::unique_ptr<Object3d> skydomeObj = nullptr;

	FbxModel* fbxModel = nullptr;
	FbxObject3d* fbxObject = nullptr;

	XMFLOAT3 p_pos = {};
	XMFLOAT3 p_sca = {};
	XMFLOAT3 e_pos = {};
	XMFLOAT3 e_sca = {};

	bool p_flag = false;//自由落下のフラグ
	float p_val = 0.0f;//速度
	float p_gra = 0.1f;//重力
};
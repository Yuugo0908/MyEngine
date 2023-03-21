#pragma once
#include "DirectXCommon.h"
#include "Audio.h"
#include "GameScene.h"
#include "Controller.h"
#include "Light.h"
#include "Mouse.h"

class Framework
{
public: // メンバ関数

	virtual ~Framework() = default;

	// 初期化
	virtual void Initialize();

	// 終了
	virtual void Finalize();

	// 毎フレーム更新
	virtual void Update();

	// 描画
	virtual void Draw();

	// 終了チェック
	virtual bool IsEndRequest() { return endRequest_; }

	// 実行
	void Run();

private: // メンバ変数
	bool endRequest_ = false;
	//ポインタ置き場
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	Keyboard* keyboard = nullptr;
	Mouse* mouse = nullptr;
	Controller* controller = nullptr;
	Audio* audio = nullptr;
	GameScene* gameScene = nullptr;
	Camera* camera = nullptr;
	Image2d* image2d = nullptr;
	Light* light = nullptr;
};


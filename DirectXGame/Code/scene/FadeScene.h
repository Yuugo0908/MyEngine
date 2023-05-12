#pragma once
#include "DirectXCommon.h"
#include "Image2d.h"
#include "Object3d.h"
#include "BaseScene.h"
#include "SafeDelete.h"
#include "Easing.h"
#include "LevelLoader.h"

enum FadeState
{
	None,
	FadeInPlay,
	FadeInEnd,
	FadeOutPlay,
	FadeOutEnd,
};

class FadeScene
{
private:

	FadeScene();

	~FadeScene();

public:
	// コピーコンストラクタを無効化
	FadeScene(const FadeScene& obj) = delete;
	// 代入演算子を無効化
	FadeScene& operator=(const FadeScene& obj) = delete;

public: // 静的メンバ関数
	static FadeScene* GetInstance();

public: // メンバ関数
	// 初期化
	void Initialize();
	// 終了
	void Finalize();
	// 毎フレーム処理
	void Update();
	// 描画
	void Draw();

	// jsonオブジェクトの初期化
	void jsonObjectInit(const std::string sceneName);

	// フェードイン
	void FadeIn(float alpha);
	// フェードアウト
	void FadeOut(float alpha);

	void reset();

	// フェードフラグのゲッター
	const bool& GetFadeInEnd() { return fadeInEnd; }
	const bool& GetFadeOutEnd() { return fadeOutEnd; }

private: // メンバ変数

	Camera* camera = Camera::GetInstance();
	Light* light = nullptr;

	// レベルデータ
	LevelData* levelData = nullptr;
	// レベルデータで読み込んだオブジェクト
	std::vector<std::unique_ptr<Object3d>> jsonObject{};

	enum Image2dNum
	{
		fadeNum = 10,
	};

	FadeState fadeState;
	Image2d* fadeTex = nullptr;

	float alpha = 0.0f;

	bool fadeInEnd = false;
	bool fadeOutEnd = false;
};


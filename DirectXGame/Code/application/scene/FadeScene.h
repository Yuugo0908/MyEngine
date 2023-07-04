#pragma once
#include "DirectXCommon.h"
#include "Image2d.h"
#include "Object3d.h"
#include "BaseScene.h"
#include "SafeDelete.h"
#include "Easing.h"

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

public: // 静的メンバ変数
	static bool fadeInEnd;
	static bool fadeOutEnd;

public: // メンバ関数
	// 初期化
	void Initialize();
	// 毎フレーム処理
	void Update();
	// 描画
	void Draw();
	// フェードイン
	void FadeIn(float alpha);
	// フェードアウト
	void FadeOut(float alpha);

	void reset();

private: // メンバ変数

	Image2d* fadeImg = nullptr;
	std::vector<Image2d*> fade;

	FadeState fadeState{};

	float alpha = 0.0f;
	float alphaAdd = 0.0f;
	int fadeNum = 16;
	int fadeCount = 0;
};
#pragma once
#include "Object3d.h"
#include "Keyboard.h"
#include "Easing.h"
#include "Collision.h"
#include "Operator.h"
#include "Player.h"
#include <DirectXMath.h>

class Enemy
{
private: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;

public: // メンバ関数

	bool Initialize();

	void Finalize();

	void Update();

	void Draw();

	const std::unique_ptr<Object3d>& GetObj() { return enemyObj; }

private: // メンバ変数

	Player* player = nullptr;

	// モデル
	Model* enemyModel = nullptr;
	std::unique_ptr<Object3d> enemyObj = nullptr;

	// エネミー
	XMFLOAT3 ePos = {};
	bool eFlag = false; // 自由落下のフラグ
	float eVal = 0.2f; // 速度
	float eGra = 0.1f; // 重力
	bool eAlive = false;// 生きているかのフラグ
	int eAliveCount = 0;
	int enemyCount = 0; // 倒した数

	// 回避管理フラグ
	bool easeFlag = false;
};


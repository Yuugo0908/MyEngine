#pragma once
#include "Object3d.h"
#include "Keyboard.h"
#include "Easing.h"
#include "Collision.h"
#include "Operator.h"
#include "Rope.h"
#include <DirectXMath.h>

class Player
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

	bool Initialize(Keyboard* keyboard);

	void Finalize() { delete playerModel; }

	void Update(bool rFlag, bool moveFlag);

	void Draw() { playerObj->Draw(); }

	void Rush(bool rFlag);

	void Jump();
	// オブジェクト
	const std::unique_ptr<Object3d>& GetObj() { return playerObj; }
	// 突進
	const bool& GetEaseFlag() { return easeFlag; }

	// 位置
	const XMFLOAT3& GetPos() { return pPos; }
	void SetPos(XMFLOAT3 pPos) { this->pPos = pPos; }

private: // メンバ変数

	Keyboard* keyboard = nullptr;
	Easing* easing = nullptr;
	Rope* rope = nullptr;

	// モデル
	Model* playerModel = nullptr;
	std::unique_ptr<Object3d> playerObj = nullptr;

	// プレイヤー
	XMFLOAT3 pPos = {};//座標
	bool jumpFlag = false;//自由落下のフラグ
	bool moveFlag = false;//移動管理フラグ
	float pMove = 0.0f;//移動量
	float pAcc = 0.2f;//加速
	float pVal = 0.2f;//速度
	const float pGra = 0.1f;//重力
	float rate = 1.0f; // 斜め移動時の制限

	// 突進用
	XMFLOAT3 startPos = {}; // 開始位置
	XMFLOAT3 endPos = {}; // 終点位置
	float avoidMove = 5.0f; // 距離
	float avoidTime = 0.0f; // 経過時間
	bool easeFlag = false; // イージング開始フラグ
	int rushCount = 0;

	// ロープ管理用
	bool rFlag = false;
};


#pragma once
#include "Object3d.h"
#include <DirectXMath.h>
class Collision
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

	static Collision* GetInstance();

	// オブジェクト同士の距離を取得
	float GetLength(XMFLOAT3 pos_a, XMFLOAT3 pos_b);

	//オブジェクト同士の当たり判定
	bool CollisionObject(const std::unique_ptr<Object3d>& object_a, const std::unique_ptr<Object3d>& object_b);
};


#pragma once
#include "Object3d.h"
#include <DirectXMath.h>
class Collision
{
private: // エイリアス
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;
public: // サブクラス
	struct Sphere
	{
		// 中心座標
		XMVECTOR center = { 0, 0, 0, 1 };
		// 半径
		float radius = 1.0f;
	};
	struct Plane
	{
		// 法線ベクトル
		XMVECTOR normal = { 0, 1, 0, 0 };
		// 原点からの距離
		float distance = 0.0f;
	};
	struct Ray
	{
		// 始点座標
		XMVECTOR start = { 0, 0, 0, 1};
		// 方向
		XMVECTOR dir = { 1, 0, 0, 0};
	};
	struct Triangle
	{
		// 頂点座標
		XMVECTOR p0;
		XMVECTOR p1;
		XMVECTOR p2;
		// 法線ベクトル
		XMVECTOR normal;
	};
public: // 静的メンバ関数

	static Collision* GetInstance();
	//オブジェクト同士の当たり判定
	static bool CollisionObject(const std::unique_ptr<Object3d>& object_a, const std::unique_ptr<Object3d>& object_b);
	// 球と平面の当たり判定
	static bool CollisionSpherePlane(const Sphere& sphere, const Plane* plane, XMVECTOR* inter);
	// 球と三角形の当たり判定
	static bool CollisionSphereTriangle(const Sphere& sphere, const Triangle* triangle, XMVECTOR* closest);

	// レイと平面の当たり判定
	static bool CollisionRayPlane(const Ray& ray, const Plane& plane, float* distance = nullptr, XMVECTOR* inter = nullptr);
	// レイと法線付き三角形の当たり判定
	static bool CollisionRayTriangle(const Ray& ray, const Triangle& triangle, float* distance = nullptr, XMVECTOR* inter = nullptr);
	// レイと球の当たり判定
	static bool CollisionRaySphere(const Ray& ray, const Sphere& sphere, float* distance = nullptr, XMVECTOR* inter = nullptr);

private: // メンバ関数

	// オブジェクト同士の距離を取得
	float GetLength(XMFLOAT3 pos_a, XMFLOAT3 pos_b);
};


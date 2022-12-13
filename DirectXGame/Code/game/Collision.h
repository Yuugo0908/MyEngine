#pragma once
#include "Object3d.h"
#include "Operator.h"
class Collision
{
public: // サブクラス
	struct Sphere
	{
		// 中心座標
		XMVECTOR center = { 0, 0, 0 };
		// 半径
		float radius = 1.0f;
	};
	struct Plane
	{
		// 法線ベクトル
		XMVECTOR normal = { 0, 0, 0 };
		// 原点からの距離
		float distance = 0.0f;
	};
	struct Ray
	{
		// 始点座標
		XMVECTOR start = { 0, 0, 0 };
		// 方向
		XMVECTOR dir = { 1, 0, 0 };
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
	static bool CollisionSpherePlane(const Sphere& sphere, const Plane& plane, XMVECTOR* inter);
	// 球と三角形の当たり判定
	static bool CollisionSphereTriangle(const Sphere& sphere, const Triangle& triangle, XMVECTOR* closest);

	// レイと平面の当たり判定
	static bool CollisionRayPlane(const Ray& ray, const Plane& plane, float* distance = nullptr, XMVECTOR* inter = nullptr);
	// レイと法線付き三角形の当たり判定
	static bool CollisionRayTriangle(const Ray& ray, const Triangle& triangle, float* distance = nullptr, XMVECTOR* inter = nullptr);
	// レイと球の当たり判定
	static bool CollisionRaySphere(const Ray& ray, const Sphere& sphere, float* distance = nullptr, XMVECTOR* inter = nullptr);

	// BOXと点の当たり判定
	static bool CollisionBoxPoint(const XMFLOAT3 boxPos, const XMFLOAT3 boxRadius, XMFLOAT3& pos, const XMFLOAT3 radius, XMFLOAT3 oldPos);
	// 地面との当たり判定
	static bool CollisionStage(const XMFLOAT3 stagePos, const XMFLOAT3 stageRadius, XMFLOAT3& pos, const XMFLOAT3 radius, XMFLOAT3 oldPos);

public: // メンバ関数

	// オブジェクト同士の距離を取得
	float GetLength(XMFLOAT3 pos_a, XMFLOAT3 pos_b);

private: // メンバ変数

	Operator* ope = nullptr;
};


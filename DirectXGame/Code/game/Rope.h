#pragma once

#include "Object3d.h"
#include "Keyboard.h"
#include "Easing.h"
#include "Collision.h"
#include "Operator.h"
#include <DirectXMath.h>

class Rope
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

	bool Initialize(Keyboard* keyboard, Mouse* mouse);

	void Finalize()
	{
		delete ropeModel;
	}

	void Draw()
	{
		ropeObj->Draw();
	}

	void Update(XMFLOAT3& pPos, XMFLOAT3& ePos, const std::unique_ptr<Object3d>& object);

	void Throw(XMFLOAT3& pPos, XMFLOAT3& ePos, XMFLOAT3& rPos, XMFLOAT3& rScale, const std::unique_ptr<Object3d>& object);

	void Collision(const std::unique_ptr<Object3d>& object);

	float GetLength(XMFLOAT3 posA, XMFLOAT3 posB)
	{
		XMFLOAT3 len = { posA.x - posB.x, posA.y - posB.y, posA.z - posB.z };
		return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
	}

	// 円運動
	void CircularMotion(XMFLOAT3& pos, const XMFLOAT3 centerPos, const float r, int& angle, const int add);

	// 移動しているか
	const bool& GetmoveFlag() { return moveFlag; }
	void SetmoveFlag(bool moveFlag) { this->moveFlag = moveFlag; }
	// ロープがつながっているか
	const bool& GetrFlag() { return rFlag; }
	void SetrFlag(bool rFlag) { this->rFlag = rFlag; }

private: // メンバ変数

	Keyboard* keyboard = nullptr;
	Mouse* mouse = nullptr;
	Easing* easing = nullptr;

	Collision::Ray ray;

	//ロープモデル
	Model* ropeModel = nullptr;
	std::unique_ptr<Object3d> ropeObj = nullptr;

	// 変数
	XMFLOAT3 rPos = {};
	XMFLOAT3 rScale = {};
	float angleX = 0.0f; // X軸
	float angleY = 0.0f; // Y軸
	float vecXZ = 0.0f; // XZ平面上のベクトル
	float objLength = 0.0f;
	const float maxRope = 15.0f; // ロープの最大
	bool rFlag = false; // 接触フラグ
	bool rThrowFlag = false; // ロープを飛ばす
	bool rBackFlag = false; // ロープを戻す
	XMFLOAT3 manageRopePos = {}; // ロープ位置管理用
	XMFLOAT3 manageRopeScale = {0.2f, 0.2f, 0.2f}; // ロープスケール管理用
	int throwCount = 0;

	// 突進用
	XMFLOAT3 startPos = {}; // 開始位置
	XMFLOAT3 endPos = {}; // 終点位置
	float avoidMove = 5.0f; // 距離
	float avoidTime = 0.0f; // 経過時間
	bool pEaseFlag = false;
	bool eEaseFlag = false;

	// 移動管理フラグ
	bool moveFlag = false; // 自機が移動しているか
};


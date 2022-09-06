#pragma once
#include <DirectXMath.h>
class Camera
{
protected: // エイリアス
// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public: // 静的メンバ関数

	// カメラ初期化
	Camera(const int window_width, const int window_height);
	// 視点座標の取得
	const XMFLOAT3& GetEye() { return eye; }
	// 視点座標の設定
	void SetEye(XMFLOAT3 eye);
	// 注視点座標の取得
	const XMFLOAT3& GetTarget() { return target; }
	// 注視点座標の設定
	void SetTarget(XMFLOAT3 target);
	// 上方向ベクトルの取得
	const XMFLOAT3& GetUp() { return up; }
	// 上方向ベクトルの設定
	void SetUp(XMFLOAT3 up);

	// ビュー行列の取得
	const XMMATRIX& GetMatView() {
		return matView;
	}

	// 射影行列の取得
	const XMMATRIX& GetMatProjection() {
		return matProjection;
	}

	// ビュー射影行列の取得
	const XMMATRIX& GetMatViewProjection() {
		return matViewProjection;
	}

	// ベクトルによる移動
	void CameraMoveVector(XMFLOAT3 move);
	void CameraMoveEyeVector(XMFLOAT3 move);

	/// 毎フレーム更新
	void Update();
	/// ビュー行列を更新
	void UpdateViewMatrix();
	/// 射影行列を更新
	void UpdateProjectionMatrix();


public: // メンバ変数
	// ビュー行列
	XMMATRIX matView = DirectX::XMMatrixIdentity();
	// 射影行列
	XMMATRIX matProjection = DirectX::XMMatrixIdentity();
	// ビュー射影行列
	XMMATRIX matViewProjection = DirectX::XMMatrixIdentity();
	//対象とカメラの距離
	float distance = 20.0f;
	// 視点座標
	XMFLOAT3 eye = { 0, 0, distance };
	// 注視点座標
	XMFLOAT3 target = { 0, 0, 0 };
	// 上方向ベクトル
	XMFLOAT3 up = { 0, 1, 0 };
	// ビュー行列ダーティフラグ
	bool viewDirty = false;
	// 射影行列ダーティフラグ
	bool projectionDirty = false;
	// アスペクト比
	float aspectRatio = 1.0f;
};
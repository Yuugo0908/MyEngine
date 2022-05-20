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
	static void Initialize(const int window_width, const int window_height);
	// 視点座標の取得
	static const XMFLOAT3& GetEye() { return eye; }
	// 視点座標の設定
	static void SetEye(XMFLOAT3 eye);
	// 注視点座標の取得
	static const XMFLOAT3& GetTarget() { return target; }
	// 注視点座標の設定
	static void SetTarget(XMFLOAT3 target);
	// ベクトルによる移動
	static void CameraMoveVector(XMFLOAT3 move);
	static void CameraMoveEyeVector(XMFLOAT3 move);
	// ビュー行列を更新
	static void UpdateViewMatrix();
	//射影行列を更新
	//static void UpdateProjectionMatrix();
private: // 静的メンバ変数
	// ビュー行列
	static XMMATRIX matView;
	// 射影行列
	static XMMATRIX matProjection;
	// ビュー射影行列
	static XMMATRIX matViewProjection;
	// 視点座標
	static XMFLOAT3 eye;
	// 注視点座標
	static XMFLOAT3 target;
	// 上方向ベクトル
	static XMFLOAT3 up;
public: // メンバ関数
	static const XMMATRIX& GetMatViewProjection();
};
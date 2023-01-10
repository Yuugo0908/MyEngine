#pragma once
#include "Mouse.h"
#include "Operator.h"
#include "DebugText.h"
class Camera
{
private:
	// カメラ初期化
	Camera();

	~Camera();

public:
	// コピーコンストラクタを無効化
	Camera(const Camera& obj) = delete;
	// 代入演算子を無効化
	Camera& operator=(const Camera& obj) = delete;

public: // 静的メンバ関数

	static Camera* GetInstance();
	// 初期化
	bool Initialize(const int window_width, const int window_height, Mouse* mouse);
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
	void CameraMoveVector(const XMVECTOR& move);
	void CameraMoveEyeVector(const XMVECTOR& move);

	// 毎フレーム更新
	void Update();
	// ビュー行列を更新
	void UpdateViewMatrix();
	// 射影行列を更新
	void UpdateProjectionMatrix();
	// カメラのシェイクを実行
	void CameraShake(bool& flag);
	// カメラの追尾
	XMFLOAT3 CameraTrack(XMFLOAT3 pPos);
	float CameraRot(XMFLOAT3 pPos);

	void Reset();


private: // メンバ変数
	// 入力クラスのポインタ
	Mouse* mouse;
	// ビュー行列
	XMMATRIX matView = DirectX::XMMatrixIdentity();
	// 射影行列
	XMMATRIX matProjection = DirectX::XMMatrixIdentity();
	// ビュー射影行列
	XMMATRIX matViewProjection = DirectX::XMMatrixIdentity();
	// 回転行列
	XMMATRIX matRot = DirectX::XMMatrixIdentity();
	//対象とカメラの距離
	float distance = 15.0f;
	// スケーリング
	float scaleX = 1.0f;
	float scaleY = 1.0f;
	// 視点座標
	XMFLOAT3 eye = { 0, 0, distance };
	// 注視点座標
	XMFLOAT3 target = { 0, 0, 0 };
	// 上方向ベクトル
	XMFLOAT3 up = { 0, 1, 0 };
	// カメラと対象物までの距離
	XMVECTOR cameraLength = {};
	// ビュー行列ダーティフラグ
	bool viewDirty = false;
	// 射影行列ダーティフラグ
	bool projectionDirty = false;
	// アスペクト比
	float aspectRatio = 1.0f;
	// シェイク座標
	XMFLOAT3 shake = {};
	// シェイク管理カウンター
	int shakeCount = 0;
	// カメラの動作管理用カウンター
	int moveCount = 30;
	// 注視点から視点へのベクトルと、上方向ベクトル
	XMVECTOR vTargetEye = {};
	XMVECTOR vUp = {};

	Mouse::MouseMove mouseMove;

	bool dirty = false;
	float dy = 0.0f;
	float angleX = 0;
	float angleY = 0;
	float speed = 0.75f;
};
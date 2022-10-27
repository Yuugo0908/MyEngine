#include "Camera.h"
using namespace DirectX;

Camera::Camera(const int window_width, const int window_height, Mouse* mouse)
{
	assert(mouse);

	this->mouse = mouse;
	// 画面サイズに対する相対的なスケールに調整
	scaleX = 1.0f / (float)window_width;
	scaleY = 1.0f / (float)window_height;
	aspectRatio = (float)window_width / window_height;

	//ビュー行列の計算
	UpdateViewMatrix();

	// 射影行列の計算
	UpdateProjectionMatrix();

	// ビュープロジェクションの合成
	matViewProjection = matView * matProjection;
}

void Camera::SetEye(XMFLOAT3 eye)
{
	Camera::eye = eye;
	viewDirty = true;
}

void Camera::SetTarget(XMFLOAT3 target)
{
	Camera::target = target;
	viewDirty = true;
}

void Camera::SetUp(XMFLOAT3 up)
{
	Camera::up = up;
	viewDirty = true;
}

void Camera::CameraMoveVector(XMFLOAT3 move)
{
	XMFLOAT3 eye_moved = GetEye();
	XMFLOAT3 target_moved = GetTarget();

	eye_moved.x += move.x;
	eye_moved.y += move.y;
	eye_moved.z += move.z;

	target_moved.x += move.x;
	target_moved.y += move.y;
	target_moved.z += move.z;

	SetEye(eye_moved);
	SetTarget(target_moved);
}

void Camera::CameraMoveVector(const XMVECTOR& move)
{
	// 視点と注視点座標を移動し、反映
	XMFLOAT3 eye_moved = GetEye();
	XMFLOAT3 target_moved = GetTarget();

	eye_moved.x += move.m128_f32[0];
	eye_moved.y += move.m128_f32[1];
	eye_moved.z += move.m128_f32[2];

	target_moved.x += move.m128_f32[0];
	target_moved.y += move.m128_f32[1];
	target_moved.z += move.m128_f32[2];

	SetEye(eye_moved);
	SetTarget(target_moved);
}

void Camera::CameraMoveEyeVector(XMFLOAT3 move)
{
	XMFLOAT3 eye_moved = GetEye();

	eye_moved.x += move.x;
	eye_moved.y += move.y;
	eye_moved.z += move.z;

	SetEye(eye_moved);
}

void Camera::CameraMoveEyeVector(const XMVECTOR& move)
{
	// 視点座標を移動し、反映
	XMFLOAT3 eye_moved = GetEye();

	eye_moved.x += move.m128_f32[0];
	eye_moved.y += move.m128_f32[1];
	eye_moved.z += move.m128_f32[2];

	SetEye(eye_moved);
}

void Camera::Update()
{
	bool dirty = false;
	float angleX = 0;
	float angleY = 0;

	// マウスの入力を取得
	Mouse::MouseMove mouseMove = mouse->GetMouseMove();

	// マウスの左ボタンが押されていたらカメラを回転させる
	if (mouse->PushMouseLeft())
	{
		float dy = mouseMove.MouseX * scaleY;
		//float dx = mouseMove.MouseY * scaleX;

		//angleX = -dx * XM_PI;
		angleY = -dy * XM_PI;
		dirty = true;
	}

	// マウスの中ボタンが押されていたらカメラを並行移動させる
	/*if (mouse->PushMouseMiddle())
	{
		float dx = mouseMove.MouseX / 100.0f;
		float dy = mouseMove.MouseY / 100.0f;

		XMVECTOR move = { -dx, +dy, 0, 0 };
		move = XMVector3Transform(move, matRot);

		CameraMoveVector(move);
		dirty = true;
	}*/

	// ホイール入力で距離を変更
	if (mouseMove.MouseZ != 0) {
		distance -= mouseMove.MouseZ / 100.0f;
		distance = max(distance, 1.0f);
		dirty = true;
	}

	if (dirty || viewDirty) {
		// 追加回転分の回転行列を生成
		XMMATRIX matRotNew = XMMatrixIdentity();
		matRotNew *= XMMatrixRotationX(-angleX);
		matRotNew *= XMMatrixRotationY(-angleY);
		// 累積の回転行列を合成
		matRot = matRotNew * matRot;

		// 注視点から視点へのベクトルと、上方向ベクトル
		XMVECTOR vTargetEye = { 0.0f, 0.0f, -distance, 1.0f };
		XMVECTOR vUp = { 0.0f, 1.0f, 0.0f, 0.0f };

		// ベクトルを回転
		vTargetEye = XMVector3Transform(vTargetEye, matRot);
		vUp = XMVector3Transform(vUp, matRot);

		// 注視点からずらした位置に視点座標を決定
		const XMFLOAT3& target = GetTarget();
		SetEye({ target.x + vTargetEye.m128_f32[0], target.y + vTargetEye.m128_f32[1] + 10.0f, target.z + vTargetEye.m128_f32[2] });
		SetUp({ vUp.m128_f32[0], vUp.m128_f32[1], vUp.m128_f32[2] });
	}

	if (viewDirty || projectionDirty) {
		// 再計算必要なら
		if (viewDirty) {
			// ビュー行列更新
			UpdateViewMatrix();
			viewDirty = false;
		}

		// 再計算必要なら
		if (projectionDirty) {
			// ビュー行列更新
			UpdateProjectionMatrix();
			projectionDirty = false;
		}
		// ビュープロジェクションの合成
		matViewProjection = matView * matProjection;
	}
}

void Camera::UpdateViewMatrix()
{
	// 視点座標
	XMVECTOR eyePosition = XMLoadFloat3(&eye);
	// 注視点座標
	XMVECTOR targetPosition = XMLoadFloat3(&target);
	// （仮の）上方向
	XMVECTOR upVector = XMLoadFloat3(&up);

	// カメラZ軸（視線方向）
	XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
	// 0ベクトルだと向きが定まらないので除外
	assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
	assert(!XMVector3IsInfinite(cameraAxisZ));
	assert(!XMVector3Equal(upVector, XMVectorZero()));
	assert(!XMVector3IsInfinite(upVector));
	// ベクトルを正規化
	cameraAxisZ = XMVector3Normalize(cameraAxisZ);

	// カメラのX軸（右方向）
	XMVECTOR cameraAxisX;
	// X軸は上方向→Z軸の外積で求まる
	cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
	// ベクトルを正規化
	cameraAxisX = XMVector3Normalize(cameraAxisX);

	// カメラのY軸（上方向）
	XMVECTOR cameraAxisY;
	// Y軸はZ軸→X軸の外積で求まる
	cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);

	// ここまでで直交した3方向のベクトルが揃う
	//（ワールド座標系でのカメラの右方向、上方向、前方向）	

	// カメラ回転行列
	XMMATRIX matCameraRot;
	// カメラ座標系→ワールド座標系の変換行列
	matCameraRot.r[0] = cameraAxisX;
	matCameraRot.r[1] = cameraAxisY;
	matCameraRot.r[2] = cameraAxisZ;
	matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
	// 転置により逆行列（逆回転）を計算
	matView = XMMatrixTranspose(matCameraRot);

	// 視点座標に-1を掛けた座標
	XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
	// カメラの位置からワールド原点へのベクトル（カメラ座標系）
	XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);	// X成分
	XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);	// Y成分
	XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);	// Z成分
	// 一つのベクトルにまとめる
	XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
	// ビュー行列に平行移動成分を設定
	matView.r[3] = translation;
}

void Camera::UpdateProjectionMatrix()
{
	// 透視投影による射影行列の生成
	matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(60.0f),
		aspectRatio,
		0.1f, 1000.0f);
}

void Camera::CameraShake(XMFLOAT3& eye, XMFLOAT3& target, bool& flag)
{
	//カメラ更新
	int power = 5;
	shake.x = static_cast<float>(rand() % power) / 10;
	shake.y = static_cast<float>(rand() % power) / 10;

	shakeCount++;
	if (shakeCount > 20)
	{
		shake.x = 0.0f;
		shake.y = 0.0f;
		shakeCount = 0;
		flag = false;
	}

	eye = GetEye();
	target = GetTarget();

	eye.x += shake.x;
	eye.y += shake.y;
	target.x += shake.x;
	target.y += shake.y;

	SetEye(eye);
	SetTarget(target);
	Update();
}

#pragma once
#include "Object3d.h"
#include "Keyboard.h"
#include "Collision.h"
#include "Operator.h"
#include "Camera.h"
#include "Rope.h"
#include "Mapchip.h"

class Player
{
public: // メンバ関数

	bool Initialize(Keyboard* keyboard, Mouse* mouse);

	void Finalize() { delete playerModel; }

	void Update(bool rFlag, bool moveFlag);

	void Draw() { playerObj->Draw(); }

	void Rush(bool rFlag);

	void Jump();

	// オブジェクト
	const std::unique_ptr<Object3d>& GetObj() { return playerObj; }
	// 突進
	const bool& GetAvoidFlag() { return avoidFlag; }
	void SetAvoidFlag(bool avoidFlag) { this->avoidFlag = avoidFlag; }
	// ジャンプ
	const bool& GetOnGround() { return onGround; }
	void SetOnGround(bool onGround) { this->onGround = onGround; }

	// 位置
	const XMFLOAT3& GetPos() { return pPos; }
	void SetPos(XMFLOAT3 pPos) { this->pPos = pPos; }

	const XMFLOAT3& GetRot() { return pRot; }
	void SetRot(XMFLOAT3 pRot) { this->pRot = pRot; }

	const XMFLOAT3& GetScale() { return pScale; }
	void SetScale(XMFLOAT3 pScale) { this->pScale = pScale; }

private: // メンバ変数

	Keyboard* keyboard = nullptr;
	Mouse* mouse = nullptr;
	Rope* rope = nullptr;
	Camera* camera = Camera::GetInstance();

	// モデル
	Model* playerModel = nullptr;
	std::unique_ptr<Object3d> playerObj = nullptr;

	// プレイヤー
	XMFLOAT3 pPos = {};//座標
	XMFLOAT3 pScale = {};//大きさ
	XMFLOAT3 pRot = {};//回転

	float pSpeed = 0.35f;
	bool onGround = false;//自由落下のフラグ
	bool jumpFlag = false;
	bool moveFlag = false;//移動管理フラグ
	float pMove = 0.0f;//移動量
	float pAcc = 0.2f;//加速
	float pVal = 0.2f;//速度
	float pDown = 0.0f;
	float pGra = 0.1f;//重力
	float rate = 1.0f; // 斜め移動時の制限

	// 突進用
	bool avoidFlag = false; // 回避開始フラグ
	int avoidCount = 0;

	// ロープ管理用
	bool rFlag = false;

	// カメラ距離取得用
	XMFLOAT3 cameraTrack = {};
	float cameraRot = 0.0f;
};


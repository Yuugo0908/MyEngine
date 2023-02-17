#pragma once
#include "Object3d.h"
#include "Keyboard.h"
#include "Controller.h"
#include "Collision.h"
#include "Easing.h"
#include "Operator.h"
#include "Camera.h"

class Player
{
public: // メンバ関数

	bool Initialize();

	void Finalize() { delete playerModel; }

	void Update(bool rFlag, bool moveFlag);

	void Draw() { playerObj->Draw(); }

	void Attack(XMFLOAT3 targetPos, bool& flag, float& avoidTime);

	void Rush(bool rFlag);

	void Jump();

	void Reset();

	// マップチップ当たり判定
	bool MapCollide(XMFLOAT3 boxPos, XMFLOAT3 boxScale);

	bool StageCollide(XMFLOAT3 stagePos, XMFLOAT3 stageScale);

	// オブジェクト
	const std::unique_ptr<Object3d>& GetObj() { return playerObj; }
	// 回避
	const bool& GetAvoidFlag() { return avoidFlag; }
	void SetAvoidFlag(bool avoidFlag) { this->avoidFlag = avoidFlag; }
	// 着地
	const bool& GetOnGround() { return onGround; }
	void SetOnGround(bool onGround) { this->onGround = onGround; }

private: // メンバ変数

	Controller* controller = nullptr;
	Keyboard* keyboard = nullptr;
	Mouse* mouse = nullptr;
	Camera* camera = Camera::GetInstance();

	// モデル
	Model* playerModel = nullptr;
	std::unique_ptr<Object3d> playerObj = nullptr;

	// プレイヤー
	XMFLOAT3 pPos = {};//座標
	XMFLOAT3 pPosOld = {};
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

	// カメラ距離取得用
	XMFLOAT3 cameraTrack = {};
	float cameraRot = 0.0f;
};


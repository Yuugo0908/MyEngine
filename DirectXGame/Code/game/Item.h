#pragma once
#include "Object3d.h"
#include "Keyboard.h"
#include "Easing.h"
#include "Collision.h"
#include "Operator.h"
#include "Player.h"

class Item final
{
private:
	Item();

	~Item();
public:
	// コピーコンストラクタを無効化
	Item(const Item& obj) = delete;
	// 代入演算子を無効化
	Item& operator=(const Item& obj) = delete;

public: // メンバ関数

	static Item* GetInstance();

	bool Initialize();

	void Update();

	void Draw() { itemObj->Draw(); }

	// オブジェクト
	const std::unique_ptr<Object3d>& GetObj() { return itemObj; }

	// 位置
	const XMFLOAT3& GetPos() { return itemPos; }
	void SetPos(XMFLOAT3 iPos) { this->itemPos = iPos; }

private: // メンバ変数

	// モデル
	Model* itemModel = nullptr;
	std::unique_ptr<Object3d> itemObj = nullptr;

	// プレイヤー
	XMFLOAT3 itemPos = {};//座標
};
#pragma once
#include <string>
#include <DirectXMath.h>
#include <vector>

// DirectX::を省略
using XMFLOAT2 = DirectX::XMFLOAT2;
using XMFLOAT3 = DirectX::XMFLOAT3;
using XMFLOAT4 = DirectX::XMFLOAT4;
using XMMATRIX = DirectX::XMMATRIX;
using XMVECTOR = DirectX::XMVECTOR;

struct Node
{
	//名前
	std::string name;
	//ローカルスケール
	XMVECTOR scaling = { 1, 1, 1, 0 };
	//ローカル回転角
	XMVECTOR rotation = { 0, 0, 0, 0 };
	//ローカル移動
	XMVECTOR translation = { 0, 0, 0, 1 };
	//ローカル変形行列
	XMMATRIX transform;
	//グローバル変形行列
	XMMATRIX globalTransform;
	//親ノード
	Node* parent = nullptr;
};

class FbxModel
{
public:
	//フレンドクラス
	friend class FbxLoader;
private:
	//モデル名
	std::string name;
	//ノード配列
	std::vector<Node> nodes;
};
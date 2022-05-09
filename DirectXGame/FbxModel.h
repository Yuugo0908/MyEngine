#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <string>
#include <DirectXMath.h>
#include <vector>
#include <DirectXTex.h>


// DirectX::を省略
using XMFLOAT2 = DirectX::XMFLOAT2;
using XMFLOAT3 = DirectX::XMFLOAT3;
using XMFLOAT4 = DirectX::XMFLOAT4;
using XMMATRIX = DirectX::XMMATRIX;
using XMVECTOR = DirectX::XMVECTOR;
using TexMetadata = DirectX::TexMetadata;
using ScratchImage = DirectX::ScratchImage;

struct Node
{
	// 名前
	std::string name;
	// ローカルスケール
	XMVECTOR scaling = { 1, 1, 1, 0 };
	// ローカル回転角
	XMVECTOR rotation = { 0, 0, 0, 0 };
	// ローカル移動
	XMVECTOR translation = { 0, 0, 0, 1 };
	// ローカル変形行列
	XMMATRIX transform;
	// グローバル変形行列
	XMMATRIX globalTransform;
	// 親ノード
	Node* parent = nullptr;
};

class FbxModel
{
private: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
// std::を省略
	template <class T> using vector = std::vector<T>;
	using string = std::string;
public:
	// フレンドクラス
	friend class FbxLoader;
public:// サブクラス
	// 頂点データ構造体
	struct VertexPosNormalUv
	{
		XMFLOAT3 pos;	// xyz座標
		XMFLOAT3 normal;// 法線ベクトル
		XMFLOAT2 uv;	// uv座標
	};
private:
	// モデル名
	std::string name;
	// ノード配列
	std::vector<Node> nodes;
	// メッシュを持つノード
	Node* meshNode = nullptr;
	// 頂点データ配列
	std::vector<VertexPosNormalUv> vertices;
	// 頂点インデックス配列
	std::vector<unsigned short> indices;
	// アンビエント係数
	XMFLOAT3 ambient = { 1, 1, 1 };
	// ディフューズ係数
	XMFLOAT3 diffuse = { 1, 1, 1 };
	// テクスチャメタデータ
	TexMetadata metadata = {};
	// スクラッチイメージ
	ScratchImage scratchImg = {};
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;
	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	// SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
public:
	void CreateBuffers(ID3D12Device* device);
	// 描画
	void Draw(ID3D12GraphicsCommandList* cmdList);
	// モデルの変形行列取得
	const XMMATRIX& GetModelTransform() { return meshNode->globalTransform; }
};
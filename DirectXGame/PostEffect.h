#pragma once
#include "Sprite.h"
class PostEffect : public Sprite
{
protected: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

private: // 静的メンバ変数
	// 画面クリアカラー
	static const float clearColor[4];
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PostEffect();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// テクスチャ生成
	/// </summary>
	void CreateTex();

	/// <summary>
	/// シェーダーリソースビュー作成
	/// </summary>
	void CreateSRV();

	/// <summary>
	/// レンダーターゲットビュー生成
	/// </summary>
	void CreateRTV();

	/// <summary>
	/// 深度バッファ生成
	/// </summary>
	void CreateDepth();

	/// <summary>
	/// デプスステンシルビュー生成
	/// </summary>
	void CreateDSV();

	/// <summary>
	/// 描画コマンドの発行
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="cmdList"></param>
	void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	/// <param name="cmdList"></param>
	void PostDraw(ID3D12GraphicsCommandList* cmdList);

private: // メンバ変数
	// テクスチャバッファ
	ComPtr<ID3D12Resource> texBuff;
	// 深度バッファ
	ComPtr<ID3D12Resource> depthBuff;
	// SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	// RTV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	// DSV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
};


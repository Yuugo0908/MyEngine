#pragma once
#include <DirectXMath.h>
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>

class Light
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
public: // サブクラス
	// 定数バッファ用データ構造体
	struct ConstBufferData
	{
		XMVECTOR lightVec; // ライトのベクトル
		XMFLOAT3 lightColor; // ライトの色
	};
private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* device;
public: // 静的メンバ関数
	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device);
	/// <summary>
	/// インスタンス生成
	/// </summary>
	static Light* Create();
private: // メンバ変数
	// 定数バッファ
	ComPtr<ID3D12Resource> constBuffer;
	// ライト光線方向(単位ベクトル)
	XMVECTOR lightdir = { 1, 0, 0, 0 };
	// ライト色
	XMFLOAT3 lightColor = { 1, 0, 0 };
	// ダーティフラグ
	bool dirty = false;
public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 初期化
	/// </summary>
	void TransferConstBuffer();
	/// <summary>
	/// ライト方向をセット
	/// </summary>
	/// <param name = "lightdir">ライト方向</param>
	void SetLightDir(const XMVECTOR& lightdir);
	/// <summary>
	/// ライト色をセット
	/// </summary>
	/// <param name = "lightcolor">ライト方向</param>
	void SetLightColor(const XMFLOAT3& lightcolor);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex);
};


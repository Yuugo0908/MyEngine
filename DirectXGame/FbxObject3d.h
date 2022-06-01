#pragma once

#include "FbxModel.h"
#include "Camera.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <string>
#include <DirectXMath.h>

class FbxObject3d
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

	// ローカルスケール
	XMFLOAT3 scale = { 1, 1, 1 };
	// X, Y, Z軸回りのローカル回転角
	XMFLOAT3 rotation = { 0, 0, 0 };
	// ローカル座標
	XMFLOAT3 position = { 0, 0, 0 };
	// ローカルワールド変換行列
	XMMATRIX matWorld;
	// モデル
	FbxModel* fbxModel = nullptr;
public: // 定数
	// ボーンの最大数
	static const int MAX_BONES = 32;
public: // 定数バッファ用データ構造体(スキニング)
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[MAX_BONES];
	};
public: // 静的メンバ関数
	// setter
	static void SetDevice(ID3D12Device* device) { FbxObject3d::device = device; }
	static void SetCamera(Camera* camera) { FbxObject3d::camera = camera; }
private:
	// デバイス
	static ID3D12Device* device;
	// カメラ
	static Camera* camera;
public: // サブクラス
	// 定数バッファ用データ構造体
	struct ConstBufferDataTransform
	{
		XMMATRIX viewproj;	// ビュープロジェクション行列
		XMMATRIX world;		// ワールド行列
		XMFLOAT3 cameraPos;	// カメラ座標（ワールド座標）
	};
public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// グラフィックスパイプラインの生成
	/// </summary>
	static void CreateGraphicsPipeline();
	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();
	/// <summary>
	/// モデルのセット
	/// </summary>
	/// <param name="model">モデル</param>
	void SetModel(FbxModel* fbxModel) { this->fbxModel = fbxModel; }
	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	// 座標の設定
	void SetPosition(XMFLOAT3 position) { this->position = position; }
private: // メンバ変数
	// 定数バッファ
	ComPtr<ID3D12Resource> constBufferTransform;
	// 定数バッファ(スキン)
	ComPtr<ID3D12Resource> constBufferSkin;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestate;
	// コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;
};

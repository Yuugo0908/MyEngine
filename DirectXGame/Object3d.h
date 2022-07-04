#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include "Model.h"
#include "Camera.h"

class Object3d
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
	// 定数バッファ用データ構造体B0
	struct ConstBufferDataB0
	{
		//XMFLOAT4 color; // 色 (RGBA)
		XMMATRIX mat; // ３Ｄ変換行列
	};

public: // 静的メンバ関数
	// 静的初期化
	static bool StaticInitialize(ID3D12Device* device);
	// 描画前処理
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);
	// 描画後処理
	static void PostDraw();
	// 3Dオブジェクト生成
	static Object3d* Create();
	// グラフィックパイプライン生成
	static bool CreateGraphicsPipeline();

private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* device;
	// コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestate;	
	// 頂点シェーダオブジェクト
	static ComPtr<ID3DBlob> vsBlob;
	// ピクセルシェーダオブジェクト
	static ComPtr<ID3DBlob> psBlob;
	// エラーオブジェクト
	static ComPtr<ID3DBlob> errorBlob;

public: // メンバ関数
	bool Initialize();
	// 毎フレーム処理
	void Update();
	// 描画
	void Draw();
	// モデルスケールの取得
	const XMFLOAT3& GetScale() { return scale; }
	// モデルスケールの設定
	void SetScale(XMFLOAT3 scale) { this->scale = scale; }
	// 回転角の取得
	const XMFLOAT3& GetRotation() { return rotation; }
	// 回転角の設定
	void SetRotation(XMFLOAT3 rotation) { this->rotation = rotation; }
	// 座標の取得
	const XMFLOAT3& GetPosition() { return position; }
	// 座標の設定
	void SetPosition(XMFLOAT3 position) { this->position = position; }
	// モデルの設定
	void SetModel(Model* model) { this->model = model; };

private: // メンバ変数
	ComPtr<ID3D12Resource> constBufferB0; // 定数バッファ
	// 色
	XMFLOAT4 color = { 1,1,1,1 };
	// ローカルスケール
	XMFLOAT3 scale = { 1,1,1 };
	// X,Y,Z軸回りのローカル回転角
	XMFLOAT3 rotation = { 0,0,0 };
	// ローカル座標
	XMFLOAT3 position = { 0,0,0 };
	// ローカルワールド変換行列
	XMMATRIX matWorld;
	// 親オブジェクト
	Object3d* parent = nullptr;

	Model* model = nullptr;
};
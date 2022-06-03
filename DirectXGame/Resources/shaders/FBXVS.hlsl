#include "FBX.hlsli"

// スキニング後の頂点・法線が入る
struct SkinOutput
{
	float4 pos;
	float3 normal;
};

// スキニング計算
SkinOutput ComputeSkin(VSInput input)
{
	// ゼロクリア
	SkinOutput output = (SkinOutput)0;

	uint iBone;
	float boneWeight;
	matrix skinMatrix;

	// ボーン0
	iBone = input.boneIndices.x;
	boneWeight = input.boneWeights.x;
	skinMatrix = matSkinning[iBone];
	output.pos += boneWeight * mul(skinMatrix, input.pos);
	output.normal += boneWeight * mul((float3x3)skinMatrix, input.normal);

	// ボーン1
	iBone = input.boneIndices.y;
	boneWeight = input.boneWeights.y;
	skinMatrix = matSkinning[iBone];
	output.pos += boneWeight * mul(skinMatrix, input.pos);
	output.normal += boneWeight * mul((float3x3)skinMatrix, input.normal);

	// ボーン2
	iBone = input.boneIndices.z;
	boneWeight = input.boneWeights.z;
	skinMatrix = matSkinning[iBone];
	output.pos += boneWeight * mul(skinMatrix, input.pos);
	output.normal += boneWeight * mul((float3x3)skinMatrix, input.normal);

	// ボーン3
	iBone = input.boneIndices.w;
	boneWeight = input.boneWeights.w;
	skinMatrix = matSkinning[iBone];
	output.pos += boneWeight * mul(skinMatrix, input.pos);
	output.normal += boneWeight * mul((float3x3)skinMatrix, input.normal);

	return output;
}

// エントリーポイント
VSOutput main(VSInput input)
{
	SkinOutput skinned = ComputeSkin(input);
	// 法線ワールド行列によるスケーリング・回転を適用
	float4 wnormal = normalize(mul(world, float4(skinned.normal, 0)));
	// ピクセルシェーダーに渡す値
	VSOutput output;
	// 行列による座標変換
	output.svpos = mul(mul(viewproj, world), skinned.pos);
	// ワールド法線を次のステージに渡す
	output.normal = wnormal.xyz;
	// 入力した値をそのまま次のステージに渡す
	output.uv = input.uv;

	return output;
}
#include "FBX.hlsli"

// �X�L�j���O��̒��_�E�@��������
struct SkinOutput
{
	float4 pos;
	float3 normal;
};

// �X�L�j���O�v�Z
SkinOutput ComputeSkin(VSInput input)
{
	// �[���N���A
	SkinOutput output = (SkinOutput)0;

	uint iBone;
	float boneWeight;
	matrix skinMatrix;

	// �{�[��0
	iBone = input.boneIndices.x;
	boneWeight = input.boneWeights.x;
	skinMatrix = matSkinning[iBone];
	output.pos += boneWeight * mul(skinMatrix, input.pos);
	output.normal += boneWeight * mul((float3x3)skinMatrix, input.normal);

	// �{�[��1
	iBone = input.boneIndices.y;
	boneWeight = input.boneWeights.y;
	skinMatrix = matSkinning[iBone];
	output.pos += boneWeight * mul(skinMatrix, input.pos);
	output.normal += boneWeight * mul((float3x3)skinMatrix, input.normal);

	// �{�[��2
	iBone = input.boneIndices.z;
	boneWeight = input.boneWeights.z;
	skinMatrix = matSkinning[iBone];
	output.pos += boneWeight * mul(skinMatrix, input.pos);
	output.normal += boneWeight * mul((float3x3)skinMatrix, input.normal);

	// �{�[��3
	iBone = input.boneIndices.w;
	boneWeight = input.boneWeights.w;
	skinMatrix = matSkinning[iBone];
	output.pos += boneWeight * mul(skinMatrix, input.pos);
	output.normal += boneWeight * mul((float3x3)skinMatrix, input.normal);

	return output;
}

// �G���g���[�|�C���g
VSOutput main(VSInput input)
{
	SkinOutput skinned = ComputeSkin(input);
	// �@�����[���h�s��ɂ��X�P�[�����O�E��]��K�p
	float4 wnormal = normalize(mul(world, float4(skinned.normal, 0)));
	// �s�N�Z���V�F�[�_�[�ɓn���l
	VSOutput output;
	// �s��ɂ����W�ϊ�
	output.svpos = mul(mul(viewproj, world), skinned.pos);
	// ���[���h�@�������̃X�e�[�W�ɓn��
	output.normal = wnormal.xyz;
	// ���͂����l�����̂܂܎��̃X�e�[�W�ɓn��
	output.uv = input.uv;

	return output;
}
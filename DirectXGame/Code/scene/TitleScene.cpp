#include "TitleScene.h"
#include "TitleScene.h"
#include <imgui.h>
#include <cassert>

TitleScene::TitleScene() {

}

TitleScene::~TitleScene() {

}

void TitleScene::Initialize() {
	// �f�o�C�X�̃Z�b�g
	FbxObject3d::SetDevice(dxCommon->GetDevice());
	// �O���t�B�b�N�X�p�C�v���C������
	FbxObject3d::CreateGraphicsPipeline();

	// �^�C�g���摜�ǂݍ���
	if (!Image2d::LoadTexture(1, L"Resources/title.png"))
	{
		assert(0);
	}
	title = Image2d::Create(1, { 0.0f,0.0f });
	title->SetSize({ 1280.0f,720.0f });

	if (!Image2d::LoadTexture(8, L"Resources/black_backGround.png"))
	{
		assert(0);
	}
	backGround = Image2d::Create(8, { 0.0f,0.0f });
	backGround->SetSize({ 1280.0f,720.0f });
	backGround->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f });

	if (!Image2d::LoadTexture(9, L"Resources/Operation_Explanation.png"))
	{
		assert(0);
	}
	explanation = Image2d::Create(9, { 0.0f,0.0f });
	explanation->SetSize({ 1280.0f,720.0f });
	explanation->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f });
}

void TitleScene::Finalize()
{

}

void TitleScene::Update() {

	if (!fadeIn && keyboard->TriggerKey(DIK_SPACE))
	{

		if (!expFlag)
		{
			expFlag = true;
			explanation->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f });
		}

		else if (!fadeIn)
		{
			fadeIn = true;
		}
	}

	if (fadeIn)
	{
		alpha += 0.02f;
		backGround->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, alpha });
		if (alpha >= 1.0f)
		{
			//nowScene = game_;
			fadeIn = false;
			fadeOut = true;
			explanation->SetColor(XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f });
		}
	}
}

void TitleScene::Draw() {
#pragma region �w�i�摜�`��
	// �w�i�摜�`��O����
	Image2d::PreDraw(dxCommon->GetCommandList());


	// �摜�`��㏈��
	Image2d::PostDraw();
	// �[�x�o�b�t�@�N���A
	dxCommon->ClearDepthBuffer();
#pragma endregion �w�i�摜�`��

#pragma region 3D�I�u�W�F�N�g�`��
	// 3D�I�u�W�F�N�g�`��O����
	Object3d::PreDraw(dxCommon->GetCommandList());

	// 3D�I�u�W�F�N�g�`��㏈��
	Object3d::PostDraw();
#pragma endregion 3D�I�u�W�F�N�g�`��

#pragma region �O�i�摜�`��
	// �O�i�摜�`��O����
	Image2d::PreDraw(dxCommon->GetCommandList());

	// �O�i�摜�̕`��

	title->Draw();
	explanation->Draw();
	backGround->Draw();

	// �f�o�b�O�e�L�X�g�̕`��
	DebugText::GetInstance()->DrawAll(dxCommon->GetCommandList());
	// �摜�`��㏈��
	Image2d::PostDraw();
#pragma endregion �O�i�摜�`��
}
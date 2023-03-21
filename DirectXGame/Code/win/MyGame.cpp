#include "MyGame.h"

void MyGame::Initialize()
{
	// �Q�[���E�B���h�E�̍쐬
	win = new WinApp();
	win->CreateGameWindow();

	//DirectX����������
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(win);


	//���͂̏�����
	keyboard = Keyboard::GetInstance();
	keyboard->Initialize(win);
	controller = Controller::GetInstance();
	mouse = Mouse::GetInstance();
	mouse->Initialize(win);

	// �J����������
	camera = Camera::GetInstance();
	camera->Initialize(WinApp::window_width, WinApp::window_height);

	// �I�[�f�B�I�̏�����
	audio = new Audio();
	if (!audio->Initialize())
	{
		assert(0);
	}
	// �摜�ÓI������
	if (!Image2d::StaticInitialize(dxCommon->GetDevice()))
	{
		assert(0);
	}

	// 3D�I�u�W�F�N�g�ÓI������
	if (!Object3d::StaticInitialize(dxCommon->GetDevice(), camera))
	{
		assert(0);
	}
	// ���C�g�ÓI������
	if (!Light::StaticInitialize(dxCommon->GetDevice()))
	{
		assert(0);
	}

	// �Q�[���V�[���̏�����
	gameScene = new GameScene();
	gameScene->Initialize(dxCommon, audio);
}

void MyGame::Finalize()
{
	// �I�����ɃJ�[�\���ړ��̐���������
	ClipCursor(NULL);
	//���
	safe_delete(gameScene);
	safe_delete(audio);
	safe_delete(image2d);
	safe_delete(light);
	safe_delete(dxCommon);

	// �Q�[���E�B���h�E�̔j��
	win->TerminateGameWindow();
	safe_delete(win);
}

void MyGame::Update()
{
	if (GetActiveWindow())
	{
		// ���͊֘A�̖��t���[������
		keyboard->Update();
		controller->Update();
		mouse->Update();
		// �Q�[���V�[���̖��t���[������
		gameScene->Update();
	}
}

void MyGame::Draw()
{
	//// �w�i�摜�`��O����
	//Image2d::PreDraw(dxCommon->GetCommandList());
	//// �w�i�摜�`��
	//image2d->Draw();
	//// �摜�`��㏈��
	//Image2d::PostDraw();

	// �`��J�n
	dxCommon->PreDraw();
	// �Q�[���V�[���̕`��
	gameScene->Draw();
	// �`��I��
	dxCommon->PostDraw();

	////�����_�[�e�N�X�`���ւ̕`��J�n
	//multiTex->PreDraw(dxCommon->GetCommandList());
	//// �|�X�g�G�t�F�N�g�̕`��
	//multiTex->Draw(dxCommon->GetCommandList());
	////�����_�[�e�N�X�`���ւ̕`��I��
	//multiTex->PostDraw(dxCommon->GetCommandList());
}

bool MyGame::IsEndRequest()
{
	// ���b�Z�[�W����
	if (win->ProcessMessage() || keyboard->PushKey(DIK_ESCAPE))
	{
		return true;
	}
	return false;
}

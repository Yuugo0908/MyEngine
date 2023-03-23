#include "Framework.h"

void Framework::Initialize()
{
	// �Q�[���E�B���h�E�̍쐬
	win = new WinApp();
	win->CreateGameWindow();

	//DirectX����������
	dxCommon = DirectXCommon::GetInstance();
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
}

void Framework::Finalize()
{
	// �I�����ɃJ�[�\���ړ��̐���������
	ClipCursor(NULL);
	//���
	safe_delete(audio);
	safe_delete(image2d);
	safe_delete(light);

	// �Q�[���E�B���h�E�̔j��
	win->TerminateGameWindow();
	safe_delete(win);
}

void Framework::Update()
{
	if (GetActiveWindow())
	{
		// ���b�Z�[�W����
		if (win->ProcessMessage() || keyboard->PushKey(DIK_ESCAPE))
		{
			// �Q�[�����[�v�𔲂���
			endRequest_ = true;
			return;
		}

		// ���͊֘A�̖��t���[������
		keyboard->Update();
		controller->Update();
		mouse->Update();
		// �Q�[���V�[���̖��t���[������
		SceneManager::GetInstance()->Update();
	}
}

void Framework::Draw()
{
	// �`��J�n
	dxCommon->PreDraw();
	// �Q�[���V�[���̕`��
	SceneManager::GetInstance()->Draw();
	// �`��I��
	dxCommon->PostDraw();
}

void Framework::Run()
{
	// �Q�[���̏�����
	Initialize();

	while (true)
	{
		// ���t���[���X�V
		Update();

		if (IsEndRequest())
		{
			break;
		}

		Draw();
	}

	Finalize();
}

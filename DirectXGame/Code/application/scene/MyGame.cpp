#include "MyGame.h"
#include "SceneFactory.h"

void MyGame::Initialize()
{
	//���N���X�̏�����
	Framework::Initialize();

	// �^�C�g���摜�ǂݍ���
	if (!Image2d::LoadTexture(Image2d::ImgNumber::titleNum, "title"))
	{
		assert(0);
	}
	// �w�i�摜�ǂݍ���
	if (!Image2d::LoadTexture(Image2d::ImgNumber::backNum, "backGround"))
	{
		assert(0);
	}
	// �Q�[���N���A�摜�ǂݍ���
	if (!Image2d::LoadTexture(Image2d::ImgNumber::GameClearNum, "GameClear"))
	{
		assert(0);
	}
	// �Q�[���I�[�o�[�摜�ǂݍ���
	if (!Image2d::LoadTexture(Image2d::ImgNumber::GameOverNum, "GameOver"))
	{
		assert(0);
	}
	// �v���C���[HP�\���p�摜�̓ǂݍ���
	if (!Image2d::LoadTexture(Image2d::ImgNumber::HPTextNum, "HPText"))
	{
		assert(0);
	}
	if (!Image2d::LoadTexture(Image2d::ImgNumber::HPBarNum, "HPBar"))
	{
		assert(0);
	}
	if (!Image2d::LoadTexture(Image2d::ImgNumber::HPGaugeNum, "HPGauge"))
	{
		assert(0);
	}
	// �`���[�g���A���p�摜�̓ǂݍ���
	if (!Image2d::LoadTexture(Image2d::ImgNumber::wasdNum, "wasdKey"))
	{
		assert(0);
	}
	if (!Image2d::LoadTexture(Image2d::ImgNumber::spaceNum, "spaceKey"))
	{
		assert(0);
	}
	if (!Image2d::LoadTexture(Image2d::ImgNumber::mouseNum, "mouse"))
	{
		assert(0);
	}
	if (!Image2d::LoadTexture(Image2d::ImgNumber::mouseLeftNum, "mouse_left"))
	{
		assert(0);
	}
	if (!Image2d::LoadTexture(Image2d::ImgNumber::mouseRightNum, "mouse_right"))
	{
		assert(0);
	}

	// �t�F�[�h�̏�����
	FadeScene::GetInstance()->Initialize();

	// �V�[���t�@�N�g���[�𐶐����A�}�l�[�W���[�ɃZ�b�g
	sceneFactory_ = new SceneFactory();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_);

	SceneManager::GetInstance()->ChangeScene("Title");
}

void MyGame::Finalize()
{
	// ���N���X�̏I��
	Framework::Finalize();
}

void MyGame::Update()
{
	//���N���X�̃A�b�v�f�[�g
	Framework::Update();
}

void MyGame::Draw()
{
	//���N���X�̃A�b�v�f�[�g
	Framework::Draw();
}

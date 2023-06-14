#include "MyGame.h"
#include "SceneFactory.h"

void MyGame::Initialize()
{
	//基底クラスの初期化
	Framework::Initialize();

	// タイトル画像読み込み
	if (!Image2d::LoadTexture(Image2d::ImgNumber::titleNum, "title"))
	{
		assert(0);
	}
	// 背景画像読み込み
	if (!Image2d::LoadTexture(Image2d::ImgNumber::backNum, "backGround"))
	{
		assert(0);
	}
	// ゲームクリア画像読み込み
	if (!Image2d::LoadTexture(Image2d::ImgNumber::GameClearNum, "GameClear"))
	{
		assert(0);
	}
	// ゲームオーバー画像読み込み
	if (!Image2d::LoadTexture(Image2d::ImgNumber::GameOverNum, "GameOver"))
	{
		assert(0);
	}
	// プレイヤーHP表示用画像の読み込み
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
	// チュートリアル用画像の読み込み
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

	// フェードの初期化
	FadeScene::GetInstance()->Initialize();

	// シーンファクトリーを生成し、マネージャーにセット
	sceneFactory_ = new SceneFactory();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_);

	SceneManager::GetInstance()->ChangeScene("Title");
}

void MyGame::Finalize()
{
	// 基底クラスの終了
	Framework::Finalize();
}

void MyGame::Update()
{
	//基底クラスのアップデート
	Framework::Update();
}

void MyGame::Draw()
{
	//基底クラスのアップデート
	Framework::Draw();
}

#include "MyGame.h"

void MyGame::Initialize()
{
	//基底クラスの初期化
	Framework::Initialize();
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

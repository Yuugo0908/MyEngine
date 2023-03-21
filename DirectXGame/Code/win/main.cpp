#include "MyGame.h"

//# Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	MyGame game;
	// ゲームの初期化
	game.Initialize();

	while (true)
	{
		// 毎フレーム更新
		game.Update();

		if (game.IsEndRequest())
		{
			break;
		}

		game.Draw();
	}

	game.Finalize();

	return 0;
}
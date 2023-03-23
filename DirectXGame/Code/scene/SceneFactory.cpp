#include "SceneFactory.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "GameClearScene.h"
#include "GameOverScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
    // éüÇÃÉVÅ[ÉìÇê∂ê¨
    BaseScene* newScene = nullptr;

    if (sceneName == "Title")
    {
        newScene = new TitleScene();
    }
    else if (sceneName == "Game")
    {
        newScene = new GameScene();
    }
    else if (sceneName == "GameClear")
    {
        newScene = new GameClearScene();
    }
    else if (sceneName == "GameOver")
    {
        newScene = new GameOverScene();
    }

    return newScene;
}

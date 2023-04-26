#include "SceneFactory.h"
#include "TitleScene.h"
#include "tutorialScene.h"
#include "GameScene.h"
#include "GameClearScene.h"
#include "GameOverScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
    // ���̃V�[���𐶐�
    BaseScene* newScene = nullptr;

    if (sceneName == "Title")
    {
        newScene = new TitleScene();
    }
    else if (sceneName == "Tutorial")
    {
        // �N�����Ă����x�ł��`���[�g���A�����s���Ă����ꍇ�A�Q�[���V�[���ֈڍs����
        if (tutorialFlag)
        {
            newScene = new GameScene();
            return newScene;
        }

        tutorialFlag = true;
        newScene = new TutorialScene();
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

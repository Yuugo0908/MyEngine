#include "DebugText.h"

DebugText::DebugText()
{
}

DebugText::~DebugText()
{
	for (int i = 0; i < _countof(image2dDatas); i++)
	{
		delete image2dDatas[i];
	}
}

void DebugText::Initialize(UINT texnumber)
{
	// 全ての画像データについて
	for (int i = 0; i < _countof(image2dDatas); i++)
	{
		// 画像を生成する
		image2dDatas[i] = Image2d::Create(texnumber, { 0,0 });
	}
}

// 1文字列追加
void DebugText::Print(const std::string& text, float x, float y, float scale = 1.0f)
{
	// 全ての文字について
	for (int i = 0; i < text.size(); i++)
	{
		// 最大文字数超過
		if (image2dIndex >= maxCharCount)
		{
			break;
		}

		// 1文字取り出す(※ASCIIコードでしか成り立たない)
		const unsigned char& character = text[i];

		int fontIndex = character - 32;
		if (character >= 0x7f)
		{
			fontIndex = 0;
		}

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;

		// 座標計算
		image2dDatas[image2dIndex]->SetPosition({ x + fontWidth * scale * i, y });
		image2dDatas[image2dIndex]->SetTextureRect({ (float)fontIndexX * fontWidth, (float)fontIndexY * fontHeight }, { (float)fontWidth, (float)fontHeight });
		image2dDatas[image2dIndex]->SetSize({ fontWidth * scale, fontHeight * scale });

		// 文字を１つ進める
		image2dIndex++;
	}
}

// まとめて描画
void DebugText::DrawAll(ID3D12GraphicsCommandList* cmdList)
{
	// 全ての文字の画像について
	for (int i = 0; i < image2dIndex; i++)
	{
		// 画像描画
		image2dDatas[i]->Draw();
	}

	image2dIndex = 0;
}
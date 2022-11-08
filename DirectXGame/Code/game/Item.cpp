#include "Item.h"

Item::Item()
{

}

Item::~Item()
{

}

Item* Item::GetInstance()
{
	static Item instance;

	return &instance;
}

bool Item::Initialize()
{
	itemModel = itemModel->CreateFromObject("sphere");
	itemObj = Object3d::Create();
	itemObj->SetModel(itemModel);

	itemObj->SetPosition({ 0.0f, 1.0f, 0.0f });
	itemObj->SetScale({ 0.5, 0.5, 0.5 });
	itemObj->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f });
	itemPos = itemObj->GetPosition();
	return true;
}

void Item::Update()
{
	itemObj->Update();
}

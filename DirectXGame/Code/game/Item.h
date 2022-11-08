#pragma once
#include "Object3d.h"
#include "Keyboard.h"
#include "Easing.h"
#include "Collision.h"
#include "Operator.h"
#include "Player.h"

class Item final
{
private:
	Item();

	~Item();
public:
	// �R�s�[�R���X�g���N�^�𖳌���
	Item(const Item& obj) = delete;
	// ������Z�q�𖳌���
	Item& operator=(const Item& obj) = delete;

public: // �����o�֐�

	static Item* GetInstance();

	bool Initialize();

	void Update();

	void Draw() { itemObj->Draw(); }

	// �I�u�W�F�N�g
	const std::unique_ptr<Object3d>& GetObj() { return itemObj; }

	// �ʒu
	const XMFLOAT3& GetPos() { return itemPos; }
	void SetPos(XMFLOAT3 iPos) { this->itemPos = iPos; }

private: // �����o�ϐ�

	// ���f��
	Model* itemModel = nullptr;
	std::unique_ptr<Object3d> itemObj = nullptr;

	// �v���C���[
	XMFLOAT3 itemPos = {};//���W
};
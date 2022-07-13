#include "Collision.h"

Collision* Collision::GetInstance()
{
	static Collision instance;

	return &instance;
}

float Collision::GetLength(XMFLOAT3 pos_a, XMFLOAT3 pos_b)
{
	XMFLOAT3 len = { pos_a.x - pos_b.x, pos_a.y - pos_b.y, pos_a.z - pos_b.z };

	return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
}

bool Collision::CollisionObject(const std::unique_ptr<Object3d>& object_a, const std::unique_ptr<Object3d>& object_b)
{
	//ç¿ïW
	XMFLOAT3 A = object_a->GetPosition();
	XMFLOAT3 B = object_b->GetPosition();

	//îºåa
	float a_r = 1.0f * object_a->GetScale().x;
	float b_r = 1.0f * object_a->GetScale().x;

	//í∑Ç≥
	float l_x = sqrtf(powf(A.x - B.x, 2));
	float l_y = sqrtf(powf(A.y - B.y, 2));

	//îºåaÇÃçáåvÇÊÇËíZÇØÇÍÇŒ
	if (l_x < a_r + b_r && l_y < a_r + b_r)
	{
		return true;
	}

	return false;
}

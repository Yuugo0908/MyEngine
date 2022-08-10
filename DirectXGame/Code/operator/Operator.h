#pragma once
#include <DirectXMath.h>

using namespace DirectX;
class Operator : public XMFLOAT3
{
public:
	//�R���X�g���N�^
	Operator();
	Operator(float x, float y, float z);

	//�����o�֐�
	float length();
	XMFLOAT3 normalize();
	static float dot(const XMFLOAT3& v1, const XMFLOAT3& v2);
	XMFLOAT3 cross(const XMFLOAT3& v);

	//�P�����Z�q�I�[�o�[���[�h
	Operator operator+();
	Operator operator-();

	//������Z�q�I�[�o�[���[�h
	Operator& operator+=(const Operator& v);
	Operator& operator-=(const Operator& v);
	Operator& operator*=(float s);
	Operator& operator/=(float s);
};

//2�����Z�q�I�[�o�[���[�h
//�������Ȉ���(�����̌^�Ə���)�̃p�^�[���ɑΉ����邽�߁A�ȉ��̂悤�ɏ������Ă���
const XMFLOAT3 operator+(const XMFLOAT3& v1, const XMFLOAT3& v2);
const XMFLOAT3 operator+(const XMFLOAT3& v, float s);
const XMFLOAT3 operator-(const XMFLOAT3& v1, const XMFLOAT3& v2);
const XMFLOAT3 operator-(const XMFLOAT3& v, float s);
const XMFLOAT3 operator*(const XMFLOAT3& v1, const XMFLOAT3& v2);
const XMFLOAT3 operator*(const XMFLOAT3& v, float s);
const XMFLOAT3 operator/(const XMFLOAT3& v1, const XMFLOAT3& v2);
const XMFLOAT3 operator/(const XMFLOAT3& v, float s);

#include "Operator.h"

Operator::Operator() : Operator(0, 0, 0) {}

Operator::Operator(float x, float y, float z) : XMFLOAT3{ x,y,z } {}

float Operator::length()
{
	return sqrt(x * x + y * y + z * z);
}

XMFLOAT3 Operator::normalize()
{
	float len = length();
	if (len != 0) {
		return *this /= len;
	}
	return *this;
}

float Operator::dot(const XMFLOAT3& v1, const XMFLOAT3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

XMFLOAT3 Operator::cross(const XMFLOAT3& v)
{
	XMFLOAT3 temp;
	temp.x = this->y * v.z - this->z * v.y;
	temp.y = this->z * v.x - this->x * v.z;
	temp.z = this->x * v.y - this->y * v.x;
	return temp;
}

XMFLOAT3 Operator::cross3D(const XMFLOAT3& v1, XMFLOAT3& v2)
{
	XMFLOAT3 temp = {};
	temp.x = v1.y * v2.z - v1.z * v2.y;
	temp.y = v1.z * v2.x - v1.x * v2.z;
	temp.z = v1.x * v2.y - v1.y * v2.x;
	return temp;
}

XMFLOAT3 Operator::Degrees(XMFLOAT3 Radians)
{
	XMFLOAT3 resultRad = Radians;
	resultRad.x * (180.0 / XM_PI);
	resultRad.y * (180.0 / XM_PI);
	resultRad.z * (180.0 / XM_PI);
	return resultRad;
}

Operator Operator::operator+()
{
	return *this;
}

Operator Operator::operator-()
{
	return Operator(-x, -y, -z);
}

Operator& Operator::operator+=(const Operator& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Operator& Operator::operator-=(const Operator& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Operator& Operator::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

Operator& Operator::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	return *this;
}

const XMFLOAT3 operator+(const XMFLOAT3& v1, const XMFLOAT3& v2)
{
	XMFLOAT3 w = {};
	w.x = v1.x + v2.x;
	w.y = v1.y + v2.y;
	w.z = v1.z + v2.z;
	return w;
}

const XMFLOAT3 operator+(const XMFLOAT3& v1, float s)
{
	XMFLOAT3 w = {};
	w.x = v1.x + s;
	w.y = v1.y + s;
	w.z = v1.z + s;
	return w;
}

const XMFLOAT3 operator-(const XMFLOAT3& v1, const XMFLOAT3& v2)
{
	XMFLOAT3 w = {};
	w.x = v1.x - v2.x;
	w.y = v1.y - v2.y;
	w.z = v1.z - v2.z;
	return w;
}

const XMFLOAT3 operator-(const XMFLOAT3& v1, float s)
{
	XMFLOAT3 w = {};
	w.x = v1.x - s;
	w.y = v1.y - s;
	w.z = v1.z - s;
	return w;
}

const XMFLOAT3 operator*(const XMFLOAT3& v1, const XMFLOAT3& v2)
{
	XMFLOAT3 w = {};
	w.x = v1.x * v2.x;
	w.y = v1.y * v2.y;
	w.z = v1.z * v2.z;
	return w;
}

const XMFLOAT3 operator*(const XMFLOAT3& v, float s)
{
	XMFLOAT3 w = {};
	w.x = v.x * s;
	w.y = v.y * s;
	w.z = v.z * s;
	return w;
}

const XMFLOAT3 operator/(const XMFLOAT3& v1, const XMFLOAT3& v2)
{
	XMFLOAT3 w = {};
	w.x = v1.x / v2.x;
	w.y = v1.y / v2.y;
	w.z = v1.z / v2.z;
	return w;
}

const XMFLOAT3 operator*(float s, const XMFLOAT3& v)
{
	XMFLOAT3 w = {};
	w.x = v.x * s;
	w.y = v.y * s;
	w.z = v.z * s;
	return w;
}

const XMFLOAT3 operator/(const XMFLOAT3& v, float s)
{
	XMFLOAT3 w = {};
	w.x = v.x / s;
	w.y = v.y / s;
	w.z = v.z / s;
	return w;
}
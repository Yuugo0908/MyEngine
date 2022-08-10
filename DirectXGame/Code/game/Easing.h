#pragma once
#include <DirectXMath.h>
using namespace DirectX;
class Easing
{
public:
	static XMFLOAT3 lerp		(const XMFLOAT3& start, const XMFLOAT3& end, const float time);
	static XMFLOAT3 easeIn		(const XMFLOAT3& start, const XMFLOAT3& end, const float time);
	static XMFLOAT3 easeOut		(const XMFLOAT3& start, const XMFLOAT3& end, const float time);
	static XMFLOAT3 easeInOut	(const XMFLOAT3& start, const XMFLOAT3& end, const float time);
};

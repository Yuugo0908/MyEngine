#include "Easing.h"
#include "Operator.h"
#include <cmath>

XMFLOAT3 Easing::lerp(const XMFLOAT3& start, const XMFLOAT3& end, const float time)
{
	return start * (1.0f - time) + end * time;
}

XMFLOAT3 Easing::easeIn(const XMFLOAT3& start, const XMFLOAT3& end, const float time)
{
	float t = time * time;
	return start * (1.0f - t) + end * t;
}

XMFLOAT3 Easing::easeOut(const XMFLOAT3& start, const XMFLOAT3& end, const float time)
{
	float y = time * (2 - time);
	return start * (1.0f - y) + end * y;
}

XMFLOAT3 Easing::easeInOut(const XMFLOAT3& start, const XMFLOAT3& end, const float time)
{
	float y = time * time * (3 - 2 * time);
	return start * (1.0f - y) + end * y;
}
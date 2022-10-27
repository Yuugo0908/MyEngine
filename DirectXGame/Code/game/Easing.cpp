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
	float t = time * (2 - time);
	return start * (1.0f - t) + end * t;
}

XMFLOAT3 Easing::easeInOut(const XMFLOAT3& start, const XMFLOAT3& end, const float time)
{
	float t = time * time * (3 - 2 * time);
	return start * (1.0f - t) + end * t;
}

void Easing::EaseInUpdate(const XMFLOAT3 startPos, const XMFLOAT3 endPos, XMFLOAT3& reflectPos, bool& flag, float& avoidTime)
{
	// フラグがtrueじゃない場合はリターンする
	if (!flag)
	{
		avoidTime = 0.0f;
		return;
	}

	if (avoidTime < 1.0f)
	{
		avoidTime += 0.1f;
	}
	else
	{
		flag = false;
		return;
	}

	reflectPos = easeIn(startPos, endPos, avoidTime);
}

void Easing::EaseOutUpdate(const XMFLOAT3 startPos, const XMFLOAT3 endPos, XMFLOAT3& reflectPos, bool& flag, float& avoidTime)
{
	// フラグがtrueじゃない場合はリターンする
	if (!flag)
	{
		avoidTime = 0.0f;
		return;
	}

	if (avoidTime < 1.0f)
	{
		avoidTime += 0.05f;
	}
	else
	{
		flag = false;
		return;
	}

	reflectPos = easeOut(startPos, endPos, avoidTime);
}

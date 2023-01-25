#pragma once
#include "Operator.h"
#include <stdlib.h>
#include <time.h>
class Easing
{
public:

	static Easing* GetInstance();

	static XMFLOAT3 lerp		(const XMFLOAT3& start, const XMFLOAT3& end, const float time);
	static XMFLOAT3 easeIn		(const XMFLOAT3& start, const XMFLOAT3& end, const float time);
	static XMFLOAT3 easeOut		(const XMFLOAT3& start, const XMFLOAT3& end, const float time);
	static XMFLOAT3 easeInOut	(const XMFLOAT3& start, const XMFLOAT3& end, const float time);

	// イージングの更新
	void EaseInUpdate(const XMFLOAT3 startPos, const XMFLOAT3 endPos, XMFLOAT3& reflectPos, bool& flag, float& avoidTime);
	void EaseOutUpdate(const XMFLOAT3 startPos, const XMFLOAT3 endPos, XMFLOAT3& reflectPos, bool& flag, float& avoidTime);
};

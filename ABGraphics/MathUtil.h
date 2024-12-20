#pragma once

constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;

template <typename T>
constexpr auto sq(const T& x) noexcept
{
	return x * x;
}

template<typename T>
T wrap_angle(T theta) noexcept
{
	constexpr T twoPi = (T)2 * (T)PI_D;
	const T mod = fmod(theta, twoPi);
	if (mod > (T)PI_D)
	{
		return mod - twoPi;
	}
	else if (mod < (T)PI_D)
	{
		return mod + twoPi;
	}
	return mod;
}

template<typename T>
constexpr T interpolate(const T& src, const T& dst, float alpha) noexcept
{
	return src + (dst - src) * alpha;
}

template<typename T>
constexpr T to_rad(T deg) noexcept
{
	return deg * PI / (T)180.0;
}

template<typename T>
constexpr T gauss(T x, T sigma) noexcept
{
	const auto ss = sq(sigma);
	return ((T)1.0 / sqrt((T)2.0 * (T)PI_D * ss)) * exp(-sq(x) / ((T)2.0 * ss));
}

class MathHelper
{
public:
	static float AngleFromXY(float x, float y);
	static float RandF();
	static float RandF(float a, float b);
	static Matrix InverseTranspose(const Matrix& M);
	static Vector4 RandHemisphereUnitVec3(Vector4 n);

	template<typename T>
	static T Max(const T& a, const T& b);
	template <typename T>
	static T Clamp(const T& x, const T& low, const T& high);
	template<typename T>
	static T Min(const T& a, const T& b);

public:
	static constexpr float Pi = 3.1415926535f;
	static constexpr float Infinity = FLT_MAX;
};

template<typename T>
T MathHelper::Max(const T& a, const T& b)
{
	return a > b ? a : b;
}

template<typename T>
T MathHelper::Min(const T& a, const T& b)
{
	return a < b ? a : b;
}

template <typename T>
T MathHelper::Clamp(const T& x, const T& low, const T& high)
{
	if (x < low)
	{
		return low;
	}
	else if (x > high)
	{
		return high;
	}

	return x;
}
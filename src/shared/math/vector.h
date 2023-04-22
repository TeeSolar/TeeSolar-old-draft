/*
 * Copyright (C) 2016 necropotame (necropotame@gmail.com)
 * 
 * This file is part of TeeUniverse.
 * 
 * TeeUniverse is free software: you can redistribute it and/or  modify
 * it under the terms of the GNU Affero General Public License, version 3,
 * as published by the Free Software Foundation.
 *
 * TeeUniverse is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with TeeUniverse.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Some parts of this file comes from other projects.
 * These parts are itendified in this file by the following block:
 * 
 * FOREIGN CODE BEGIN: ProjectName *************************************
 * 
 * FOREIGN CODE END: ProjectName ***************************************
 * 
 * If ProjectName is "TeeWorlds", then this part of the code follows the
 * TeeWorlds licence:
 *      (c) Magnus Auvinen. See LICENSE_TEEWORLDS in the root of the
 *      distribution for more information. If you are missing that file,
 *      acquire a complete release at teeworlds.com.
 */

#ifndef __SHARED_MATH_VECTOR__
#define __SHARED_MATH_VECTOR__

#include <math.h>

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

#include "math.h"	// mix

// ------------------------------------

template<typename T>
class vector2_base
{
public:
	union
	{
		T x, u;
	};
	union
	{
		T y, v;
	};

	constexpr vector2_base() :
		x(T()), y(T())
	{
	}

	constexpr vector2_base(T nx, T ny) :
		x(nx), y(ny)
	{
	}
	
	constexpr vector2_base(T n) :
		x(n), y(n)
	{		
	}

	vector2_base operator -() const { return vector2_base(-x, -y); }
	vector2_base operator -(const vector2_base &v) const { return vector2_base(x-v.x, y-v.y); }
	vector2_base operator +(const vector2_base &v) const { return vector2_base(x+v.x, y+v.y); }
	vector2_base operator *(const T v) const { return vector2_base(x*v, y*v); }
	vector2_base operator *(const vector2_base &v) const { return vector2_base(x*v.x, y*v.y); }
	vector2_base operator /(const T v) const { return vector2_base(x/v, y/v); }
	vector2_base operator /(const vector2_base &v) const { return vector2_base(x/v.x, y/v.y); }

	const vector2_base &operator =(const vector2_base &v) { x = v.x; y = v.y; return *this; }

	const vector2_base &operator +=(const vector2_base &v) { x += v.x; y += v.y; return *this; }
	const vector2_base &operator -=(const vector2_base &v) { x -= v.x; y -= v.y; return *this; }
	const vector2_base &operator *=(const T v) { x *= v; y *= v; return *this;	}
	const vector2_base &operator *=(const vector2_base &v) { x *= v.x; y *= v.y; return *this; }
	const vector2_base &operator /=(const T v) { x /= v; y /= v; return *this;	}
	const vector2_base &operator /=(const vector2_base &v) { x /= v.x; y /= v.y; return *this; }

	bool operator ==(const vector2_base &v) const { return x == v.x && y == v.y; } //TODO: do this with an eps instead
	bool operator !=(const vector2_base &v) const { return x != v.x || y != v.y; }

	operator const T* () { return &x; }
};

template<typename T>
inline vector2_base<T> rotate(const vector2_base<T> &a, float angle)
{
	angle = angle * Pi / 180.0f;
	float s = std::sin(angle);
	float c = std::cos(angle);
	return vector2_base<T>((T)(c * a.x - s * a.y), (T)(s * a.x + c * a.y));
}

template<typename T>
inline T distance(const vector2_base<T> a, const vector2_base<T> &b)
{
	return length(a - b);
}

template<typename T>
constexpr inline T dot(const vector2_base<T> a, const vector2_base<T> &b)
{
	return a.x * b.x + a.y * b.y;
}

inline float length(const vector2_base<float> &a)
{
	return std::sqrt(dot(a, a));
}

inline float length_squared(const vector2_base<float> &a)
{
	return dot(a, a);
}

inline float angle(const vector2_base<float> &a)
{
	return atan2f(a.y, a.x);
}

inline float angle(const vector2_base<float> &a, const vector2_base<float> &b)
{
	float w = (a.x * b.y - b.x * a.y)/(length(a) * length(b));
	float d = dot(a, b);
	
	if(d >= 0.0f)
		return asinf(w);
	else
	{
		float a = Pi - asinf(w);
		return (a > Pi ? a - 2.0f*Pi : a);
	}
}

template<typename T>
constexpr inline vector2_base<T> normalize_pre_length(const vector2_base<T> &v, T len)
{
	if(len == 0)
		return vector2_base<T>();
	return vector2_base<T>(v.x / len, v.y / len);
}

inline vector2_base<float> normalize(const vector2_base<float> &v)
{
	float divisor = length(v);
	if(divisor == 0.0f)
		return vector2_base<float>(0.0f, 0.0f);
	float l = (float)(1.0f / divisor);
	return vector2_base<float>(v.x * l, v.y * l);
}

inline vector2_base<float> direction(float angle)
{
	return vector2_base<float>(std::cos(angle), std::sin(angle));
}

inline vector2_base<float> random_direction()
{
	return direction(random_angle());
}

typedef vector2_base<float> vec2;
typedef vector2_base<bool> bvec2;
typedef vector2_base<int> ivec2;

template<typename T>
constexpr inline bool closest_point_on_line(vector2_base<T> line_pointA, vector2_base<T> line_pointB, vector2_base<T> target_point, vector2_base<T> &out_pos)
{
	vector2_base<T> AB = line_pointB - line_pointA;
	T SquaredMagnitudeAB = dot(AB, AB);
	if(SquaredMagnitudeAB > 0)
	{
		vector2_base<T> AP = target_point - line_pointA;
		T APdotAB = dot(AP, AB);
		T t = APdotAB / SquaredMagnitudeAB;
		out_pos = line_pointA + AB * clamp(t, (T)0, (T)1);
		return true;
	}
	else
		return false;
}

template<typename T>
inline vector2_base<T> ortho(const vector2_base<T> &a)
{
	return vector2_base<T>(a.y, -a.x);
}

// ------------------------------------
template<typename T>
class vector3_base
{
public:
	union
	{
		T x, r, h, u;
	};
	union
	{
		T y, g, s, v;
	};
	union
	{
		T z, b, l, w;
	};

	constexpr vector3_base() :
		x(T()), y(T()), z(T())
	{
	}

	constexpr vector3_base(T nx, T ny, T nz) :
		x(nx), y(ny), z(nz)
	{
	}

	vector3_base operator-(const vector3_base &vec) const { return vector3_base(x - vec.x, y - vec.y, z - vec.z); }
	vector3_base operator-() const { return vector3_base(-x, -y, -z); }
	vector3_base operator+(const vector3_base &vec) const { return vector3_base(x + vec.x, y + vec.y, z + vec.z); }
	vector3_base operator*(const T rhs) const { return vector3_base(x * rhs, y * rhs, z * rhs); }
	vector3_base operator*(const vector3_base &vec) const { return vector3_base(x * vec.x, y * vec.y, z * vec.z); }
	vector3_base operator/(const T rhs) const { return vector3_base(x / rhs, y / rhs, z / rhs); }
	vector3_base operator/(const vector3_base &vec) const { return vector3_base(x / vec.x, y / vec.y, z / vec.z); }

	const vector3_base &operator+=(const vector3_base &vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
		return *this;
	}
	const vector3_base &operator-=(const vector3_base &vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		return *this;
	}
	const vector3_base &operator*=(const T rhs)
	{
		x *= rhs;
		y *= rhs;
		z *= rhs;
		return *this;
	}
	const vector3_base &operator*=(const vector3_base &vec)
	{
		x *= vec.x;
		y *= vec.y;
		z *= vec.z;
		return *this;
	}
	const vector3_base &operator/=(const T rhs)
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}
	const vector3_base &operator/=(const vector3_base &vec)
	{
		x /= vec.x;
		y /= vec.y;
		z /= vec.z;
		return *this;
	}

	bool operator==(const vector3_base &vec) const { return x == vec.x && y == vec.y && z == vec.z; } //TODO: do this with an eps instead
	bool operator!=(const vector3_base &vec) const { return x != vec.x || y != vec.y || z != vec.z; }
};

template<typename T>
inline T distance(const vector3_base<T> &a, const vector3_base<T> &b)
{
	return length(a - b);
}

template<typename T>
constexpr inline T dot(const vector3_base<T> &a, const vector3_base<T> &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

template<typename T>
constexpr inline vector3_base<T> cross(const vector3_base<T> &a, const vector3_base<T> &b)
{
	return vector3_base<T>(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x);
}

//
inline float length(const vector3_base<float> &a)
{
	return std::sqrt(dot(a, a));
}

inline vector3_base<float> normalize(const vector3_base<float> &v)
{
	float divisor = length(v);
	if(divisor == 0.0f)
		return vector3_base<float>(0.0f, 0.0f, 0.0f);
	float l = (float)(1.0f / divisor);
	return vector3_base<float>(v.x * l, v.y * l, v.z * l);
}

typedef vector3_base<float> vec3;
typedef vector3_base<bool> bvec3;
typedef vector3_base<int> ivec3;

// ------------------------------------

template<typename T>
class vector4_base
{
public:
	union
	{
		T x, r, h;
	};
	union
	{
		T y, g, s;
	};
	union
	{
		T z, b, l;
	};
	union
	{
		T w, a;
	};

	constexpr vector4_base() :
		x(T()), y(T()), z(T()), w(T())
	{
	}

	constexpr vector4_base(T nx, T ny, T nz, T nw) :
		x(nx), y(ny), z(nz), w(nw)
	{
	}

	constexpr vector4_base(T n) :
		x(n), y(n), z(n), w(n)
	{
	}

	vector4_base operator+(const vector4_base &vec) const { return vector4_base(x + vec.x, y + vec.y, z + vec.z, w + vec.w); }
	vector4_base operator-(const vector4_base &vec) const { return vector4_base(x - vec.x, y - vec.y, z - vec.z, w - vec.w); }
	vector4_base operator-() const { return vector4_base(-x, -y, -z, -w); }
	vector4_base operator*(const vector4_base &vec) const { return vector4_base(x * vec.x, y * vec.y, z * vec.z, w * vec.w); }
	vector4_base operator*(const T rhs) const { return vector4_base(x * rhs, y * rhs, z * rhs, w * rhs); }
	vector4_base operator/(const vector4_base &vec) const { return vector4_base(x / vec.x, y / vec.y, z / vec.z, w / vec.w); }
	vector4_base operator/(const T vec) const { return vector4_base(x / vec, y / vec, z / vec, w / vec); }

	const vector4_base &operator+=(const vector4_base &vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
		w += vec.w;
		return *this;
	}
	const vector4_base &operator-=(const vector4_base &vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		w -= vec.w;
		return *this;
	}
	const vector4_base &operator*=(const T rhs)
	{
		x *= rhs;
		y *= rhs;
		z *= rhs;
		w *= rhs;
		return *this;
	}
	const vector4_base &operator*=(const vector4_base &vec)
	{
		x *= vec.x;
		y *= vec.y;
		z *= vec.z;
		w *= vec.w;
		return *this;
	}
	const vector4_base &operator/=(const T rhs)
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
		w /= rhs;
		return *this;
	}
	const vector4_base &operator/=(const vector4_base &vec)
	{
		x /= vec.x;
		y /= vec.y;
		z /= vec.z;
		w /= vec.w;
		return *this;
	}

	bool operator==(const vector4_base &vec) const { return x == vec.x && y == vec.y && z == vec.z && w == vec.w; } //TODO: do this with an eps instead
	bool operator!=(const vector4_base &vec) const { return x != vec.x || y != vec.y || z != vec.z || w != vec.w; }
};

typedef vector4_base<float> vec4;
typedef vector4_base<bool> bvec4;
typedef vector4_base<int> ivec4;

/* FOREIGN CODE END: TeeWorlds ****************************************/

template<typename T>
class matrix2x2_base
{
public:
	float v[4];

	matrix2x2_base()
	{
		v[0] = 1;
		v[1] = 0;
		v[2] = 0;
		v[3] = 1;
	}
	
	matrix2x2_base(T a, T b, T c, T d)
	{
		v[0] = a;
		v[1] = b;
		v[2] = c;
		v[3] = d;
	}

	matrix2x2_base<T> operator*(const matrix2x2_base<T> &m) const
	{
		return matrix2x2_base(
			v[0]*m.v[0] + v[1]*m.v[2],
			v[0]*m.v[1] + v[1]*m.v[3],
			v[2]*m.v[0] + v[3]*m.v[2],
			v[2]*m.v[1] + v[3]*m.v[3]
		);
	}

	vector2_base<T> operator*(const vector2_base<T> &a) const
	{
		return vector2_base<T>(
			v[0]*a.x + v[1]*a.y,
			v[2]*a.x + v[3]*a.y
		);
	}
	
	T operator[](int i) const
	{
		return v[i];
	}
	
	static matrix2x2_base<T> identity()
	{
		return matrix2x2_base<T>(1, 0, 0, 1);
	}
	
	static matrix2x2_base<T> rotation(T angle)
	{
		return matrix2x2_base<T>(
			cosf(angle), -sinf(angle),
			sinf(angle), cosf(angle)
		);
	}
	
	static matrix2x2_base<T> scaling(vector2_base<T> scale)
	{
		return matrix2x2_base<T>(
			scale.x, 0, 0, scale.y
		);
	}
	
	static matrix2x2_base<T> inverse(matrix2x2_base<T> m)
	{
		T det = m.v[0] * m.v[3] - m.v[1] * m.v[2];
		return matrix2x2_base<T>(m.v[3]/det, -m.v[1]/det, -m.v[2]/det, m.v[0]/det);
	}
};

typedef matrix2x2_base<float> matrix2x2;

#endif

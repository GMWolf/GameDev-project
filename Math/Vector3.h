#pragma once
#include "Swizzle.h"
class Vector3 {

public:

	Vector3() {
	}

	Vector3(float x, float y, float z): x(x), y(y), z(z) {
	}


#pragma region arithmetic operators
	inline Vector3 operator+(const Vector3 &rhs) const {
		return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	inline Vector3 operator+(const float rhs) const {
		return Vector3(x + rhs, y + rhs, z + rhs);
	}

	inline Vector3 operator-(const Vector3 &rhs) const {
		return Vector3(x - rhs.y, y - rhs.y, z - rhs.z);
	}

	inline Vector3 operator-(const float rhs) const {
		return Vector3(x - rhs, y - rhs, z - rhs);
	}

	inline Vector3 operator*(const float rhs) const {
		return Vector3(x * rhs, y * rhs, z * rhs);
	}

	inline Vector3 operator/(const float rhs) const {
		float s = 1 / rhs;
		return operator*(rhs);
	}

	inline Vector3 operator*(const Vector3 &rhs) const {
		return Vector3(x * rhs.x, y * rhs.y, z * rhs.z);
	}

	inline Vector3 operator/(const Vector3 &rhs) const{
		return Vector3(x / rhs.x, y / rhs.y, z / rhs.z);
	}

	inline Vector3& operator+=(const Vector3 &rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	inline Vector3& operator+=(const float rhs) {
		x += rhs;
		y += rhs;
		z += rhs;
		return *this;
	}

	inline Vector3& operator-=(const Vector3 &rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	inline Vector3& operator-=(const float rhs) {
		x -= rhs;
		y -= rhs;
		z -= rhs;
		return *this;
	}

	inline Vector3& operator*=(const Vector3& rhs) {
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		return *this;
	}

	inline Vector3& operator*=(const float rhs) {
		x *= rhs;
		y *= rhs;
		z *= rhs;
		return *this;
	}

	inline Vector3& operator/=(const Vector3& rhs) {
		x /= rhs.x;
		y /= rhs.y;
		z /= rhs.z;
		return *this;
	}

	inline Vector3& operator/=(const float rhs) {
		float s = 1 / rhs;
		return operator*=(s);
	}
#pragma endregion

	bool operator==(const Vector3& rhs) const {
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}
	void operator=(const Vector3& rhs) {
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
	}

#pragma region SwizzleOperations

#pragma endregion


	union {
		struct {
			float x;
			float y;
			float z;
		};

#pragma region Swizzles
		Swizzle2<3, 0, 0> xx;
		Swizzle2<3, 0, 1> xy;
		Swizzle2<3, 0, 2> xz;
		Swizzle2<3, 1, 0> yx;
		Swizzle2<3, 1, 1> yy;
		Swizzle2<3, 1, 2> yz;
		Swizzle2<3, 2, 0> zx;
		Swizzle2<3, 2, 1> zy;
		Swizzle2<3, 2, 2> zz;

		Swizzle3<3, 0, 0, 0> xxx;
		Swizzle3<3, 0, 0, 1> xxy;
		Swizzle3<3, 0, 0, 2> xxz;
		Swizzle3<3, 0, 1, 0> xyx;
		Swizzle3<3, 0, 1, 1> xyy;
		Swizzle3<3, 0, 1, 2> xyz;
		Swizzle3<3, 0, 2, 0> xzx;
		Swizzle3<3, 0, 2, 1> xzy;
		Swizzle3<3, 0, 2, 2> xzz;
		Swizzle3<3, 1, 0, 0> yxx;
		Swizzle3<3, 1, 0, 1> yxy;
		Swizzle3<3, 1, 0, 2> yxz;
		Swizzle3<3, 1, 1, 0> yyx;
		Swizzle3<3, 1, 1, 1> yyy;
		Swizzle3<3, 1, 2, 2> yyz;
		Swizzle3<3, 1, 2, 0> yzx;
		Swizzle3<3, 1, 2, 1> yzy;
		Swizzle3<3, 1, 2, 2> yzz;
		Swizzle3<3, 2, 0, 0> zxx;
		Swizzle3<3, 2, 0, 1> zxy;
		Swizzle3<3, 2, 0, 2> zxz;
		Swizzle3<3, 2, 1, 0> zyx;
		Swizzle3<3, 2, 1, 1> zyy;
		Swizzle3<3, 2, 1, 2> zyz;
		Swizzle3<3, 2, 2, 0> zzx;
		Swizzle3<3, 2, 2, 1> zzy;
		Swizzle3<3, 2, 2, 2> zzz;
#pragma endregion

		



		float items[3];
	};

};
#pragma once

#include <array>
#include <cmath>
#include <ostream>

class Vec3 {
public:
	Vec3(double, double, double);

	double dot(const Vec3&) const;
	Vec3 cross(const Vec3&) const;
	double length_squared() const;
	double length() const;
	Vec3 norm() const;

	Vec3 operator*(double) const;
	void operator*=(double);
	Vec3 operator/(double) const;
	void operator/=(double);
	Vec3 operator+(const Vec3&) const;
	void operator+=(const Vec3&);
	Vec3 operator-() const;
	Vec3 operator-(const Vec3&) const;
	void operator-=(const Vec3&);

	inline double x() const { return data[0]; }
	inline double y() const { return data[1]; }
	inline double z() const { return data[2]; }

	inline double r() const { return data[0]; }
	inline double g() const { return data[1]; }
	inline double b() const { return data[2]; }

private:
	std::array<double, 3> data {};
};

std::ostream& operator<<(std::ostream&, const Vec3&);


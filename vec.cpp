#include "vec.h"

using namespace std;

Vec3::Vec3(double x, double y, double z) : data({x, y, z}) {}

double Vec3::dot(const Vec3 &other) const {
	return x() * other.x() + y() * other.y() + z() * other.z();
}

Vec3 Vec3::cross(const Vec3 &other) const {
	return Vec3(
		y() * other.z() - z() * other.y(),
		z() * other.x() - x() * other.z(),
		x() * other.y() - y() * other.x()
	);
}

double Vec3::length_squared() const {
	return dot(*this);
}

double Vec3::length() const {
	return sqrt(length_squared());
}

Vec3 Vec3::norm() const {
	return operator/(length());
}

Vec3 Vec3::operator*(const double s) const {
	return Vec3(x() * s, y() * s, z() * s);
}

Vec3 Vec3::operator/(const double s) const {
	return operator*(1 / s);
}

Vec3 Vec3::operator+(const Vec3 &other) const {
	return Vec3(x() + other.x(), y() + other.y(), z() + other.z());
}

Vec3 Vec3::operator-() const {
	return Vec3(-x(), -y(), -z());
}

Vec3 Vec3::operator-(const Vec3 &other) const {
	return operator+(other.operator-());
}

std::ostream& operator<<(std::ostream &stream, const Vec3 &vec) {
	stream << "[" << vec.x() << ", " << vec.y() << ", " << vec.z() << "]";
	return stream;
}


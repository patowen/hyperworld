#pragma once
#pragma warning( push )
#pragma warning( disable : 26812 26450 26495 )
#include <Eigen/Dense>
#pragma warning( pop )

using Matrix4d = Eigen::Matrix<double, 4, 4, Eigen::DontAlign>;
using Vector3d = Eigen::Vector3d;
using Vector4d = Eigen::Matrix<double, 4, 1, Eigen::DontAlign>;
using Vector2d = Eigen::Matrix<double, 2, 1, Eigen::DontAlign>;

namespace VectorMath {
	inline Matrix4d perspective(double x, double y, double zNear, double zFar) {
		Matrix4d result;
		result << 2.0/x, 0.0, 0.0, 0.0,
			0.0, 2.0/y, 0.0, 0.0,
			0.0, 0.0, -(zFar + zNear) / (zFar - zNear), -2.0*(zFar * zNear) / (zFar - zNear),
			0.0, 0.0, -1.0, 0.0;
		return result;
	}

	inline Matrix4d euclideanTranslation(const Vector3d &v) {
		Matrix4d result;
		result << 1.0, 0.0, 0.0, v.x(),
			0.0, 1.0, 0.0, v.y(),
			0.0, 0.0, 1.0, v.z(),
			0.0, 0.0, 0.0, 1.0;
		return result;
	}

	// Moves the origin to the specified position
	inline Matrix4d translation(const Vector4d &v) {
		Matrix4d result;
		double x = v.x(), y = v.y(), z = v.z(), w = v.w();
		double f = 1.0 / (w + 1.0);
		result << x*x*f + 1, x*y*f, x*z*f, x,
			x*y*f, y*y*f + 1, y*z*f, y,
			x*z*f, y*z*f, z*z*f + 1, z,
			x, y, z, w;
		return result;
	}

	// Does an ideal rotation about (0, 0, 1, 1)
	inline Matrix4d horoRotation(double x, double y) {
		double sqrSum = 0.5*(x*x + y*y);
		Matrix4d result;
		result << 1, 0, x, x,
			0, 1, y, y,
			-x, -y, -sqrSum+1, -sqrSum,
			x, y, sqrSum, sqrSum+1;
		return result;
	}

	inline Matrix4d rotation(const Vector3d &axis, double theta) {
		Matrix4d result = Matrix4d::Identity();
		double x = axis.x(), y = axis.y(), z = axis.z();
		double xx = x*x, yy = y*y, zz = z*z, yz = y*z, zx = z*x, xy = x*y;
		double c = cos(theta), s = sin(theta);

		result(0, 0) = xx + (1-xx)*c;
		result(1, 0) = xy*(1-c) + z*s;
		result(2, 0) = zx*(1-c) - y*s;

		result(0, 1) = xy*(1-c) - z*s;
		result(1, 1) = yy + (1-yy)*c;
		result(2, 1) = yz*(1-c) + x*s;

		result(0, 2) = zx*(1-c) + y*s;
		result(1, 2) = yz*(1-c) - x*s;
		result(2, 2) = zz + (1-zz)*c;

		return result;
	}
	
	// Moves the origin in the specified direction with a distance proportional
	// to the magnitude of the argument
	inline Matrix4d displacement(const Vector3d &displacement) {
		double norm = displacement.norm();
		if (norm == 0) {
			return Matrix4d::Identity();
		}
		double scaleFactor = sinh(norm) / norm;

		Vector4d translateVector(displacement.x() * scaleFactor, displacement.y() * scaleFactor, displacement.z() * scaleFactor, cosh(norm));

		return translation(translateVector);
	}
}

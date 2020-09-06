#pragma once
#include <Eigen/Dense>

using Matrix4d = Eigen::Matrix<double, 4, 4, Eigen::DontAlign>;
using Vector3d = Eigen::Vector3d;
using Vector4d = Eigen::Matrix<double, 4, 1, Eigen::DontAlign>;
using Vector2d = Eigen::Matrix<double, 2, 1, Eigen::DontAlign>;

constexpr auto M_TAU = 6.2831853071795864769252867665590057683943;

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

	inline Matrix4d reflection(const Vector4d& normal) {
		return Matrix4d::Identity() - 2.0 * normal * Eigen::RowVector4d(normal(0), normal(1), normal(2), -normal(3));
	}

	inline Matrix4d isometricInverse(const Matrix4d& matrix) {
		Matrix4d result;

		result << matrix(0, 0), matrix(1, 0), matrix(2, 0), -matrix(3, 0),
			matrix(0, 1), matrix(1, 1), matrix(2, 1), -matrix(3, 1),
			matrix(0, 2), matrix(1, 2), matrix(2, 2), -matrix(3, 2),
			-matrix(0, 3), -matrix(1, 3), -matrix(2, 3), matrix(3, 3);

		return result;
	}

	inline Vector4d hyperbolicNormal(Vector4d v0, Vector4d v1, Vector4d v2) {
		double x =   v0[1]*v1[2]*v2[3] + v0[2]*v1[3]*v2[1] + v0[3]*v1[1]*v2[2] - v0[1]*v1[3]*v2[2] - v0[2]*v1[1]*v2[3] - v0[3]*v1[2]*v2[1];
		double y = -(v0[0]*v1[2]*v2[3] + v0[2]*v1[3]*v2[0] + v0[3]*v1[0]*v2[2] - v0[0]*v1[3]*v2[2] - v0[2]*v1[0]*v2[3] - v0[3]*v1[2]*v2[0]);
		double z =   v0[0]*v1[1]*v2[3] + v0[1]*v1[3]*v2[0] + v0[3]*v1[0]*v2[1] - v0[0]*v1[3]*v2[1] - v0[1]*v1[0]*v2[3] - v0[3]*v1[1]*v2[0];
		double w = -(v0[0]*v1[1]*v2[2] + v0[1]*v1[2]*v2[0] + v0[2]*v1[0]*v2[1] - v0[0]*v1[2]*v2[1] - v0[1]*v1[0]*v2[2] - v0[2]*v1[1]*v2[0]);
		return Vector4d(x, y, z, -w);
	}

	inline double hyperbolicDotProduct(Vector4d v0, Vector4d v1) {
		return v0(0)*v1(0) + v0(1)*v1(1) + v0(2)*v1(2) - v0(3)*v1(3);
	}

	inline double hyperbolicSqrNorm(Vector4d v) {
		return hyperbolicDotProduct(v, v);
	}

	// Moves the origin in the specified direction with a distance proportional
	// to the magnitude of the argument (The fourth component is assumed to be 0)
	inline Matrix4d displacement(const Vector4d &displacement) {
		double norm = displacement.norm();
		double scaleFactor = norm < 1e-30 ? 1.0 : sinh(norm) / norm;
		Vector4d translateVector(displacement.x() * scaleFactor, displacement.y() * scaleFactor, displacement.z() * scaleFactor, cosh(norm));
		return translation(translateVector);
	}

	inline Matrix4d orthogonalizeGramSchmidt(const Matrix4d& matrix) {
		Matrix4d result = matrix;
		result.col(0) /= sqrt(hyperbolicSqrNorm(result.col(0)));
		result.col(1) -= result.col(0) * hyperbolicDotProduct(result.col(0), result.col(1));
		result.col(2) -= result.col(0) * hyperbolicDotProduct(result.col(0), result.col(2));
		result.col(3) -= result.col(0) * hyperbolicDotProduct(result.col(0), result.col(3));

		result.col(1) /= sqrt(hyperbolicSqrNorm(result.col(1)));
		result.col(2) -= result.col(1) * hyperbolicDotProduct(result.col(1), result.col(2));
		result.col(3) -= result.col(1) * hyperbolicDotProduct(result.col(1), result.col(3));

		result.col(2) /= sqrt(hyperbolicSqrNorm(result.col(2)));
		result.col(3) -= result.col(2) * hyperbolicDotProduct(result.col(2), result.col(3));

		result.col(3) /= sqrt(-hyperbolicSqrNorm(result.col(3)));
		return result;
	}

	Matrix4d orthogonalizeWithSqrt(const Matrix4d& matrix);
}

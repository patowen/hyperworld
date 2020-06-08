#include "VectorMath.h"
#include <unsupported/Eigen/MatrixFunctions>

namespace VectorMath {
	Matrix4d orthogonalizeWithSqrt(const Matrix4d& matrix) {
		return matrix * (isometricInverse(matrix) * matrix).sqrt().inverse();
	}
}
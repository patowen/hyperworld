/*
	Copyright 2020 Patrick Owen

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
 */

#include "VectorMath.h"
#include <unsupported/Eigen/MatrixFunctions>
#include <Eigen/SVD>

Matrix4d VectorMath::hyperbolicSvdUnitary(const Matrix4d& matrix) {
	return matrix * (hyperbolicTranspose(matrix) * matrix).sqrt().inverse();
}

Matrix4d VectorMath::sphericalSvdUnitary(const Matrix4d& matrix) {
	Eigen::JacobiSVD<Matrix4d> svd(matrix, Eigen::ComputeThinU | Eigen::ComputeThinV);
	return svd.matrixU() * svd.matrixV().adjoint();
}

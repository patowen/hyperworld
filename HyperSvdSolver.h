#pragma once
#include <iomanip>
#include "VectorMath.h"

namespace VectorMath {
	class HyperSvdSolver {
	public:
		HyperSvdSolver(Matrix4d matrix): svdU(Matrix4d::Identity()), svdV(Matrix4d::Identity()) {
			Matrix4d m = matrix;
			int i;
			double deviation = 0;
			for (i=0; i<500; ++i) {
				QrFactorization qr(m);
				svdU *= qr.q;
				QrFactorization lpT(isometricInverse(qr.r));
				svdV *= lpT.q;
				m = isometricInverse(lpT.r);

				deviation = lpT.distFromDiagonal;
				if (lpT.distFromDiagonal < 1e-4) {
					break;
				}
			}

			std::cout << i << ", " << deviation << std::endl;
		}

		Matrix4d getOrthogonal() {
			return svdU * isometricInverse(svdV);
		}

	private:
		Matrix4d svdU;
		Matrix4d svdV;

		class QrFactorization {
		public:
			QrFactorization(Matrix4d matrix): q(matrix), r(Matrix4d::Zero()), distFromDiagonal(0) {
				int indices[4] = {3, 0, 1, 2};
				int poles[4] = {-1, 1, 1, 1};

				for (int i=0; i<4; ++i) {
					r(indices[i], indices[i]) = sqrt(poles[i] * hyperbolicSqrNorm(q.col(indices[i])));
					q.col(indices[i]) /= r(indices[i], indices[i]);

					for (int j=i+1; j<4; ++j) {
						r(indices[i], indices[j]) = poles[i] * hyperbolicDotProduct(q.col(indices[i]), q.col(indices[j]));
						distFromDiagonal += r(indices[i], indices[j]) * r(indices[i], indices[j]);
						q.col(indices[j]) -= q.col(indices[i]) * r(indices[i], indices[j]);
					}
				}
			}

			Matrix4d q;
			Matrix4d r;
			double distFromDiagonal;
		};
	};
}
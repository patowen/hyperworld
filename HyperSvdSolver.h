#pragma once
#include <iomanip>
#include "VectorMath.h"

namespace VectorMath {
	class HyperSvdSolver {
	public:
		HyperSvdSolver(Matrix4d matrix): svdU(Matrix4d::Identity()), svdV(Matrix4d::Identity()) {
			Matrix4d m = matrix;
			for (int i=0; i<20; ++i) {
				QrFactorization qr(m);
				svdU *= qr.q;
				QrFactorization lpT(isometricInverse(qr.r));
				svdV *= lpT.q;
				m = isometricInverse(lpT.r);
			}

			Matrix4d dummy = Matrix4d::Identity();
			for (int i=0; i<4; ++i) {
				dummy(i, i) = m(i, i);
			}
			std::cout << (m - dummy).norm() << "\n";
		}

		Matrix4d getOrthogonal() {
			return svdU * isometricInverse(svdV);
		}

	private:
		Matrix4d svdU;
		Matrix4d svdV;

		class QrFactorization {
		public:
			QrFactorization(Matrix4d matrix): q(matrix), r(Matrix4d::Zero()) {
				r(3, 3) = sqrt(-hyperbolicSqrNorm(q.col(3)));
				q.col(3) /= r(3, 3);
				r(3, 0) = -hyperbolicDotProduct(q.col(3), q.col(0));
				r(3, 1) = -hyperbolicDotProduct(q.col(3), q.col(1));
				r(3, 2) = -hyperbolicDotProduct(q.col(3), q.col(2));
				q.col(0) -= q.col(3) * r(3, 0);
				q.col(1) -= q.col(3) * r(3, 1);
				q.col(2) -= q.col(3) * r(3, 2);

				r(0, 0) = sqrt(hyperbolicSqrNorm(q.col(0)));
				q.col(0) /= r(0, 0);
				r(0, 1) = hyperbolicDotProduct(q.col(0), q.col(1));
				r(0, 2) = hyperbolicDotProduct(q.col(0), q.col(2));
				q.col(1) -= q.col(0) * r(0, 1);
				q.col(2) -= q.col(0) * r(0, 2);

				r(1, 1) = sqrt(hyperbolicSqrNorm(q.col(1)));
				q.col(1) /= r(1, 1);
				r(1, 2) = hyperbolicDotProduct(q.col(1), q.col(2));
				q.col(2) -= q.col(1) * r(1, 2);

				r(2, 2) = sqrt(hyperbolicSqrNorm(q.col(2)));
				q.col(2) /= r(2, 2);
			}

			Matrix4d q;
			Matrix4d r;
		};
	};
}
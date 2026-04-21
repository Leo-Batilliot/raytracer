// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// Matrix4::inverse() — Gauss-Jordan elimination with partial pivoting.
//
// Called at Transform construction time (never from hot paths), so
// clarity wins over micro-optimisation. Partial pivoting keeps the
// algorithm numerically stable on rigid + scaled transforms, which is
// all the raytracer ever builds.

#include "math/Matrix4.hpp"

#include <array>
#include <cmath>
#include <cstddef>
#include <utility>

#include "core/Exceptions.hpp"

namespace rt {

namespace {

// Threshold below which a pivot is considered zero. Picked well above
// the double epsilon to reject matrices that would otherwise produce
// nonsense inverses (garbage in, garbage out).
constexpr double kSingularityThreshold = 1e-12;

}  // namespace

Matrix4 Matrix4::inverse() const
{
    // Augmented matrix [A | I], row-reduced in place to [I | A^-1].
    std::array<std::array<double, 8>, 4> aug{};
    for (std::size_t i = 0; i < 4; ++i) {
        for (std::size_t j = 0; j < 4; ++j) {
            aug[i][j] = (*this)(i, j);
        }
        aug[i][4 + i] = 1.0;
    }

    for (std::size_t col = 0; col < 4; ++col) {
        // Partial pivoting: pick the row with the largest |A[r][col]|.
        std::size_t pivot = col;
        double maxAbs = std::fabs(aug[col][col]);
        for (std::size_t r = col + 1; r < 4; ++r) {
            const double v = std::fabs(aug[r][col]);
            if (v > maxAbs) {
                maxAbs = v;
                pivot = r;
            }
        }

        if (maxAbs < kSingularityThreshold) {
            throw MathError("Matrix4::inverse(): matrix is singular");
        }

        if (pivot != col) {
            std::swap(aug[pivot], aug[col]);
        }

        // Normalise the pivot row so aug[col][col] becomes 1.
        const double pv = aug[col][col];
        for (std::size_t j = 0; j < 8; ++j) {
            aug[col][j] /= pv;
        }

        // Eliminate the current column in every other row.
        for (std::size_t r = 0; r < 4; ++r) {
            if (r == col) {
                continue;
            }
            const double factor = aug[r][col];
            if (factor == 0.0) {
                continue;  // nothing to subtract
            }
            for (std::size_t j = 0; j < 8; ++j) {
                aug[r][j] -= factor * aug[col][j];
            }
        }
    }

    Matrix4 result;
    for (std::size_t i = 0; i < 4; ++i) {
        for (std::size_t j = 0; j < 4; ++j) {
            result(i, j) = aug[i][4 + j];
        }
    }
    return result;
}

}  // namespace rt

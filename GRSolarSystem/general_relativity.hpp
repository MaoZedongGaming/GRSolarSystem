#include "particles.hpp"
#include "maths.hpp"

//spatial distance
template <typename T0, typename T1>
auto distance2(Vector4X<T0> const& v, Vector4X<T1> const& w);

template <typename T>
T calculateNewtonianPotential(Vector4X<T> const& pos, Particles const& masses, size_t planetIdx);

// g_{\mu \nu} (pos)
template <typename T>
T metricAt(size_t mu, size_t nu, Vector4X<T> const& pos, Particles const& masses, size_t planetIdx);

// \partial_\alpha g_{\mu \nu} (pos)
double metricDerivativeAt(size_t alpha, size_t mu, size_t nu, Vector4d const& pos, Particles const& masses, size_t planetIdx);

// g^{\mu \nu} (pos)
double inverseMetricAt(size_t mu, size_t nu, Vector4d const& pos, Particles const& masses, size_t planetIdx);

// Gamma^\lambda_{\mu \nu} (pos) = \frac{1}{2} g^{\lambda \alpha} (\partial_\mu g_{\alpha \nu} + \partial_\nu g_{\alpha \mu} - \partial_\alpha g_{\mu \nu}
double christoffelSymbol(size_t lambda, size_t mu, size_t nu, Vector4d const& pos, Particles const& masses, size_t planetIdx);

// \frac{dv^\lambda}{d\tau} = -\Gamma^\lambda_{\muj \nu} (pos) v^\mu v^\nu
Vector4d geodesicEquation(Vector4d const& pos, Vector4d const& vel, Particles const& masses, size_t planetIdx);

void updateParticlesEinstein(Particles& masses, double dTau);
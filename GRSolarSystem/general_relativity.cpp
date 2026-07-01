#include "general_relativity.hpp"
#include "constants.hpp"

using namespace Constants;

enum class MetricType {
    LINEAR = 0,
    ISOTROPIC
};

constexpr MetricType metricType = MetricType::ISOTROPIC;

template <typename T0, typename T1>
auto distance2(Vector4X<T0> const& v, Vector4X<T1> const& w) {
    decltype((T0() - T1()) * (T0() - T1())) res{};
    for (std::size_t i = 1; i < 4; ++i)
        res += (v[i] - w[i]) * (v[i] - w[i]);
    return res;
}

template <typename T>
T calculateNewtonianPotential(Vector4X<T> const& pos, Particles const& bodies, size_t planetIdx) {
    T res = 0.0;
    for (size_t i = 0; i < bodies.size; ++i) {
        if (i == planetIdx)
            continue;
        res += G * bodies.masses[i] / sqrt(distance2(pos, bodies.positions[i]));
    }
    return res;
}

template <typename T>
T metricAt(size_t mu, size_t nu, Vector4X<T> const& pos, Particles const& bodies, size_t planetIdx) {
    if (mu != nu)
        return T{};
    T newtonianPotential = calculateNewtonianPotential(pos, bodies, planetIdx) / (c * c);
    switch (metricType) {
    case MetricType::LINEAR:
        if (mu == 0)
            return -(1 - 2 * newtonianPotential);
        return 1 + 2 * newtonianPotential;
        break;
    case MetricType::ISOTROPIC:
        T num = 1.0 - newtonianPotential / 2.0;
        T den = 1.0 + newtonianPotential / 2.0;
        if (mu == 0)
            return -(num * num) / (den * den);  // g_00 = -((1 - \Phi/2)/(1 + \Phi/2))^2
        return den * den * den * den; // g_ii = (1 + \Phi/2)^4
        break;
    }
}

double metricDerivativeAt(size_t alpha, size_t mu, size_t nu, Vector4d const& pos, Particles const& bodies, size_t planetIdx) {
    if(mu != nu)
        return 0;
    Vector4X<Dual<double>> dualPos;
    for (size_t mu = 0; mu < 4; ++mu)
        dualPos[mu] = pos[mu];
    dualPos[alpha] += Dual<double>(0, 1.0);  // \partial_\alpha 
    return metricAt(mu, nu, dualPos, bodies, planetIdx).derivative;
}

double inverseMetricAt(size_t mu, size_t nu, Vector4d const& pos, Particles const& bodies, size_t planetIdx) {
    if (mu != nu)
        return 0;
    return 1.0 / metricAt(mu, nu, pos, bodies, planetIdx);  //g_{\mu \nu} * g^{\nu \alpha) = \delta^\alpha_\mu, g^{\mu \nu} = 1 / g_{\mu \nu} for diagonal metric
}

double christoffelSymbol(size_t lambda, size_t mu, size_t nu, Vector4d const& pos, Particles const& bodies, size_t planetIdx) {
    double res =  //\alpha = \lambda when the metric is diagonal
        0.5 * inverseMetricAt(lambda, lambda, pos, bodies, planetIdx) *
             (metricDerivativeAt(mu, lambda, nu, pos, bodies, planetIdx)
            + metricDerivativeAt(nu, lambda, mu, pos, bodies, planetIdx)
            - metricDerivativeAt(lambda, mu, nu, pos, bodies, planetIdx));
    return res;
}

Vector4d geodesicEquation(Vector4d const& pos, Vector4d const& vel, Particles const& bodies, size_t planetIdx) {
    Vector4d acc;
    for (size_t lambda = 0; lambda < 4; ++lambda) {
        for (size_t mu = 0; mu < 4; ++mu) {
            for (size_t nu = 0; nu < 4; ++nu)
                acc[lambda] -= christoffelSymbol(lambda, mu, nu, pos, bodies, planetIdx) * vel[mu] * vel[nu];
        }
    }
    return acc;
}

struct ParticleState {
    Vector4d pos;
    Vector4d vel;
};

struct StateDerivative {
    Vector4d vel;
    Vector4d acc;
};

StateDerivative evaluate(ParticleState const& state, Particles const& bodies, size_t planetIdx) {
    StateDerivative d;
    d.vel = state.vel;
    d.acc = geodesicEquation(state.pos, state.vel, bodies, planetIdx);
    return d;
}

void updateParticlesEinstein(Particles& bodies, double dTau) {
    static ParticleState initial;
    static std::vector<Vector4d> nextPositions(bodies.size);
    static std::vector<Vector4d> nextVelocities(bodies.size);
    static StateDerivative k1, k2, k3, k4;
    static double spatialVel = 0;  // for normalisation since g_{\mu \nu} u^\mu u^\nu = -c^2
    for (size_t i = 0; i < bodies.size; ++i) {
        initial.pos = bodies.positions[i];
        initial.vel = bodies.velocities[i];

        auto step_state = [](ParticleState const& state, StateDerivative const& k, double timestep) -> ParticleState {
            ParticleState res;
            res.pos = state.pos + k.vel * timestep;
            res.vel = state.vel + k.acc * timestep;
            return res;
        };

        k1 = evaluate(initial, bodies, i);
        k2 = evaluate(step_state(initial, k1, dTau / 2.0), bodies, i);
        k3 = evaluate(step_state(initial, k2, dTau / 2.0), bodies, i);
        k4 = evaluate(step_state(initial, k3, dTau), bodies, i);

        nextPositions[i] = initial.pos + dTau / 6.0 * (k1.vel + 2.0 * k2.vel + 2.0 * k3.vel + k4.vel);
        nextVelocities[i] = initial.vel + dTau / 6.0 * (k1.acc + 2.0 * k2.acc + 2.0 * k3.acc + k4.acc);

        spatialVel = 0;
        for (int j = 1; j < 4; ++j)
            spatialVel += metricAt(j, j, nextPositions[i], bodies, i) * (nextVelocities[i])[j] * (nextVelocities[i])[j];
        nextVelocities[i][0] = std::sqrt((-c * c - spatialVel) / metricAt(0, 0, nextPositions[i], bodies, i));
    }

    for (size_t i = 0; i < bodies.size; ++i) {
        bodies.positions[i] = nextPositions[i];
        bodies.velocities[i] = nextVelocities[i];
    }
}
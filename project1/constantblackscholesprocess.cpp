
#include "constantblackscholesprocess.hpp"

#include <iostream>

#include <ql/processes/eulerdiscretization.hpp>

namespace QuantLib {

ConstantBlackScholesProcess::ConstantBlackScholesProcess(
                                        double underlyingValue,
                                        double riskFreeRate,
                                        double volatility,
                                        double dividend) : StochasticProcess1D(ext::make_shared<EulerDiscretization>()) {
    underlyingValue = underlyingValue;
    riskFreeRate = riskFreeRate;
    volatility = volatility;
    dividend = dividend;
    }
Real ConstantBlackScholesProcess::x0() const {
    return underlyingValue;
}

Real ConstantBlackScholesProcess::diffusion(Time t, Real x) const {
    return volatility;
}
Real ConstantBlackScholesProcess::drift(Time t, Real x) const {
    return riskFreeRate - dividend - 0.5*volatility*volatility;
}
Real ConstantBlackScholesProcess::apply(Real s0, Real dx) const {
    return s0*std::exp(dx);
}

}

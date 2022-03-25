
#include "constantblackscholesprocess.hpp"

#include <iostream>

#include <ql/processes/eulerdiscretization.hpp>

namespace QuantLib {

    ConstantBlackScholesProcess::ConstantBlackScholesProcess(
                                        double underlyingValue,
                                        double riskFreeRate,
                                        double volatility_,
                                        double dividend_) :StochasticProcess1D(ext::make_shared<EulerDiscretization>())
    {underlying_value = underlyingValue;
    risk_free_rate = riskFreeRate;
    volatility = volatility_;
    dividend = dividend_;}

Real ConstantBlackScholesProcess::x0() const {
    return underlying_value;
}

Real ConstantBlackScholesProcess::drift(Time t, Real x) const {
    return risk_free_rate - dividend - 0.5*volatility*volatility;
}

Real ConstantBlackScholesProcess::diffusion(Time t, Real x) const {
    return volatility;
}

Real ConstantBlackScholesProcess::apply(Real x0, Real dx) const {
    return x0 * std::exp(dx);
}

}

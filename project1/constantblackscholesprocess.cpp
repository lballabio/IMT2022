
#include "constantblackscholesprocess.hpp"
#include <iostream>
#include <ql/processes/eulerdiscretization.hpp>


namespace QuantLib {


//Constructor
ConstantBlackScholesProcess::ConstantBlackScholesProcess(double rfr, double div, double underlyingValue, double vol)
:   riskFreeRate_(rfr),
    dividendYield_(div),
    blackVolatility_(vol),
    underlyingValue_(underlyingValue),
    StochasticProcess1D(ext::make_shared<EulerDiscretization>()) {}

//Diffusion is constant and equals to the BS Volatility
Real ConstantBlackScholesProcess::diffusion(Time t, Real x) const {
        return this->blackVolatility_;
    }

//Drift comes from the BS Stochastic Differential Equation Solution
Real ConstantBlackScholesProcess::drift(Time t, Real x) const {
        Real sigma = this->diffusion(t,x);

        return this->riskFreeRate_
             - this->dividendYield_
             - 0.5 * sigma * sigma;
    }

Real ConstantBlackScholesProcess::apply(Real x0, Real dx) const {
        return x0 * std::exp(dx);
    }

Real ConstantBlackScholesProcess::x0() const {
        return this->underlyingValue_;
    }



}

#ifndef CONSTANT_BLACK_SCHOLES_PROCESS_H
#define CONSTANT_BLACK_SCHOLES_PROCESS_H
#include <ql/stochasticprocess.hpp>

using namespace QuantLib;

class constantblackscholesprocess : public StochasticProcess1D {

private:
    double spot;
    double rf_rate;
    double dividend;
    double volatility;



public:
    constantblackscholesprocess();
    constantblackscholesprocess(constantblackscholesprocess& process);
    constantblackscholesprocess(double spot, double rf_rate, double dividend, double volatility);
    ~constantblackscholesprocess();
    double getSpot() const;
    double getRf_rate() const;
    double getDividend() const;
    double getVolatility() const;

    Real x0() const;
    Real drift(Time t, Real x) const;
    Real diffusion(Time t, Real x) const;
    Real apply(Real x0, Real dx) const;
        
};

#endif //CONSTANT_BLACK_SCHOLES_PROCESS_H

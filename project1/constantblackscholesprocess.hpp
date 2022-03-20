
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
    double getSpot();
    double getRf_rate();
    double getDividend();
    double getVolatility();

    double x0();
    double drift(Time t, Real x);
    double diffusion(Time t, Real x);
    double apply(Real x0, Real dx);
        
};

#endif //CONSTANT_BLACK_SCHOLES_PROCESS_H

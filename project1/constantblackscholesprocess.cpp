#include "constantblackscholesprocess.hpp"

constantblackscholesprocess::constantblackscholesprocess() {
    spot = 100.;
    rf_rate = 0.05;
    dividend = 3.;
    volatility = 1.;
}

constantblackscholesprocess::constantblackscholesprocess(constantblackscholesprocess& process_ref) {
    spot = process_ref.getSpot();
    rf_rate = process_ref.getRf_rate();
    dividend = process_ref.getDividend();
    volatility = process_ref.getVolatility();
}

constantblackscholesprocess::constantblackscholesprocess(double spot1, double rf_rate1, double dividend1, double volatility1) {
    spot = spot1;
    rf_rate = rf_rate1;
    dividend = dividend1;
    volatility = volatility1;
}

constantblackscholesprocess::~constantblackscholesprocess() {

}

double constantblackscholesprocess::getSpot() {
    return spot;
}

double constantblackscholesprocess::getRf_rate() {
    return rf_rate;
}

double constantblackscholesprocess::getDividend() {
    return dividend;
}

double constantblackscholesprocess::getVolatility() {
    return volatility;
}

double constantblackscholesprocess::x0() {
    return getSpot();
}

double constantblackscholesprocess::drift(Time t, Real x) {
    return this->getRf_rate() - this->getDividend() -0.5*getVolatility()*getVolatility();
}

double constantblackscholesprocess::diffusion(Time t, Real x) {
    return this->getVolatility();
}

double constantblackscholesprocess::apply(Real x0, Real dx) {
    return x0 * std::exp(dx);
}


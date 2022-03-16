#include "constantblackscholesprocess.hpp"

constantblackscholesprocess::constantblackscholesprocess() {
    process = new constantblackscholesprocess();
    process.spot = 100
    process.rf_rate = 0.05;
    process.dividend = 3;
    process.volatility = 1;
}

constantblackscholesprocess::constantblackscholesprocess(constantblackscholesprocess process_ref) {
    process = new constantblackscholesprocess();
    process.spot = process_ref.getSpot();
    process.rf_rate = process_ref.getRf_rate();
    process.dividend = process_ref.getDividend();
    process.volatility = process_ref.getVolatility();
}

constantblackscholesprocess::constantblackscholesprocess(double spot1, double rf_ratee2, double dividend1, double volatility1) {
    process = new constantblackscholesprocess();
    process.spot = spot1;
    process.rf_rate = rf_rate1;
    process.dividend = dividend1;
    process.volatility = volatility1;
}

constantblackscholesprocess::~constantblackscholesprocess() {

}

constantblackscholesprocess::getSpot() {
    return spot;
}

constantblackscholesprocess::getRf_rate() {
    return rf_rate;
}

constantblackscholesprocess::getDividend() {
    return dividend;
}

constantblackscholesprocess::getVolatility() {
    return volatility;
}

Real GeneralizedBlackScholesProcess::x0() const {
    return x0_->value();
}

constantblackscholesprocess::drift(Time t, Real x) const {
    return this->getRf_rate() - this->getDividend() -0.5*getVolatility()*getVolatility();
}

constantblackscholesprocess::diffusion(Time t, Real x) const {
    return this->getVolatility();
}

constantblackscholesprocess::apply(Real x0, Real dx) const {
    return this -> x0 * std::exp(dx);
}


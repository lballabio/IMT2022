#ifndef CONSTANTBLACKSCHOLESPROCESS_H
#define CONSTANTBLACKSCHOLESPROCESS_H
#include <ql/stochasticprocess.hpp>

namespace QuantLib {

    class ConstantBlackScholesProcess : public StochasticProcess1D {

        // your implementation goes here
        
        public :
            ConstantBlackScholesProcess(double underlyingValue, double riskFreeRate, double volatility_, double dividend_);

            // we modify methods that are used in the usual case and called during compilation
            Real x0() const;
            Real drift(Time t, Real x) const;
            Real diffusion(Time t, Real x) const;
            Real apply(Real x0, Real dx) const;
        
        
        private :
            double underlying_value;
            double risk_free_rate;
            double volatility;
            double dividend;
    };

}

#endif


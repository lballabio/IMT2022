
#include <ql/stochasticprocess.hpp>

namespace QuantLib {

    class ConstantBlackScholesProcess : public StochasticProcess1D {

        // your implementation goes here
        public :
        double underlyingValue;
        double riskFreeRate;
        double volatility;
        double dividend;
        
        public :
        ConstantBlackScholesProcess(double underlyingValue,
                                    double riskFreeRate,
                                    double volatility,
                                    double dividend);

        Real x0() const;
        Real diffusion(Time t, Real x) const;
        Real drift(Time t, Real x) const;
        Real apply(Real s0, Real dx) const;
    };

}


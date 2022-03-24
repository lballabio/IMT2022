
#include <ql/stochasticprocess.hpp>

namespace QuantLib {

    class ConstantBlackScholesProcess : public StochasticProcess1D {

    public:
        ConstantBlackScholesProcess(float underlying_value_ , float risk_free_rate_, float volatility_ , float dividend_);
        

        Real x0() const;
        Real drift(Time t, Real x) const;
        Real diffusion(Time t, Real x) const;
        Real apply(Real x0, Real dx) const;
        
        float underlying_value;
        float risk_free_rate;
        float volatility;
        float dividend;
    };

}
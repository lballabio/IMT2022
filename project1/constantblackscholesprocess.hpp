
#include <ql/stochasticprocess.hpp>

namespace QuantLib {

    class ConstantBlackScholesProcess : public StochasticProcess1D {

    private:
        const double spot;
        const double rf_rate;
        const double dividend;
        const double volatility;
        
    

    public:
        ConstantBlackScholesProcess();
        ConstantBlackScholesProcess(const ConstantBlackScholesProcess & process);
        ConstantBlackScholesProcess(double spot, double rf_rate, double dividend, double volatility);
        ~ConstantBlackScholesProcess();

        double getSpot();
        double getRf_rate();
        double getDividend();
        double getVolatility();

        Real x0() const;
        Real drift(Time t, Real x) const;
        Real diffusion(Time t, Real x) const;
        Real apply(Real x0, Real dx) const;
        
    };


}

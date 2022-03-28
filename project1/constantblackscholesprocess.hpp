
#pragma once //in order to cause the current source file to be included only once in a single compilation, otherwise error of redefinition
#include <ql/stochasticprocess.hpp>

#include <iostream>

namespace QuantLib {

    class ConstantBlackScholesProcess : public StochasticProcess1D {

        // your implementation goes here

        //Constant parameters
        private:
            double riskFreeRate_;
            double dividendYield_;
            double underlyingValue_ ;
            double blackVolatility_;
        
        public:

            //Constructor
            ConstantBlackScholesProcess(double rfr, double div, double underlyingValue,
            double vol);

            //Need to be precalculated with constant param
            Real drift(Time t, Real x) const override; 

            //Need to be precalculated with constant param
            Real diffusion(Time t, Real x) const override;
            
            Real apply(Real x0, Real dx) const override;
            
            //Equivalent to the underlyingValue_ as it is supposed constant
            Real x0() const override; 
    };

}


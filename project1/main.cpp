
#include <ql/qldefines.hpp>
#ifdef BOOST_MSVC
#  include <ql/auto_link.hpp>
#endif
#include "constantblackscholesprocess.hpp"
#include "mceuropeanengine.hpp"
#include <ql/instruments/vanillaoption.hpp>
#include <ql/instruments/payoffs.hpp>
#include <ql/exercise.hpp>
#include <ql/pricingengines/vanilla/mceuropeanengine.hpp>
#include <ql/termstructures/yield/zerocurve.hpp>
#include <ql/termstructures/volatility/equityfx/blackvariancecurve.hpp>
#include <ql/time/calendars/target.hpp>
#include <ql/utilities/dataformatters.hpp>
#include <iostream>
#include <chrono>

#include <fstream>

using namespace QuantLib;

int main() {

    try {

        // modify the sample code below to suit your project
        
        std::ofstream resultats("Resultats.csv");
        //resultats.open("Resultats.csv");
        resultats << "step, sample, const_npv, const_err, const_time, npv, err time," << "\n";
        
        
        Calendar calendar = TARGET();
        Date today = Date(24, February, 2022);
        Settings::instance().evaluationDate() = today;

        Option::Type type(Option::Put);
        Real underlying = 36;
        Real strike = 40;
        Date maturity(24, August, 2022);
        
        std::cout << "-------------------------------" << std::endl;
        std::cout << "Date: " << today << std::endl;
        std::cout << "Underlying: " << underlying << std::endl;
        std::cout << "Strike: " << strike << std::endl;
        std::cout << "Maturity: " << maturity << std::endl;
        std::cout << "-------------------------------" << std::endl;
        
        ext::shared_ptr<Exercise> europeanExercise(new EuropeanExercise(maturity));
        ext::shared_ptr<StrikedTypePayoff> payoff(new PlainVanillaPayoff(type, strike));

        Handle<Quote> underlyingH(ext::make_shared<SimpleQuote>(underlying));

        DayCounter dayCounter = Actual365Fixed();
        Handle<YieldTermStructure> riskFreeRate(
            ext::shared_ptr<YieldTermStructure>(
                new ZeroCurve({today, today + 6*Months}, {0.01, 0.015}, dayCounter)));
        Handle<BlackVolTermStructure> volatility(
            ext::shared_ptr<BlackVolTermStructure>(
                new BlackVarianceCurve(today, {today+3*Months, today+6*Months}, {0.20, 0.25}, dayCounter)));

        ext::shared_ptr<BlackScholesProcess> bsmProcess(
                 new BlackScholesProcess(underlyingH, riskFreeRate, volatility));

        // options
        VanillaOption europeanOption(payoff, europeanExercise);

        Size timeSteps = 10;
        Size mcSeed = 42;
        bool constantParameters = true;
        
        
        int max_samples = 7;
        int max_steps = 4;
        Size nbrSamples = 10;
        
        // We create a loop in order to do several runs in one
        // by changing the number of steps and samples
        for (int i=1; i < max_steps; i++){
            for (int j=2; j<max_samples; j++) {
                
                timeSteps = pow(10, i);
                nbrSamples = pow(10, j);
                // Test with constant parameters
                ext::shared_ptr<PricingEngine> mcengine1;
                mcengine1 = MakeMCEuropeanEngine_2<PseudoRandom>(bsmProcess)
                    .withSteps(timeSteps)
                    //.withAbsoluteTolerance(0.01)
                    .withSeed(mcSeed)
                    .withSamples(nbrSamples)
                    .withConstantBSParameters(constantParameters);
                    
                europeanOption.setPricingEngine(mcengine1);

                auto startTime1 = std::chrono::steady_clock::now();

                Real NPV1 = europeanOption.NPV();

                auto endTime1 = std::chrono::steady_clock::now();

                double us1 = std::chrono::duration_cast<std::chrono::microseconds>(endTime1 - startTime1).count();

                Real errorEstimate1 = europeanOption.errorEstimate();
                
                std::cout << "-------------------------------" << std::endl;
                std::cout << "Constant BS Parameters: True" << std::endl;
                std::cout << "NPV: " << NPV1 << std::endl;
                std::cout << "Estimation of Error: " << errorEstimate1 << std::endl;
                std::cout << "Elapsed time: " << us1 / 1000000 << " s" << std::endl;
                std::cout << "time Steps: " << timeSteps << std::endl;
                std::cout << "Number of Samples: " << nbrSamples << std::endl;
                std::cout << "-------------------------------" << std::endl;
                
                
                // Test without constant parameters
                ext::shared_ptr<PricingEngine> mcengine2;
                mcengine2 = MakeMCEuropeanEngine_2<PseudoRandom>(bsmProcess)
                    .withSteps(timeSteps)
                    //.withAbsoluteTolerance(0.01)
                    .withSeed(mcSeed)
                    .withSamples(nbrSamples);
                europeanOption.setPricingEngine(mcengine2);

                auto startTime2 = std::chrono::steady_clock::now();

                Real NPV2 = europeanOption.NPV();

                auto endTime2 = std::chrono::steady_clock::now();

                double us2 = std::chrono::duration_cast<std::chrono::microseconds>(endTime2 - startTime2).count();

                Real errorEstimate2 = europeanOption.errorEstimate();
                
                std::cout << "-------------------------------" << std::endl;
                std::cout << "Constant BS Parameters: False" << std::endl;
                std::cout << "NPV: " << NPV2 << std::endl;
                std::cout << "Estimation of Error: " << errorEstimate2 << std::endl;
                std::cout << "Elapsed time: " << us2 / 1000000 << " s" << std::endl;
                std::cout << "time Stpes: " << timeSteps << std::endl;
                std::cout << "Number of Samples: " << nbrSamples << std::endl;
                std::cout << "-------------------------------" << std::endl;
                
                resultats << timeSteps << "," << nbrSamples << "," << NPV1 << "," << errorEstimate1 << "," << us1 << "," << NPV2 << "," << errorEstimate2 << "," << us2 << "\n";
                
            }
        }
        
        resultats.close();
        
        return 0;

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "unknown error" << std::endl;
        return 1;
    }
}


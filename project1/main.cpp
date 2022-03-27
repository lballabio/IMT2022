
#include <ql/qldefines.hpp>
#ifdef BOOST_MSVC
#  include <ql/auto_link.hpp>
#endif
#include "constantblackscholesprocess.hpp"
#include "mceuropeanengine.hpp"
#include "mcdiscretearithmeticapengine.hpp"
#include <ql/instruments/vanillaoption.hpp>
#include <ql/instruments/asianoption.hpp>
#include <ql/instruments/payoffs.hpp>
#include <ql/exercise.hpp>
#include <ql/quantlib.hpp>
#include <ql/pricingengines/vanilla/mceuropeanengine.hpp>
#include <ql/termstructures/yield/zerocurve.hpp>
#include <ql/termstructures/volatility/equityfx/blackvariancecurve.hpp>
#include <ql/time/calendars/target.hpp>
#include <ql/utilities/dataformatters.hpp>
#include <iostream>
#include <chrono>

using namespace QuantLib;

int main() {

    try {

        Real strike = 40;
        Calendar calendar = TARGET();
        Date today = Date(24, February, 2022);
        Settings::instance().evaluationDate() = today;

        Option::Type type(Option::Put);
        Real underlying = 36;
        Date maturity(24, May, 2022);

        ext::shared_ptr<Exercise> europeanExercise(new EuropeanExercise(maturity));
        ext::shared_ptr<StrikedTypePayoff> payoff(new PlainVanillaPayoff(type, strike));

        Handle<Quote> underlyingH(ext::make_shared<SimpleQuote>(underlying));

        DayCounter dayCounter = Actual365Fixed();
        Handle<YieldTermStructure> riskFreeRate(ext::shared_ptr<YieldTermStructure>(new ZeroCurve({today, today + 6*Months}, {0.01, 0.015}, dayCounter)));
        Handle<BlackVolTermStructure> volatility(ext::shared_ptr<BlackVolTermStructure>(new BlackVarianceCurve(today, {today+3*Months, today+6*Months}, {0.20, 0.25}, dayCounter)));
        ext::shared_ptr<BlackScholesProcess> bsmProcess(new BlackScholesProcess(underlyingH, riskFreeRate, volatility));

        for(int i = 1; i <= 1000; i=i*10)
        {
            for(int j = 10; j <= 100000; j=j*10)
            {
                // European Option : Calculation for non-constant parameters

                std::cout << "\nIteration with a number of steps equal to : " << i << " and a number of samples of : " << j << std::endl;
                VanillaOption europeanOption(payoff, europeanExercise);
                Size timeSteps = i;
                Size mcSeed = 42;
                std::cout << "Calculation with non constant parameters" << std::endl;
                ext::shared_ptr<PricingEngine> mcengine;
                europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(MakeMCEuropeanEngine_2<PseudoRandom>(bsmProcess, false)
                    .withSteps(timeSteps)
                    .withSeed(mcSeed)
                    .withSamples(j)
                    .withConstantParameters(false)));

                auto startTime = std::chrono::steady_clock::now();
                Real NPV = europeanOption.NPV();
                auto endTime = std::chrono::steady_clock::now();
                double us = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
                Real error_npv1 = europeanOption.errorEstimate();

                std::cout <<std::fixed;
                std::cout << std::setprecision(7);
                std::cout << "NPV: " << NPV << std::endl;
                std::cout << "Error : " << error_npv1 << std::endl;
                std::cout << "Elapsed time: " << us / 1000000 << " s" << std::endl;

                // European Option : Calculation for constant parameters

                std::cout << "Calculation with constant parameters" << std::endl;

                europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(MakeMCEuropeanEngine_2<PseudoRandom>(bsmProcess, true)
                    .withSteps(timeSteps)
                    .withSeed(mcSeed)
                    .withSamples(j)
                    .withConstantParameters(true)));

                startTime = std::chrono::steady_clock::now();
                Real NPV_2 = europeanOption.NPV();
                endTime = std::chrono::steady_clock::now();
                us = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
                Real error_npv2 = europeanOption.errorEstimate();

                std::cout << std::fixed;
                std::cout << std::setprecision(7);
                std::cout << "NPV: " << NPV_2 << std::endl;
                std::cout << "Elapsed time: " << us / 1000000 << " s" << std::endl;
                std::cout << "Error : " << error_npv2 << std::endl;
            }
        }

        std::cout << "\nSame approach but with an asian option"<< std::endl;
        // Asian Option : non-constant parameters
        Size mcSeed = 42;
        ext::shared_ptr<Exercise> europeanExercise_2(new EuropeanExercise(maturity));
        ext::shared_ptr<StrikedTypePayoff> payoff_2(new PlainVanillaPayoff(type, strike));
        Average::Type averageType = Average::Arithmetic;
        Real runningSum = 1.0;
        Size pastFixings = 0;
        std::vector<Date> fixingDates;
        Date incrementedDate=today+7;
        while(incrementedDate <= maturity)
        {
            fixingDates.push_back(incrementedDate);
            incrementedDate=incrementedDate+7;
        }

        DiscreteAveragingAsianOption discreteAsianOption(averageType, runningSum, pastFixings, fixingDates, payoff_2, europeanExercise_2);
        discreteAsianOption.setPricingEngine(
            boost::shared_ptr<PricingEngine>(
            MakeMCDiscreteArithmeticAPEngine<LowDiscrepancy>(bsmProcess)
                .withSamples(1500)
                .withSeed(mcSeed)));

        auto startTime = std::chrono::steady_clock::now();
        std::cout << "Asian Option price : " << discreteAsianOption.NPV() << std::endl;
        auto endTime = std::chrono::steady_clock::now();
        double us = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        std::cout << "Elapsed time: " << us / 1000000 << " s" << std::endl;

        // Asian Option : Calculation for constant-parameters

        std::cout << "Calculation with constant parameters :" << std::endl;
        bool const_bsm = true;
        discreteAsianOption.setPricingEngine(
            boost::shared_ptr<PricingEngine>(
            MakeMCDiscreteArithmeticAPEngine<LowDiscrepancy>(bsmProcess)
                .withSamples(1500)
                .withSeed(mcSeed)
                .withConstantParameters_asian(const_bsm)));

        startTime = std::chrono::steady_clock::now();
        std::cout << "Asian Option price : " << discreteAsianOption.NPV() << std::endl;
        endTime = std::chrono::steady_clock::now();
        us = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        std::cout << "Elapsed time: " << us / 1000000 << " s" << std::endl;
 
        return 0;

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "unknown error" << std::endl;
        return 1;
    }
}


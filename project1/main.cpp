
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

using namespace QuantLib;

int main() {

    try {

        // modify the sample code below to suit your project

        Calendar calendar = TARGET();
        Date today = Date(24, February, 2022);
        Settings::instance().evaluationDate() = today;

        Option::Type type(Option::Put);
        Real underlying = 36;
        Real strike = 40;
        Date maturity(24, May, 2022);

        ext::shared_ptr <Exercise> europeanExercise(new EuropeanExercise(maturity));
        ext::shared_ptr <StrikedTypePayoff> payoff(new PlainVanillaPayoff(type, strike));

        Handle <Quote> underlyingH(ext::make_shared<SimpleQuote>(underlying));

        DayCounter dayCounter = Actual365Fixed();
        Handle <YieldTermStructure> riskFreeRate(
                ext::shared_ptr<YieldTermStructure>(
                        new ZeroCurve({today, today + 6 * Months}, {0.01, 0.015}, dayCounter)));
        Handle <BlackVolTermStructure> volatility(
                ext::shared_ptr<BlackVolTermStructure>(
                        new BlackVarianceCurve(today, {today + 3 * Months, today + 6 * Months}, {0.20, 0.25},
                                               dayCounter)));

        ext::shared_ptr <BlackScholesProcess> bsmProcess(
                new BlackScholesProcess(underlyingH, riskFreeRate, volatility));

        // options
        VanillaOption europeanOption(payoff, europeanExercise);

        Size timeSteps = 10;
        Size mcSeed = 42;
        std::vector<Real> NPVs_true;
        std::vector<Real> NPVs_false;
        double Tolerances[] = {1, 0.1, 0.01, 0.001};
        // If we modify the tolerance
        for (double tolerance: Tolerances) {

            // Constant Parameters are True
            std::cout<< "////////////////Constant parameters are TRUE//////////"<<std::endl;
            bool constantParameters = true;
            ext::shared_ptr <PricingEngine> mcengine;
            mcengine = MakeMCEuropeanEngine_2<PseudoRandom>(bsmProcess)
                    .withSteps(timeSteps)
                    .withAbsoluteTolerance(tolerance)
                    .withSeed(mcSeed)
                    .withConstantParameters(constantParameters);
            europeanOption.setPricingEngine(mcengine);


            auto startTime = std::chrono::steady_clock::now();

            Real NPV = europeanOption.NPV();

            auto endTime = std::chrono::steady_clock::now();

            double us = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

            std::cout<< "For a tolerance equal to "<< tolerance << std::endl;

            std::cout << "NPV: " << NPV << std::endl;
            std::cout << "Elapsed time: " << us / 1000000 << " s" << std::endl;
            std::cout << "Error estimation " << europeanOption.errorEstimate()<<std::endl;
            std::cout << "######################################################" << std::endl;
            NPVs_true.push_back(NPV);

            // Constant Parameters are False
            std::cout<< "////////////////Constant parameters are FALSE/////////"<<std::endl;
            constantParameters = false;
            ext::shared_ptr <PricingEngine> mcengineNonConstant;
            mcengineNonConstant = MakeMCEuropeanEngine_2<PseudoRandom>(bsmProcess)
                    .withSteps(timeSteps)
                    .withAbsoluteTolerance(tolerance)
                    .withSeed(mcSeed)
                    .withConstantParameters(constantParameters);
            europeanOption.setPricingEngine(mcengineNonConstant);


            auto startTime2 = std::chrono::steady_clock::now();

            Real NPV_nonConstant = europeanOption.NPV();

            auto endTime2 = std::chrono::steady_clock::now();

            double us2 = std::chrono::duration_cast<std::chrono::microseconds>(endTime2 - startTime2).count();

            std::cout<< "For a tolerance equal to "<< tolerance << std::endl;

            std::cout << "NPV_nonConstant: " << NPV_nonConstant << std::endl;
            std::cout << "Elapsed time: " << us2 / 1000000 << " s" << std::endl;
            std::cout << "Error estimation " << europeanOption.errorEstimate()<<std::endl;
            std::cout << "######################################################" << std::endl;
            NPVs_false.push_back(NPV_nonConstant);
        }

        // If we modify the number of samples
        Size Samples[] = {100, 1000, 10000};
        for (double sample: Samples) {

            // Constant Parameters are True
            std::cout<< "////////////////Constant parameters are TRUE//////////"<<std::endl;
            bool constantParameters = true;
            ext::shared_ptr <PricingEngine> mcengine;
            mcengine = MakeMCEuropeanEngine_2<PseudoRandom>(bsmProcess)
                    .withSteps(timeSteps)
                    .withSamples(sample)
                    .withSeed(mcSeed)
                    .withConstantParameters(constantParameters);

            europeanOption.setPricingEngine(mcengine);


            auto startTime = std::chrono::steady_clock::now();

            Real NPV = europeanOption.NPV();

            auto endTime = std::chrono::steady_clock::now();

            double us = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

            std::cout<< "For a number of samples equal to "<< sample << std::endl;

            std::cout << "NPV: " << NPV << std::endl;
            std::cout << "Elapsed time: " << us / 1000000 << " s" << std::endl;
            std::cout << "Error estimation " << europeanOption.errorEstimate()<<std::endl;

            std::cout << "######################################################" << std::endl;
            NPVs_true.push_back(NPV);

            // Constant Parameters are False
            std::cout<< "////////////////Constant parameters are FALSE/////////"<<std::endl;
            constantParameters = false;
            ext::shared_ptr <PricingEngine> mcengineNonConstant;
            mcengineNonConstant = MakeMCEuropeanEngine_2<PseudoRandom>(bsmProcess)
                    .withSteps(timeSteps)
                    .withSamples(sample)
                    .withSeed(mcSeed)
                    .withConstantParameters(constantParameters);
            europeanOption.setPricingEngine(mcengineNonConstant);


            auto startTime2 = std::chrono::steady_clock::now();

            Real NPV_nonConstant = europeanOption.NPV();

            auto endTime2 = std::chrono::steady_clock::now();

            double us2 = std::chrono::duration_cast<std::chrono::microseconds>(endTime2 - startTime2).count();

            std::cout<< "For a number of samples equal to "<< sample << std::endl;

            std::cout << "NPV_nonConstant: " << NPV_nonConstant << std::endl;
            std::cout << "Elapsed time: " << us2 / 1000000 << " s" << std::endl;
            std::cout << "Error estimation " << europeanOption.errorEstimate()<<std::endl;
            std::cout << "######################################################" << std::endl;
            NPVs_false.push_back(NPV_nonConstant);
        }
        return 0;
    }catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "unknown error" << std::endl;
        return 1;
    }
}



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


        /////////////////////////// Run MC with BS Constant Parameters /////////////////////
        bool isConstant = true;
        Size timeSteps = 10;
        Size mcSeed = 42;
        ext::shared_ptr<PricingEngine> mcengine_const;
        mcengine_const = MakeMCEuropeanEngine_2<PseudoRandom>(bsmProcess)
            .withSteps(timeSteps)
            .withAbsoluteTolerance(0.01)
            .withSeed(mcSeed)
            .isConstant(isConstant);
        europeanOption.setPricingEngine(mcengine_const);

        auto startTime_const = std::chrono::steady_clock::now();

        Real NPV_const = europeanOption.NPV();

        auto endTime_const = std::chrono::steady_clock::now();

        double us_const = std::chrono::duration_cast<std::chrono::microseconds>(endTime_const - startTime_const).count();

        std::cout << "NPV with Const BS param: " << NPV_const << std::endl;
        std::cout << "Elapsed time with Const BS param: " << us_const / 1000000 << " s" << std::endl;

        
        /////////////////////////// Run MC with BS Non Constant Parameters /////////////////////
        //bool isConstant = false;
        // Size timeSteps = 10;
        // Size mcSeed = 42;
        ext::shared_ptr<PricingEngine> mcengine_BS;
        mcengine_BS = MakeMCEuropeanEngine_2<PseudoRandom>(bsmProcess)
            .withSteps(timeSteps)
            .withAbsoluteTolerance(0.01)
            .withSeed(mcSeed);
        europeanOption.setPricingEngine(mcengine_BS);

        auto startTime = std::chrono::steady_clock::now();

        Real NPV = europeanOption.NPV();

        auto endTime = std::chrono::steady_clock::now();

        double us = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

        std::cout << "NPV with Non Const BS param: " << NPV << std::endl;
        std::cout << "Elapsed time with Non Const BS param: " << us / 1000000 << " s" << std::endl;















        return 0;

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "unknown error" << std::endl;
        return 1;
    }
}


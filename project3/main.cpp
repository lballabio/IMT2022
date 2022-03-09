
#include "binomialtree.hpp"
#include "binomialengine.hpp"
#include <ql/methods/lattices/binomialtree.hpp>
#include <ql/pricingengines/vanilla/binomialengine.hpp>
#include <ql/instruments/vanillaoption.hpp>
#include <ql/instruments/payoffs.hpp>
#include <ql/exercise.hpp>
#include <ql/termstructures/yield/zerocurve.hpp>
#include <ql/termstructures/volatility/equityfx/blackvariancecurve.hpp>
#include <ql/time/calendars/target.hpp>
#include <ql/utilities/dataformatters.hpp>
#include <iostream>
#include <chrono>

using namespace QuantLib;

int main() {

    try {

        Calendar calendar = TARGET();
        Date today = Date(24, February, 2022);
        Settings::instance().evaluationDate() = today;

        Option::Type type(Option::Put);
        Real underlying = 36;
        Real strike = 40;
        Date maturity(24, May, 2022);

        ext::shared_ptr<Exercise> americanExercise(new AmericanExercise(today, maturity));
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
        VanillaOption americanOption(payoff, americanExercise);
        Size timeSteps = 100;
        

        // JarrowRudd_2
        ext::shared_ptr<PricingEngine> engine(
                new BinomialVanillaEngine_2<JarrowRudd_2>(bsmProcess,timeSteps));
        americanOption.setPricingEngine(engine);
        auto startTime = std::chrono::steady_clock::now();
        Real NPV = americanOption.NPV();
        auto endTime = std::chrono::steady_clock::now();
        double us = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        Real delta = americanOption.delta();
        Real gamma = americanOption.gamma();
        std::cout << "------------------------" << std::endl;
        std::cout << "Tree : JarrowRudd_2" << std::endl;
        std::cout << "Net Present Value : " << NPV << std::endl;
        std::cout << "Delta: " << delta << std::endl;
        std::cout << "Gamma: " << gamma << std::endl;
        std::cout << "Elapsed time: " << us / 1000000 << " s" << std::endl;
        std::cout << "------------------------" << std::endl;
        

        // CoxRossRubinstein_2
        ext::shared_ptr<PricingEngine> engineCoxRossRubinstein_2(
                new BinomialVanillaEngine_2<CoxRossRubinstein_2>(bsmProcess,timeSteps));
        americanOption.setPricingEngine(engineCoxRossRubinstein_2);
        startTime = std::chrono::steady_clock::now();
        NPV = americanOption.NPV();
        endTime = std::chrono::steady_clock::now();
        us = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        delta = americanOption.delta();
        gamma = americanOption.gamma();
        std::cout << "------------------------" << std::endl;
        std::cout << "Tree : CoxRossRubinstein_2" << std::endl;
        std::cout << "Net Present Value : " << NPV << std::endl;
        std::cout << "Delta: " << delta << std::endl;
        std::cout << "Gamma: " << gamma << std::endl;
        std::cout << "Elapsed time: " << us / 1000000 << " s" << std::endl;
        std::cout << "------------------------" << std::endl;




        // AdditiveEQPBinomialTree_2
        ext::shared_ptr<PricingEngine> engineAdditiveEQPBinomialTree_2(
                new BinomialVanillaEngine_2<AdditiveEQPBinomialTree_2>(bsmProcess,timeSteps));
        americanOption.setPricingEngine(engineAdditiveEQPBinomialTree_2);
        startTime = std::chrono::steady_clock::now();
        NPV = americanOption.NPV();
        endTime = std::chrono::steady_clock::now();
        us = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        delta = americanOption.delta();
        gamma = americanOption.gamma();
        std::cout << "------------------------" << std::endl;
        std::cout << "Tree : AdditiveEQPBinomialTree_2" << std::endl;
        std::cout << "Net Present Value : " << NPV << std::endl;
        std::cout << "Delta: " << delta << std::endl;
        std::cout << "Gamma: " << gamma << std::endl;
        std::cout << "Elapsed time: " << us / 1000000 << " s" << std::endl;
        std::cout << "------------------------" << std::endl;




        // Trigeorgis_2
        ext::shared_ptr<PricingEngine> engineTrigeorgis_2(
                new BinomialVanillaEngine_2<Trigeorgis_2>(bsmProcess,timeSteps));
        americanOption.setPricingEngine(engineTrigeorgis_2);
        startTime = std::chrono::steady_clock::now();
        NPV = americanOption.NPV();
        endTime = std::chrono::steady_clock::now();
        us = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        delta = americanOption.delta();
        gamma = americanOption.gamma();
        std::cout << "------------------------" << std::endl;
        std::cout << "Tree : Trigeorgis_2" << std::endl;
        std::cout << "Net Present Value : " << NPV << std::endl;
        std::cout << "Delta: " << delta << std::endl;
        std::cout << "Gamma: " << gamma << std::endl;
        std::cout << "Elapsed time: " << us / 1000000 << " s" << std::endl;
        std::cout << "------------------------" << std::endl;



        // Tian_2
        ext::shared_ptr<PricingEngine> engineTian_2(
                new BinomialVanillaEngine_2<Tian_2>(bsmProcess,timeSteps));
        americanOption.setPricingEngine(engineTian_2);
        startTime = std::chrono::steady_clock::now();
        NPV = americanOption.NPV();
        endTime = std::chrono::steady_clock::now();
        us = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        delta = americanOption.delta();
        gamma = americanOption.gamma();
        std::cout << "------------------------" << std::endl;
        std::cout << "Tree : Tian_2" << std::endl;
        std::cout << "Net Present Value : " << NPV << std::endl;
        std::cout << "Delta: " << delta << std::endl;
        std::cout << "Gamma: " << gamma << std::endl;
        std::cout << "Elapsed time: " << us / 1000000 << " s" << std::endl;
        std::cout << "------------------------" << std::endl;


        // LeisenReimer_2
        ext::shared_ptr<PricingEngine> engineLeisenReimer_2(
                new BinomialVanillaEngine_2<LeisenReimer_2>(bsmProcess,timeSteps));
        americanOption.setPricingEngine(engineLeisenReimer_2);
        startTime = std::chrono::steady_clock::now();
        NPV = americanOption.NPV();
        endTime = std::chrono::steady_clock::now();
        us = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        delta = americanOption.delta();
        gamma = americanOption.gamma();
        std::cout << "------------------------" << std::endl;
        std::cout << "Tree : LeisenReimer_2" << std::endl;
        std::cout << "Net Present Value : " << NPV << std::endl;
        std::cout << "Delta: " << delta << std::endl;
        std::cout << "Gamma: " << gamma << std::endl;
        std::cout << "Elapsed time: " << us / 1000000 << " s" << std::endl;
        std::cout << "------------------------" << std::endl;


        // Joshi4_2
        ext::shared_ptr<PricingEngine> engineJoshi4_2(
                new BinomialVanillaEngine_2<Joshi4_2>(bsmProcess,timeSteps));
        americanOption.setPricingEngine(engineJoshi4_2);
        startTime = std::chrono::steady_clock::now();
        NPV = americanOption.NPV();
        endTime = std::chrono::steady_clock::now();
        us = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        delta = americanOption.delta();
        gamma = americanOption.gamma();
        std::cout << "------------------------" << std::endl;
        std::cout << "Tree : Joshi4_2" << std::endl;
        std::cout << "Net Present Value : " << NPV << std::endl;
        std::cout << "Delta: " << delta << std::endl;
        std::cout << "Gamma: " << gamma << std::endl;
        std::cout << "Elapsed time: " << us / 1000000 << " s" << std::endl;
        std::cout << "------------------------" << std::endl;



        return 0;

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "unknown error" << std::endl;
        return 1;
    }
}


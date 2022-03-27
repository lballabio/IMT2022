#ifndef quantlib_mc_discrete_arithmetic_average_price_asian_engine_hpp
#define quantlib_mc_discrete_arithmetic_average_price_asian_engine_hpp

#include <ql/exercise.hpp>
#include <ql/pricingengines/asian/analytic_discr_geom_av_price.hpp>
#include <ql/pricingengines/asian/mc_discr_geom_av_price.hpp>
#include <ql/processes/blackscholesprocess.hpp>
#include <utility>

namespace QuantLib {

    //!  Monte Carlo pricing engine for discrete arithmetic average price Asian
    /*!  Monte Carlo pricing engine for discrete arithmetic average price
         Asian options. It can use MCDiscreteGeometricAPEngine (Monte Carlo
         discrete arithmetic average price engine) and
         AnalyticDiscreteGeometricAveragePriceAsianEngine (analytic discrete
         arithmetic average price engine) for control variation.
         \ingroup asianengines
         \test the correctness of the returned value is tested by
               reproducing results available in literature.
    */
    template <class RNG = PseudoRandom, class S = Statistics>
    class MCDiscreteArithmeticAPEngine
        : public MCDiscreteAveragingAsianEngineBase<SingleVariate,RNG,S> {
      public:
        typedef
        typename MCDiscreteAveragingAsianEngineBase<SingleVariate,RNG,S>::path_generator_type
            path_generator_type;
        typedef
        typename MCDiscreteAveragingAsianEngineBase<SingleVariate,RNG,S>::path_pricer_type
            path_pricer_type;
        typedef typename MCDiscreteAveragingAsianEngineBase<SingleVariate,RNG,S>::stats_type
            stats_type;
        // constructor
        MCDiscreteArithmeticAPEngine(
             const ext::shared_ptr<GeneralizedBlackScholesProcess>& process,
             bool brownianBridge,
             bool antitheticVariate,
             bool controlVariate,
             Size requiredSamples,
             Real requiredTolerance,
             Size maxSamples,
             BigNatural seed,
             bool constantParameters_asian);

        bool constantParameters_asian;

        ext::shared_ptr<path_generator_type> pathGenerator() const override
        {
            Size dimensions = MCDiscreteAveragingAsianEngineBase<SingleVariate,RNG,S>::process_->factors();
            TimeGrid grid = this->timeGrid();
            typename RNG::rsg_type generator = RNG::make_sequence_generator(dimensions*(grid.size()-1),MCDiscreteAveragingAsianEngineBase<SingleVariate,RNG,S>::seed_);

            if(this->constantParameters_asian)
            {
                ext::shared_ptr<GeneralizedBlackScholesProcess> bs_process = ext::dynamic_pointer_cast<GeneralizedBlackScholesProcess>(this->process_);
                Time sampling_time = grid.back(); // Get the reference to the last element in the vector timeGrid (so the latest moment)
                float strike = ext::dynamic_pointer_cast<StrikedTypePayoff>(MCDiscreteAveragingAsianEngineBase<SingleVariate, RNG, S>::arguments_.payoff)->strike();
                float risk_free_rate = bs_process->riskFreeRate()->zeroRate(sampling_time, Continuous);
                float dividends = bs_process->dividendYield()->zeroRate(sampling_time, Continuous);
                float volatility = bs_process->blackVolatility()->blackVol(sampling_time, strike);
                float underlying_value = bs_process->x0();

                ext::shared_ptr<ConstantBlackScholesProcess> cst_bs_process(new ConstantBlackScholesProcess(underlying_value, risk_free_rate, volatility, dividends));

                return ext::shared_ptr<path_generator_type>(new path_generator_type(cst_bs_process, grid, generator, MCDiscreteAveragingAsianEngineBase<SingleVariate, RNG, S>::brownianBridge_));
            }
            else
            {
                return ext::shared_ptr<path_generator_type>(new path_generator_type(MCDiscreteAveragingAsianEngineBase<SingleVariate, RNG, S>::process_, grid, generator, MCDiscreteAveragingAsianEngineBase<SingleVariate, RNG, S>::brownianBridge_));
            }
        }
      protected:
        ext::shared_ptr<path_pricer_type> pathPricer() const override;
        ext::shared_ptr<path_pricer_type> controlPathPricer() const override;
        ext::shared_ptr<PricingEngine> controlPricingEngine() const override {
            ext::shared_ptr<GeneralizedBlackScholesProcess> process =
                ext::dynamic_pointer_cast<GeneralizedBlackScholesProcess>(
                    this->process_);
            QL_REQUIRE(process, "Black-Scholes process required");
            return ext::shared_ptr<PricingEngine>(new
                AnalyticDiscreteGeometricAveragePriceAsianEngine(process));
        }
    };


    class ArithmeticAPOPathPricer : public PathPricer<Path> {
      public:
        ArithmeticAPOPathPricer(Option::Type type,
                                Real strike,
                                DiscountFactor discount,
                                Real runningSum = 0.0,
                                Size pastFixings = 0);
        Real operator()(const Path& path) const override;

      private:
        PlainVanillaPayoff payoff_;
        DiscountFactor discount_;
        Real runningSum_;
        Size pastFixings_;
    };


    // inline definitions

    template <class RNG, class S>
    inline
    MCDiscreteArithmeticAPEngine<RNG,S>::MCDiscreteArithmeticAPEngine(
             const ext::shared_ptr<GeneralizedBlackScholesProcess>& process,
             bool brownianBridge,
             bool antitheticVariate,
             bool controlVariate,
             Size requiredSamples,
             Real requiredTolerance,
             Size maxSamples,
             BigNatural seed,
             bool constantParameters_asian)
    : MCDiscreteAveragingAsianEngineBase<SingleVariate,RNG,S>(process,
                                                              brownianBridge,
                                                              antitheticVariate,
                                                              controlVariate,
                                                              requiredSamples,
                                                              requiredTolerance,
                                                              maxSamples,
                                                              seed) {
                                                                  this->constantParameters_asian = constantParameters_asian;
                                                              }

    template <class RNG, class S>
    inline
    ext::shared_ptr<
            typename MCDiscreteArithmeticAPEngine<RNG,S>::path_pricer_type>
        MCDiscreteArithmeticAPEngine<RNG,S>::pathPricer() const {

        ext::shared_ptr<PlainVanillaPayoff> payoff =
            ext::dynamic_pointer_cast<PlainVanillaPayoff>(
                this->arguments_.payoff);
        QL_REQUIRE(payoff, "non-plain payoff given");

        ext::shared_ptr<EuropeanExercise> exercise =
            ext::dynamic_pointer_cast<EuropeanExercise>(
                this->arguments_.exercise);
        QL_REQUIRE(exercise, "wrong exercise given");

        ext::shared_ptr<GeneralizedBlackScholesProcess> process =
            ext::dynamic_pointer_cast<GeneralizedBlackScholesProcess>(
                this->process_);
        QL_REQUIRE(process, "Black-Scholes process required");

        return ext::shared_ptr<typename
            MCDiscreteArithmeticAPEngine<RNG,S>::path_pricer_type>(
                new ArithmeticAPOPathPricer(
                    payoff->optionType(),
                    payoff->strike(),
                    process->riskFreeRate()->discount(exercise->lastDate()),
                    this->arguments_.runningAccumulator,
                    this->arguments_.pastFixings));
    }

    template <class RNG, class S>
    inline
    ext::shared_ptr<
            typename MCDiscreteArithmeticAPEngine<RNG,S>::path_pricer_type>
        MCDiscreteArithmeticAPEngine<RNG,S>::controlPathPricer() const {

        ext::shared_ptr<PlainVanillaPayoff> payoff =
            ext::dynamic_pointer_cast<PlainVanillaPayoff>(
                this->arguments_.payoff);
        QL_REQUIRE(payoff, "non-plain payoff given");

        ext::shared_ptr<EuropeanExercise> exercise =
            ext::dynamic_pointer_cast<EuropeanExercise>(
                this->arguments_.exercise);
        QL_REQUIRE(exercise, "wrong exercise given");

        ext::shared_ptr<GeneralizedBlackScholesProcess> process =
            ext::dynamic_pointer_cast<GeneralizedBlackScholesProcess>(
                this->process_);
        QL_REQUIRE(process, "Black-Scholes process required");

        // for seasoned option the geometric strike might be rescaled
        // to obtain an equivalent arithmetic strike.
        // Any change applied here MUST be applied to the analytic engine too
        return ext::shared_ptr<typename
            MCDiscreteArithmeticAPEngine<RNG,S>::path_pricer_type>(
            new GeometricAPOPathPricer(
              payoff->optionType(),
              payoff->strike(),
              process->riskFreeRate()->discount(this->timeGrid().back())));
    }

    template <class RNG = PseudoRandom, class S = Statistics>
    class MakeMCDiscreteArithmeticAPEngine {
      public:
        explicit MakeMCDiscreteArithmeticAPEngine(
            ext::shared_ptr<GeneralizedBlackScholesProcess> process);
        // named parameters
        MakeMCDiscreteArithmeticAPEngine& withBrownianBridge(bool b = true);
        MakeMCDiscreteArithmeticAPEngine& withSamples(Size samples);
        MakeMCDiscreteArithmeticAPEngine& withAbsoluteTolerance(Real tolerance);
        MakeMCDiscreteArithmeticAPEngine& withMaxSamples(Size samples);
        MakeMCDiscreteArithmeticAPEngine& withSeed(BigNatural seed);
        MakeMCDiscreteArithmeticAPEngine& withAntitheticVariate(bool b = true);
        MakeMCDiscreteArithmeticAPEngine& withControlVariate(bool b = true);
        MakeMCDiscreteArithmeticAPEngine& withConstantParameters_asian(bool b = false);
        // conversion to pricing engine
        operator ext::shared_ptr<PricingEngine>() const;
      private:
        ext::shared_ptr<GeneralizedBlackScholesProcess> process_;
        bool antithetic_, controlVariate_;
        Size samples_, maxSamples_;
        Real tolerance_;
        bool brownianBridge_;
        BigNatural seed_;
        bool constantParameters_asian;
    };

    template <class RNG, class S>
    inline MakeMCDiscreteArithmeticAPEngine<RNG, S>::MakeMCDiscreteArithmeticAPEngine(
        ext::shared_ptr<GeneralizedBlackScholesProcess> process)
    : process_(std::move(process)), antithetic_(false), controlVariate_(false),
      samples_(Null<Size>()), maxSamples_(Null<Size>()), tolerance_(Null<Real>()),
      brownianBridge_(true), seed_(0), constantParameters_asian(false) {}

    template <class RNG, class S>
    inline MakeMCDiscreteArithmeticAPEngine<RNG,S>&
    MakeMCDiscreteArithmeticAPEngine<RNG,S>::withSamples(Size samples) {
        QL_REQUIRE(tolerance_ == Null<Real>(),
                   "tolerance already set");
        samples_ = samples;
        return *this;
    }

    template <class RNG, class S>
    inline MakeMCDiscreteArithmeticAPEngine<RNG,S>&
    MakeMCDiscreteArithmeticAPEngine<RNG,S>::withConstantParameters_asian(bool constantParameters_asian) {
        this->constantParameters_asian = constantParameters_asian;
        return *this;
    }

    template <class RNG, class S>
    inline MakeMCDiscreteArithmeticAPEngine<RNG,S>&
    MakeMCDiscreteArithmeticAPEngine<RNG,S>::withAbsoluteTolerance(
                                                             Real tolerance) {
        QL_REQUIRE(samples_ == Null<Size>(),
                   "number of samples already set");
        QL_REQUIRE(RNG::allowsErrorEstimate,
                   "chosen random generator policy "
                   "does not allow an error estimate");
        tolerance_ = tolerance;
        return *this;
    }

    template <class RNG, class S>
    inline MakeMCDiscreteArithmeticAPEngine<RNG,S>&
    MakeMCDiscreteArithmeticAPEngine<RNG,S>::withMaxSamples(Size samples) {
        maxSamples_ = samples;
        return *this;
    }

    template <class RNG, class S>
    inline MakeMCDiscreteArithmeticAPEngine<RNG,S>&
    MakeMCDiscreteArithmeticAPEngine<RNG,S>::withSeed(BigNatural seed) {
        seed_ = seed;
        return *this;
    }

    template <class RNG, class S>
    inline MakeMCDiscreteArithmeticAPEngine<RNG,S>&
    MakeMCDiscreteArithmeticAPEngine<RNG,S>::withBrownianBridge(bool b) {
        brownianBridge_ = b;
        return *this;
    }

    template <class RNG, class S>
    inline MakeMCDiscreteArithmeticAPEngine<RNG,S>&
    MakeMCDiscreteArithmeticAPEngine<RNG,S>::withAntitheticVariate(bool b) {
        antithetic_ = b;
        return *this;
    }

    template <class RNG, class S>
    inline MakeMCDiscreteArithmeticAPEngine<RNG,S>&
    MakeMCDiscreteArithmeticAPEngine<RNG,S>::withControlVariate(bool b) {
        controlVariate_ = b;
        return *this;
    }

    template <class RNG, class S>
    inline
    MakeMCDiscreteArithmeticAPEngine<RNG,S>::operator ext::shared_ptr<PricingEngine>()
                                                                      const {
        return ext::shared_ptr<PricingEngine>(new
            MCDiscreteArithmeticAPEngine<RNG,S>(process_,
                                                brownianBridge_,
                                                antithetic_, controlVariate_,
                                                samples_, tolerance_,
                                                maxSamples_,
                                                seed_,
                                                constantParameters_asian));
    }



}


#endif
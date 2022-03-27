#include <ql/pricingengines/asian/mc_discr_geom_av_price.hpp>
#include <ql/pricingengines/asian/mc_discr_arith_av_price.hpp>

namespace QuantLib {

    ArithmeticAPOPathPricer::ArithmeticAPOPathPricer(
                                         Option::Type type,
                                         Real strike, DiscountFactor discount,
                                         Real runningSum, Size pastFixings)
    : payoff_(type, strike), discount_(discount),
      runningSum_(runningSum), pastFixings_(pastFixings) {
        QL_REQUIRE(strike>=0.0,
            "strike less than zero not allowed");
    }

    Real ArithmeticAPOPathPricer::operator()(const Path& path) const  {
        Size n = path.length();
        QL_REQUIRE(n>1, "the path cannot be empty");

        Real sum;
        Size fixings;
        if (path.timeGrid().mandatoryTimes()[0]==0.0) {
            // include initial fixing
            sum = std::accumulate(path.begin(),path.end(),runningSum_);
            fixings = pastFixings_ + n;
        } else {
            sum = std::accumulate(path.begin()+1,path.end(),runningSum_);
            fixings = pastFixings_ + n - 1;
        }
        Real averagePrice = sum/fixings;
        return discount_ * payoff_(averagePrice);
    }

}
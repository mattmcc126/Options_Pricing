#include "options_classes.hpp"
#include <vector>
#include <memory>
#include <cmath>
#include <stdexcept>


// Utility function for BS method
double normalCDF(double x) {
    return 0.5 * (1 + erf(x / sqrt(2.0)));
}

// Function with function overriding definitions to calculate price given const option and const data

// BS
double BlackScholesPricer::calculatePrice(const Option& option, const MarketData& marketData) {
    // Extract parameters
    double S = marketData.getSpot();           
    double K = option.getStrike();             
    double r = marketData.getRiskFreeRate();   
    double sigma = marketData.getVolatility(); 
    double T = option.getExpiry();             
    
    // Check if option style is European
    if (option.getStyle() != Option::Style::EUROPEAN) {
        throw std::invalid_argument("Black-Scholes model only works for European options");
    }

    // Calculate d1 and d2
    double d1 = (log(S/K) + (r + sigma*sigma/2)*T) / (sigma*sqrt(T));
    double d2 = d1 - sigma*sqrt(T);
    
    double price = 0.0;
    
    // Calculate price based on option type
    if (option.getType() == Option::Type::CALL) {
        price = S * normalCDF(d1) - K * exp(-r*T) * normalCDF(d2);
    } else { // Put option
        price = K * exp(-r*T) * normalCDF(-d2) - S * normalCDF(-d1);
    }
    
    return price;
}

// BP
double BinomialPricer::calculatePrice(const Option& option, const MarketData& marketData) {
    // Implementation would include:
    // 1. Build binomial tree
    // 2. Calculate option value at each node
    // 3. Work backwards to get present value
    return 0.0;
}




// Statistical Calculation methods, will return the AnalysisResult structure after preforming calculations
StatisticalAnalyzer::AnalysisResult StatisticalAnalyzer::analyzePricingDifference(const std::vector<double>& prices1, const std::vector<double>& prices2) {
    // Implementation would:
    // 1. Calculate mean difference
    // 2. Perform statistical tests (t-test or similar)
    // 3. Calculate confidence intervals
    // 4. Return analysis results
    return AnalysisResult{0.0, 0.0, false};
}



// 
TradingDecision::Action TradingDecision::makeDecision(const Option& option, const std::vector<std::unique_ptr<PricingStrategy>>& strategies, const MarketData& marketData, const StatisticalAnalyzer& analyzer)
{
    // Implementation would:
    // 1. Get prices from all strategies
    // 2. Analyze statistical significance
    // 3. Compare with market price
    // 4. Make trading decision based on analysis
    // 5. Return an action
}

/*
Potential for future implementation
-----



*/
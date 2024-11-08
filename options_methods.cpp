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
    // Extract parameters
    double S = marketData.getSpot();
    double K = option.getStrike();
    double r = marketData.getRiskFreeRate();
    double sigma = marketData.getVolatility();
    double T = option.getExpiry();

    // Number of time steps (can be adjusted for accuracy vs. performance)
    const int N = 100;
    double dt = T/N;

    // Calculate up and down factors
    double u = exp(sigma * sqrt(dt));
    double d = 1.0/u;

    // Calculate risk-neutral probability
    double p = (exp(r * dt) - d)/(u - d);

    // Initialize price tree
    std::vector<std::vector<double>> priceTree(N + 1);
    std::vector<std::vector<double>> optionTree(N + 1);

    // Initialize size of each time step in the tree
    for(int i = 0; i <= N; i++) {
        priceTree[i].resize(i + 1);
        optionTree[i].resize(i + 1);
    }

    // Build price tree forward
    priceTree[0][0] = S;
    for(int i = 1; i <= N; i++) {
        for(int j = 0; j <= i; j++) {
            int up_moves = j;
            int down_moves = i - j;
            priceTree[i][j] = S * pow(u, up_moves) * pow(d, down_moves);
        }
    }

    // Initialize option values at expiration
    for(int j = 0; j <= N; j++) {
        if(option.getType() == Option::Type::CALL) {
            optionTree[N][j] = std::max(0.0, priceTree[N][j] - K);
        } else {
            optionTree[N][j] = std::max(0.0, K - priceTree[N][j]);
        }
    }

// Work backwards through the tree
    for(int i = N-1; i >= 0; i--) {
        for(int j = 0; j <= i; j++) {
            // Calculate holding value (European case)
            double holding_value = exp(-r * dt) * (p * optionTree[i+1][j+1] + (1-p) * optionTree[i+1][j]);

            if(option.getStyle() == Option::Style::AMERICAN) {
                // For American options, compare with immediate exercise value
                double exercise_value;
                if(option.getType() == Option::Type::CALL) {
                    exercise_value = std::max(0.0, priceTree[i][j] - K);
                } else {
                    exercise_value = std::max(0.0, K - priceTree[i][j]);
                }

                optionTree[i][j] = std::max(holding_value, exercise_value);
                } else {
                optionTree[i][j] = holding_value;
            }
        }
    }
    // Return price (root of the option tree)
    return optionTree[0][0];
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
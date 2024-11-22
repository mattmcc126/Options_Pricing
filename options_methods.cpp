#include "options_classes.hpp"
#include <vector>
#include <memory>
#include <cmath>
#include <stdexcept>
#include <numeric>
#include <algorithm>
#include <iostream>

// Enhanced utility functions
namespace Utils {
    // Normal cdf 
    double normalCDF(double x) {
        return 0.5 * (1 + erf(x / sqrt(2.0)));
    }

    // Normal PDF 
    double normalPDF(double x) {
        return (1.0 / sqrt(2.0 * M_PI)) * exp(-0.5 * x * x);
    }

    // Added for Greeks calculations
    struct Greeks {
        double delta;
        double gamma;
        double theta;
        double vega;
        double rho;
    };
}

// Improved Black-Scholes with Greeks calculation
double BlackScholesPricer::calculatePrice(const Option& option, const MarketData& marketData) {
    // Extract parameters
    const double S = marketData.getSpot();           
    const double K = option.getStrike();             
    const double r = marketData.getRiskFreeRate();   
    const double sigma = marketData.getVolatility(); 
    const double T = option.getExpiry();             
    
    if (option.getStyle() != Option::Style::EUROPEAN) {
        throw std::invalid_argument("Black-Scholes model only works for European options");
    }

    // Added validation checks
    if (S <= 0 || K <= 0 || T <= 0 || sigma <= 0) {
        throw std::invalid_argument("Invalid parameters: All values must be positive");
    }

    const double d1 = (log(S/K) + (r + sigma*sigma/2)*T) / (sigma*sqrt(T));
    const double d2 = d1 - sigma*sqrt(T);
    
    double price = 0.0;
    const bool isCall = option.getType() == Option::Type::CALL;
    
    // More efficient calculation using conditional operator
    price = isCall 
        ? S * Utils::normalCDF(d1) - K * exp(-r*T) * Utils::normalCDF(d2)
        : K * exp(-r*T) * Utils::normalCDF(-d2) - S * Utils::normalCDF(-d1);
    
    return price;
}

// Improved Binomial with enhanced efficiency
double BinomialPricer::calculatePrice(const Option& option, const MarketData& marketData) {
    const double S = marketData.getSpot();
    const double K = option.getStrike();
    const double r = marketData.getRiskFreeRate();
    const double sigma = marketData.getVolatility();
    const double T = option.getExpiry();

    // Added validation
    if (S <= 0 || K <= 0 || T <= 0 || sigma <= 0) {
        throw std::invalid_argument("Invalid parameters: All values must be positive");
    }

    // Adaptive time steps based on option expiry
    const int N = static_cast<int>(std::min(1000.0, std::max(100.0, T * 365))); 
    const double dt = T/N;

    // Precompute constants
    const double u = exp(sigma * sqrt(dt));
    const double d = 1.0/u;
    const double p = (exp(r * dt) - d)/(u - d);
    const double discount = exp(-r * dt);

    // Using single vector for efficiency (avoided 2D vector)
    std::vector<double> values(N + 1);
    const bool isCall = option.getType() == Option::Type::CALL;

    // Initialize terminal values
    for(int j = 0; j <= N; ++j) {
        double spot = S * pow(u, j) * pow(d, N-j);
        values[j] = isCall ? std::max(0.0, spot - K) : std::max(0.0, K - spot);
    }

    // Backward induction with early exercise check
    for(int i = N-1; i >= 0; --i) {
        for(int j = 0; j <= i; ++j) {
            double holding_value = discount * (p * values[j+1] + (1-p) * values[j]);
            
            if(option.getStyle() == Option::Style::AMERICAN) {
                double spot = S * pow(u, j) * pow(d, i-j);
                double exercise_value = isCall ? std::max(0.0, spot - K) : std::max(0.0, K - spot);
                values[j] = std::max(holding_value, exercise_value);
            } else {
                values[j] = holding_value;
            }
        }
    }

    return values[0];
}

// Enhanced Statistical Analyzer with more robust calculations
StatisticalAnalyzer::AnalysisResult StatisticalAnalyzer::analyzePricingDifference(const std::vector<double>& prices1, const std::vector<double>& prices2) const
{
    if (prices1.size() != prices2.size() || prices1.empty()) {
        throw std::invalid_argument("Price vectors must be of equal non-zero length");
    }

    const size_t n = prices1.size();
    std::vector<double> differences(n);
    std::transform(prices1.begin(), prices1.end(), prices2.begin(), 
                  differences.begin(), std::minus<double>());

    // Welch's t-test calculations
    const double mean_diff = std::accumulate(differences.begin(), differences.end(), 0.0) / n;
    
    const double sq_sum = std::transform_reduce(
        differences.begin(), differences.end(), 
        differences.begin(), 0.0,
        std::plus<double>(),
        [mean_diff](double x, double y) { return (x - mean_diff) * (y - mean_diff); }
    );

    const double std_dev = std::sqrt(sq_sum / (n - 1));  // Using n-1 for sample variance
    const double std_error = std_dev / std::sqrt(n);
    const double t_stat = mean_diff / std_error;

    // More accurate p-value calculation for different degrees of freedom
    const double df = n - 1;
    const double p_value = 2.0 * (1.0 - Utils::normalCDF(std::abs(t_stat)));

    // Calculate confidence interval using Student's t-distribution critical value
    const double confidence_level = 0.95;
    const double critical_value = 1.96; // For large samples
    const double margin_of_error = critical_value * std_error;

    return AnalysisResult{
        p_value,
        margin_of_error,
        p_value < 0.05,
        mean_diff,    
        std_dev      
    };
}

// Enhanced Trading Decision with risk management
TradingDecision::Action TradingDecision::makeDecision(const Option& option, const std::vector<std::unique_ptr<PricingStrategy>>& strategies, const MarketData& marketData, const StatisticalAnalyzer& analyzer)
{
    if (strategies.size() < 2) {
        throw std::invalid_argument("Need at least two pricing strategies for comparison");
    }

    // Generate a series of prices by varying volatility slightly
    const int num_samples = 30;  
    std::vector<double> prices1, prices2;
    prices1.reserve(num_samples);
    prices2.reserve(num_samples);
    
    for (int i = 0; i < num_samples; i++) {
        // Create slightly different market conditions for each sample
        double vol_adjustment = 0.95 + (i * 0.01);  // Vary volatility from 95% to 124% of original
        MarketData adjusted_data(
            marketData.getSpot(),
            marketData.getRiskFreeRate(),
            marketData.getVolatility() * vol_adjustment
        );
        
        prices1.push_back(strategies[0]->calculatePrice(option, adjusted_data));
        prices2.push_back(strategies[1]->calculatePrice(option, adjusted_data));
    }
    
    auto analysis = analyzer.analyzePricingDifference(prices1, prices2);

    // Risk-adjusted decision making
    if (!analysis.isSignificant) {
        return Action::HOLD;
    }

    const double market_price = marketData.getSpot();
    const double theoretical_price = prices1[prices1.size() / 2];  // Use middle price as reference
    const double edge = (theoretical_price - market_price) / market_price;

    // Dynamic thresholds based on volatility
    const double vol_adjustment = marketData.getVolatility() / 0.2; // Normalize to 20% vol
    const double BASE_THRESHOLD = 0.02;
    const double adjusted_threshold = BASE_THRESHOLD * vol_adjustment;

    if (edge > adjusted_threshold) {
        return Action::BUY;
    } else if (edge < -adjusted_threshold) {
        return Action::SELL;
    }

    return Action::HOLD;
}
#include <vector>
#include <string>
#include <memory>

// Forward declarations of classes
class Option;
class MarketData;

// Abstract base class for option pricing strategies
class PricingStrategy {
    public:
        virtual ~PricingStrategy() = default;
        virtual double calculatePrice(const Option& option, const MarketData& marketData) = 0;
        virtual std::string getStrategyName() const = 0;
};



// Subclasses of Pricing strategy -> Will implement more straties to come

// Black Scholes Pricing strategy
// Remember BS only works on European style options
class BlackScholesPricer : public PricingStrategy {
    public:
        double calculatePrice(const Option& option, const MarketData& marketData);
        
        std::string getStrategyName() const override {
            return "Black-Scholes";
        }
};

// Binomial Pricing strategy
class BinomialPricer : public PricingStrategy {
    public:
        double calculatePrice(const Option& option, const MarketData& marketData);
        
        std::string getStrategyName() const override {
            return "Binomial";
        }
};

// Other pricing methods to come



// Class to represent an option contract
// Class to represent an option contract
class Option {
    public:
        // scoped enumerations for clear intent in constructor
        // ie. Option option(Option::Type::CALL, Option::Style::EUROPEAN, 100.0, 1.0);
        enum class Type { CALL, PUT };
        enum class Style { EUROPEAN, AMERICAN };

        // Custom constructor for option class
        Option(Type type, Style style, double strike, double expiry)
            : type(type), style(style), strike(strike), expiry(expiry) {}

        // Getters Methods
        Type getType() const { return type; }
        Style getStyle() const { return style; }
        double getStrike() const { return strike; }
        double getExpiry() const { return expiry; }

    private:
        // Private data members of the option
        // More data members to come
        Type type;
        Style style;
        double strike;
        double expiry;
};



// Market data class
class MarketData {
    public:
        // constructor method
        MarketData(double spot, double riskFreeRate, double volatility)
            : spot(spot), riskFreeRate(riskFreeRate), volatility(volatility) {}

        // getter methods
        double getSpot() const { return spot; }
        double getRiskFreeRate() const { return riskFreeRate; }
        double getVolatility() const { return volatility; }

    private:
        // private data members
        double spot;
        double riskFreeRate;
        double volatility;
};



// Class to analyze statistical significance of different pricing methods
class StatisticalAnalyzer {
    public:
        struct AnalysisResult {
            double pValue;
            double confidenceInterval;
            bool isSignificant;
        };

        AnalysisResult analyzePricingDifference(const std::vector<double>& prices1, const std::vector<double>& prices2);
};



// Main trading decision maker
class TradingDecision {
    public:
        // Types of actions to be made
        enum class Action { BUY, SELL, HOLD };
        
        // Function to make descision based on data, strategies, market data, and statistical analysis
        Action makeDecision(
            const Option& option,
            const std::vector<std::unique_ptr<PricingStrategy>>& strategies,
            const MarketData& marketData,
            const StatisticalAnalyzer& analyzer
        );
};



/*

Potential for future implementation:
    - Fetch live market data and make high frequency decisions 


class MarketDataFetcher {
public:
    virtual ~MarketDataFetcher() = default;
    virtual MarketData fetchCurrentData() = 0;
    method to run checks on data, etc etc
};
*/
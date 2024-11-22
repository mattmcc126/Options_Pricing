#include "options_classes.hpp"
#include <iostream>
#include <iomanip>
#include <limits>

// Utility function to clear input buffering
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Function to get valid numerical inputs from user
double getNumericInput(const std::string& prompt, double min = 0.0, double max = std::numeric_limits<double>::max()) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= min && value <= max) {
            clearInputBuffer();
            return value;
        }
        std::cout << "Invalid input. Please enter a number between " << min << " and " << max << std::endl;
        clearInputBuffer();
    }
}

// Function to get option type from user
Option::Type getOptionType() {
    while (true) {
        std::cout << "\nSelect option type:\n"
                  << "1. Call\n"
                  << "2. Put\n"
                  << "Choice: ";
        int choice;
        if (std::cin >> choice) {
            clearInputBuffer();
            if (choice == 1) return Option::Type::CALL;
            if (choice == 2) return Option::Type::PUT;
        }
        std::cout << "Invalid choice. Please select 1 or 2.\n";
        clearInputBuffer();
    }
}

// Function to get option style from user
Option::Style getOptionStyle() {
    while (true) {
        std::cout << "\nSelect option style:\n"
                  << "1. European\n"
                  << "2. American\n"
                  << "Choice: ";
        int choice;
        if (std::cin >> choice) {
            clearInputBuffer();
            if (choice == 1) return Option::Style::EUROPEAN;
            if (choice == 2) return Option::Style::AMERICAN;
        }
        std::cout << "Invalid choice. Please select 1 or 2.\n";
        clearInputBuffer();
    }
}

std::string getDecisionMessage(TradingDecision::Action action, const Option& option, const MarketData& marketData,
                             const StatisticalAnalyzer::AnalysisResult& analysis) {
    // Getters 
    double spot = marketData.getSpot();
    double strike = option.getStrike();
    double vol = marketData.getVolatility();
    double expiry = option.getExpiry();
    bool isCall = option.getType() == Option::Type::CALL;
    
    // Message to be returned
    std::stringstream message;
    message << "\nTrading Decision Analysis:\n";
    
    // Added statistical confidence information
    message << "Statistical Confidence:\n"
           << "- Mean Price Difference: " << std::fixed << std::setprecision(4) 
           << analysis.mean_difference << "\n"
           << "- Standard Deviation: " << analysis.standard_deviation << "\n"
           << "- P-Value: " << analysis.pValue << "\n"
           << "- Confidence Interval: ±" << analysis.confidenceInterval << "\n\n";

    // Calculate price edge
    double edge = analysis.mean_difference / spot;
    
    // Switch to determine output
    switch(action) {
        case (TradingDecision::Action::BUY):
            message << "RECOMMENDATION: BUY";
            if (analysis.isSignificant) {
                message << " (Statistically Significant)\n";
                message << "Analysis shows consistent price discrepancy of " 
                       << std::abs(edge * 100) << "% from theoretical value.\n";
            } else {
                message << " (Limited Statistical Confidence)\n";
            }
            
            if (isCall) {
                message << "Call option analysis indicates:\n"
                       << "- Volatility study across " << (vol * 0.95 * 100) << "% to " 
                       << (vol * 1.24 * 100) << "% shows persistent undervaluation\n"
                       << "- Edge exceeds volatility-adjusted threshold";
            } else {
                message << "Put option analysis indicates:\n"
                       << "- Downside protection value exceeds current premium\n"
                       << "- Statistical edge maintained across volatility scenarios";
            }
            break;
            
        case (TradingDecision::Action::SELL):
            message << "RECOMMENDATION: SELL";
            if (analysis.isSignificant) {
                message << " (Statistically Significant)\n";
                message << "Model indicates overvaluation of " 
                       << std::abs(edge * 100) << "% compared to theoretical price.\n";
            } else {
                message << " (Limited Statistical Confidence)\n";
            }
            
            if (isCall) {
                message << "Call option analysis indicates:\n"
                       << "- Premium exceeds theoretical value across " 
                       << (vol * 0.95 * 100) << "% to " << (vol * 1.24 * 100) 
                       << "% volatility range\n"
                       << "- Edge persists beyond statistical noise";
            } else {
                message << "Put option analysis indicates:\n"
                       << "- Current premium exceeds statistical fair value\n"
                       << "- Volatility scenarios suggest favorable selling conditions";
            }
            break;
            
        case (TradingDecision::Action::HOLD):
            message << "RECOMMENDATION: HOLD\n";
            message << "Statistical analysis indicates:\n"
                   << "- Price difference not statistically significant (p-value: " 
                   << analysis.pValue << ")\n"
                   << "- Edge of " << std::abs(edge * 100) 
                   << "% below volatility-adjusted threshold\n"
                   << "- Price within expected theoretical range across volatility scenarios "
                   << (vol * 0.95 * 100) << "% to " << (vol * 1.24 * 100) << "%";
            break;
    }
    
    return message.str();
}

// Based on function above, print the decision
void printDecision(TradingDecision::Action action, const Option& option, const MarketData& marketData, 
                  const StatisticalAnalyzer::AnalysisResult& analysis) {
    std::cout << getDecisionMessage(action, option, marketData, analysis) << std::endl;
}

int main() {
    // Main with new error handling and detailed errors
    try {
        std::cout << "=== Options Pricing Calculator ===\n\n";

        // Get market data from user
        std::cout << "Enter Market Data:\n";
        double spot = getNumericInput("Spot Price: ", 0.01);
        double rate = getNumericInput("Risk-free Rate (as decimal, e.g., 0.05 for 5%): ", 0.0, 1.0);
        double vol = getNumericInput("Volatility (as decimal, e.g., 0.2 for 20%): ", 0.0, 1.0);

        // Create market data object
        MarketData marketData(spot, rate, vol);

        // Get option parameters from user
        Option::Type type = getOptionType();
        Option::Style style = getOptionStyle();
        
        double strike = getNumericInput("Strike Price: ", 0.01);
        double expiry = getNumericInput("Time to Expiry (in years): ", 0.0, 100.0);

        // Create option object
        Option option(type, style, strike, expiry);

        // Create pricing strategies
        std::vector<std::unique_ptr<PricingStrategy>> strategies;
        strategies.push_back(std::make_unique<BlackScholesPricer>());
        strategies.push_back(std::make_unique<BinomialPricer>());

        // Create analyzer and trading decision maker
        StatisticalAnalyzer analyzer;
        TradingDecision trader;

        // Print parameters
        std::cout << "\n=== Parameters ===\n";
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "Spot Price: " << spot << "\n"
                  << "Strike Price: " << strike << "\n"
                  << "Risk-free Rate: " << (rate * 100) << "%\n"
                  << "Volatility: " << (vol * 100) << "%\n"
                  << "Time to Expiry: " << expiry << " years\n"
                  << "Option Type: " << (type == Option::Type::CALL ? "Call" : "Put") << "\n"
                  << "Option Style: " << (style == Option::Style::EUROPEAN ? "European" : "American") << "\n";

        // Calculate and display prices
        std::cout << "\n=== Pricing Results ===\n";
        for (const auto& strategy : strategies) {
            try {
                double price = strategy->calculatePrice(option, marketData);
                std::cout << strategy->getStrategyName() << " Price: " << price << "\n";
            } catch (const std::exception& e) {
                std::cerr << "Error in " << strategy->getStrategyName() << ": " << e.what() << "\n";
            }
        }

        // Get trading decision
        try {
            // Redo for analysis
            std::vector<double> prices1, prices2;
            const int num_samples = 30;
            prices1.reserve(num_samples);
            prices2.reserve(num_samples);
            
            // Generate prices using same logic as in TradingDecision::makeDecision
            for (int i = 0; i < num_samples; i++) {
                double vol_adjustment = 0.95 + (i * 0.01);
                MarketData adjusted_data(
                    marketData.getSpot(),
                    marketData.getRiskFreeRate(),
                    marketData.getVolatility() * vol_adjustment
                );
                
                prices1.push_back(strategies[0]->calculatePrice(option, adjusted_data));
                prices2.push_back(strategies[1]->calculatePrice(option, adjusted_data));
            }

            // Get the analysis result
            auto analysis = analyzer.analyzePricingDifference(prices1, prices2);
            
            // Get the trading decision
            auto action = trader.makeDecision(option, strategies, marketData, analyzer);
            
            // Now we have both action and analysis defined
            printDecision(action, option, marketData, analysis);
        } catch (const std::exception& e) {
            std::cerr << "Error in trading decision: " << e.what() << "\n";
        }

        // Ask if user wants to try another calculation
        char again;
        std::cout << "\nCalculate another option? (y/n): ";
        std::cin >> again;
        if (again == 'y' || again == 'Y') {
            clearInputBuffer();
            return main(); // Restart the program
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
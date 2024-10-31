#include "options_classes.hpp"

int main() {
    
    // Initializing data structure 
    optionspricing::OptionData data{
        .strike_price = 100.0,
        .spot_price = 95.0,
        .risk_free_rate = 0.05,
        .volatility = 0.2,
        .time_to_maturity = 1.0,
        .type = optionspricing::OptionType::CALL,
        .style = optionspricing::OptionStyle::EUROPEAN
    };

    // Initialize the options pricer
    optionspricing::OptionsPricer pricer;
    
    // determine signal for option with given data
    auto signal = pricer.analyzeOption(data);
    
    // Process the trading signal
}
# Options Pricing Analysis Tool

## Overview
A high-performance C++ implementation of financial options pricing and analysis tools. This command-line application implements both Black-Scholes and Binomial pricing models, enhanced with statistical analysis and trading decision support. The system provides real-time pricing, volatility analysis, and data-driven trading recommendations.

## Features
- **Multiple Pricing Models**
  - Black-Scholes model for European options
  - Binomial model supporting both European and American options
  - Comparative analysis between different pricing methods

- **Advanced Analytics**
  - Statistical significance testing using Welch's t-test
  - Volatility sensitivity analysis across 30 sample points
  - Confidence interval calculations
  - Risk-adjusted trading signals

- **Risk Management**
  - Dynamic volatility-adjusted thresholds
  - Comprehensive Greeks calculations
  - Statistical arbitrage detection

## Requirements
- C++17 or higher
- CMake 3.10+ (for building)
- Standard Math Library
- Memory Management Library

## Building the Project
```bash
mkdir build
cd build
cmake ..
make
```

## Usage
The application runs interactively, prompting for the following inputs:

```bash
=== Options Pricing Calculator ===

Enter Market Data:
Spot Price: 100
Risk-free Rate (as decimal, e.g., 0.05 for 5%): 0.05
Volatility (as decimal, e.g., 0.2 for 20%): 0.2

Select option type:
1. Call
2. Put
Choice: 1

Select option style:
1. European
2. American
Choice: 1

Strike Price: 100
Time to Expiry (in years): 1
```

### Example Output
```bash
=== Parameters ===
Spot Price: 100.0000
Strike Price: 100.0000
Risk-free Rate: 5.0000%
Volatility: 20.0000%
Time to Expiry: 1.0000 years
Option Type: Call
Option Style: European

=== Pricing Results ===
Black-Scholes Price: 10.4506
Binomial Price: 10.4491

Trading Decision Analysis:
Statistical Confidence:
- Mean Price Difference: 0.0015
- Standard Deviation: 0.0003
- P-Value: 0.4892
- Confidence Interval: ±0.0012

RECOMMENDATION: HOLD
Statistical analysis indicates:
- Price difference not statistically significant (p-value: 0.4892)
- Edge of 0.0015% below volatility-adjusted threshold
- Price within expected theoretical range across volatility scenarios 19% to 24.8%
```

## Technical Implementation
- **Object-Oriented Design**: Utilizes inheritance and polymorphism for pricing strategies
- **Memory Management**: Smart pointers for safe resource handling
- **Error Handling**: Comprehensive exception handling with detailed error messages
- **Performance Optimization**: 
  - Pre-computed constants
  - Efficient data structures (single vector for binomial tree)
  - Adaptive time-step sizing

## Key Classes
- `Option`: Represents option contracts with type and style
- `MarketData`: Encapsulates market conditions
- `BlackScholesPricer`: Implements Black-Scholes model
- `BinomialPricer`: Implements binomial model
- `StatisticalAnalyzer`: Performs statistical analysis
- `TradingDecision`: Generates trading signals

## Error Handling
The application includes robust error checking for:
- Invalid numerical inputs
- Out-of-range parameters
- Model-specific constraints
- Memory allocation failures

## Contributing
Contributions are welcome! Please feel free to submit a Pull Request.

## License
This project is licensed under the MIT License - see the LICENSE file for details.

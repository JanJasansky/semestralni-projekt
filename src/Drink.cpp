#include "Drink.hpp"
#include <algorithm>

// Hustota ethanolu ~ 0.789 g/ml
static constexpr double ETHANOL_DENSITY_G_PER_ML = 0.789;

double Drink::pureAlcoholGrams() const
{
    // ABV% -> podíl (např. 5% = 0.05)
    const double fraction = std::max(0.0, abvPercent) / 100.0;
    const double ethanolMl = std::max(0.0, volumeMl) * fraction;
    const double grams = ethanolMl * ETHANOL_DENSITY_G_PER_ML;
    return grams;
}

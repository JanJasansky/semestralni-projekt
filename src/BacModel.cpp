#include "BacModel.h"
#include <cmath>

// Convert alcohol volume and ABV to grams of pure alcohol
// volume v ml, abv v procentech
// A_g = V_ml * (abv/100) * hustota

double BacModel::alcoholGrams(double volume, double abv) {
    return volume * (abv / 100.0) * ETHANOL_DENSITY;
}

double BacModel::calculateBAC(double weight_kg, Gender gender, double totalAlcoholGrams, double hoursElapsed) {
    const double r = rFactor(gender);
    double bac = (totalAlcoholGrams / (r * std::max(1.0, weight_kg))) - (BETA_PER_MILLE_PER_HOUR * std::max(0.0, hoursElapsed));
    return (bac < 0.0) ? 0.0 : bac;
}

BacResult BacModel::compute(
    Gender gender,
    double weight_kg,
    int drinks_count,
    const DrinkProfile& profile,
    int hours_since_first,
    int minutes_since_first,
    double legal_limit_per_mille
) {
    BacResult res{};

    weight_kg = std::max(1.0, weight_kg);
    drinks_count = std::max(0, drinks_count);

    const double t = std::max(0.0, hours_since_first + minutes_since_first / 60.0);
    const double grams_total = BacModel::alcoholGrams(profile.volume, profile.abv) * drinks_count;
    const double bac = calculateBAC(weight_kg, gender, grams_total, t);

    res.bac_per_mille = bac;
    res.allowed_to_drive = (bac <= legal_limit_per_mille);

    // čas do 0: bac - beta*x = 0 => x = bac/beta
    res.hours_to_sober = (bac <= 0.0) ? 0.0 : (bac / BETA_PER_MILLE_PER_HOUR);
    return res;
}

BacResult BacModel::computeFromGrams(
    Gender gender,
    double weight_kg,
    double totalAlcoholGrams,
    double hours_since_first,
    double legal_limit_per_mille
) {
    BacResult res{};
    weight_kg = std::max(1.0, weight_kg);

    const double t = std::max(0.0, hours_since_first);
    const double bac = calculateBAC(weight_kg, gender, std::max(0.0, totalAlcoholGrams), t);

    res.bac_per_mille = bac;
    res.allowed_to_drive = (bac <= legal_limit_per_mille);
    res.hours_to_sober = (bac <= 0.0) ? 0.0 : (bac / BETA_PER_MILLE_PER_HOUR);
    return res;
}

double BacModel::bacAtTimeHours(
    Gender gender,
    double weight_kg,
    int drinks_count,
    const DrinkProfile& profile,
    double time_hours_since_first
) {
    const double grams_total = BacModel::alcoholGrams(profile.volume, profile.abv) * std::max(0, drinks_count);
    return bacAtTimeHoursFromGrams(gender, weight_kg, grams_total, time_hours_since_first);
}

double BacModel::bacAtTimeHoursFromGrams(
    Gender gender,
    double weight_kg,
    double totalAlcoholGrams,
    double time_hours_since_first
) {
    return calculateBAC(weight_kg, gender, std::max(0.0, totalAlcoholGrams), std::max(0.0, time_hours_since_first));
}

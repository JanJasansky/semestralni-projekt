#pragma once
#include <string>
#include <algorithm>

enum class Gender { Male, Female };

struct DrinkProfile {
    double volume; // in milliliters
    double abv;    // alcohol by volume percentage
};

struct BacResult {
    double bac_per_mille;      // ‰
    double hours_to_sober;     // kolik hodin do 0.0
    bool allowed_to_drive;
};

class BacModel {
public:
    // Konstanta: hustota ethanolu (g/ml)
    static constexpr double ETHANOL_DENSITY = 0.789;

    // Metabolismus v ‰/h (běžný odhad)
    static constexpr double BETA_PER_MILLE_PER_HOUR = 0.15;

    // Distribuční faktor r (Widmark)
    static double rFactor(Gender g) { return (g == Gender::Male) ? 0.68 : 0.55; }

    static double lbToKg(double lb) { return lb * 0.45359237; }

    // g alkoholu z objemu a ABV
    static double alcoholGrams(double volume, double abv);

    // Widmark v ‰:
    // BAC‰ = (A_g / (r * weight_kg)) - beta * t_h
    static double calculateBAC(double weight_kg, Gender gender, double totalAlcoholGrams, double hoursElapsed);

    static BacResult compute(
        Gender gender,
        double weight_kg,
        int drinks_count,
        const DrinkProfile& profile,
        int hours_since_first,
        int minutes_since_first,
        double legal_limit_per_mille = 0.0
    );

    static BacResult computeFromGrams(
        Gender gender,
        double weight_kg,
        double totalAlcoholGrams,
        double hours_since_first,
        double legal_limit_per_mille = 0.0
    );

    // pomocná funkce – vygeneruje BAC v čase (pro graf)
    static double bacAtTimeHours(
        Gender gender,
        double weight_kg,
        int drinks_count,
        const DrinkProfile& profile,
        double time_hours_since_first
    );

    static double bacAtTimeHoursFromGrams(
        Gender gender,
        double weight_kg,
        double totalAlcoholGrams,
        double time_hours_since_first
    );
};

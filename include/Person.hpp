#pragma once
#include <string>

enum class Sex
{
    Male,
    Female
};

struct Person
{
    Sex sex = Sex::Male;
    double weightKg = 75.0;
    double heightCm = 175.0;   // zatím spíš informační (do budoucna)
    double eliminationRatePermilPerHour = 0.15; // ‰/h (nastavitelné)
};

#pragma once
#include <string>

// Jeden „pitný interval“: co, kolik, jak silné, od kdy do kdy
struct Drink
{
    std::string name;   // "Pivo 12°", "Víno", ...
    double volumeMl = 0.0;
    double abvPercent = 0.0;   // % alkoholu (např. 5.0)
    int startMinute = 0;       // minuty od začátku dne / nebo od startu simulace
    int endMinute = 0;         // end >= start

    // Pomocná: čistý alkohol v gramech (bez zohlednění vstřebávání)
    double pureAlcoholGrams() const;
};

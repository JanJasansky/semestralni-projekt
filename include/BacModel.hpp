#pragma once
#include <vector>
#include <string>
#include "Person.hpp"
#include "Drink.hpp"

// Výsledek simulace: křivka BAC po minutách
struct BacResult
{
    int simStartMinute = 0;
    int simEndMinute = 0;
    std::vector<double> bacPermilByMinute; // index 0 = simStartMinute

    double peakBacPermil = 0.0;
    int peakMinute = 0;

    int soberMinute_0_0 = -1; // kdy klesne na 0.0 ‰
    int soberMinute_0_2 = -1; // kdy klesne na 0.2 ‰
    int soberMinute_0_5 = -1; // kdy klesne na 0.5 ‰
};

class BacModel
{
public:
    // Hlavní metoda: vypočítá BAC křivku a důležité body
    static BacResult simulate(const Person& person, const std::vector<Drink>& drinks);

    // Export do CSV (minute, hhmm, bac)
    static bool exportCsv(const std::string& path, const BacResult& result);

private:
    // Widmarkův distribuční faktor (hrubé hodnoty)
    static double widmarkR(const Person& person);

    // Model vstřebávání: kolik gramů ethanolu přiteče do krve v dané minutě
    // Jednoduché a pro začátečníka pochopitelné:
    // - během pití "od-do" se alkohol rozprostře rovnoměrně
    // - a navíc se přidá krátké zpoždění / dojezd
    static std::vector<double> buildIntakeGramsPerMinute(const std::vector<Drink>& drinks,
                                                         int simStart, int simEnd);
};

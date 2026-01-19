#include "BacModel.hpp"
#include "TimeUtils.hpp"
#include <algorithm>
#include <fstream>
#include <cmath>

static double clamp0(double x) { return (x < 0.0) ? 0.0 : x; }

double BacModel::widmarkR(const Person& person)
{
    // Hrubý odhad:
    // Muž ~ 0.68, žena ~ 0.55
    // (reálně je to individuální, ale pro projekt OK)
    return (person.sex == Sex::Male) ? 0.68 : 0.55;
}

std::vector<double> BacModel::buildIntakeGramsPerMinute(const std::vector<Drink>& drinks,
                                                        int simStart, int simEnd)
{
    const int len = std::max(0, simEnd - simStart + 1);
    std::vector<double> intake(len, 0.0);

    // Jednoduchý „dojezd“ po dopití (např. ještě 30 minut)
    const int tailMinutes = 30;

    for (const auto& d : drinks)
    {
        const double totalG = d.pureAlcoholGrams();

        const int s = std::max(d.startMinute, simStart);
        const int e = std::min(d.endMinute, simEnd);

        if (e < s) continue;

        const int duration = std::max(1, e - s + 1);
        const double perMinute = totalG / (duration + tailMinutes);

        // během pití
        for (int t = s; t <= e; ++t)
        {
            intake[t - simStart] += perMinute;
        }
        // dojezd po dopití
        for (int t = e + 1; t <= std::min(simEnd, e + tailMinutes); ++t)
        {
            intake[t - simStart] += perMinute;
        }
    }

    return intake;
}

BacResult BacModel::simulate(const Person& person, const std::vector<Drink>& drinks)
{
    BacResult out;

    if (drinks.empty())
    {
        out.simStartMinute = 0;
        out.simEndMinute = 0;
        out.bacPermilByMinute = {0.0};
        out.peakBacPermil = 0.0;
        out.peakMinute = 0;
        out.soberMinute_0_0 = 0;
        out.soberMinute_0_2 = 0;
        out.soberMinute_0_5 = 0;
        return out;
    }

    // Simulace od nejdřívějšího startu do (nejpozdějšího endu + rezerva)
    int minStart = drinks.front().startMinute;
    int maxEnd = drinks.front().endMinute;

    for (const auto& d : drinks)
    {
        minStart = std::min(minStart, d.startMinute);
        maxEnd = std::max(maxEnd, d.endMinute);
    }

    const int extraMinutes = 12 * 60; // rezerva 12 hodin na vystřízlivění
    out.simStartMinute = minStart;
    out.simEndMinute = maxEnd + extraMinutes;

    const int len = out.simEndMinute - out.simStartMinute + 1;

    // Příjem alkoholu (g/min)
    const auto intake = buildIntakeGramsPerMinute(drinks, out.simStartMinute, out.simEndMinute);

    out.bacPermilByMinute.assign(len, 0.0);

    // Zjednodušený Widmark:
    // BAC(‰) ~ (A_g / (weight_kg * r)) - beta * hours
    // Zde to počítáme dynamicky po minutách:
    // - do "těla" přitéká intake[t]
    // - současně se odbourává konstantní rychlostí (‰/h)
    const double r = widmarkR(person);
    const double weight = std::max(1.0, person.weightKg);
    const double elimPermilPerMinute = clamp0(person.eliminationRatePermilPerHour) / 60.0;

    // Převod: kolik promile udělá 1 gram alkoholu v těle:
    // (A_g / (weight_kg * r)) dává zhruba promile (zjednodušeně)
    const double permilPerGram = 1.0 / (weight * r);

    double bac = 0.0;

    out.peakBacPermil = 0.0;
    out.peakMinute = out.simStartMinute;

    for (int i = 0; i < len; ++i)
    {
        bac += intake[i] * permilPerGram;
        bac -= elimPermilPerMinute;
        bac = clamp0(bac);

        out.bacPermilByMinute[i] = bac;

        const int minute = out.simStartMinute + i;
        if (bac > out.peakBacPermil)
        {
            out.peakBacPermil = bac;
            out.peakMinute = minute;
        }
    }

    // Najdi, kdy klesne pod hranice (poprvé po peaku, ať to dává smysl)
    auto findBelow = [&](double thr) -> int
    {
        const int peakIndex = out.peakMinute - out.simStartMinute;
        for (int i = std::max(0, peakIndex); i < len; ++i)
        {
            if (out.bacPermilByMinute[i] <= thr)
                return out.simStartMinute + i;
        }
        return -1;
    };

    out.soberMinute_0_5 = findBelow(0.5);
    out.soberMinute_0_2 = findBelow(0.2);
    out.soberMinute_0_0 = findBelow(0.0);

    return out;
}

bool BacModel::exportCsv(const std::string& path, const BacResult& result)
{
    std::ofstream f(path);
    if (!f.is_open()) return false;

    f << "minute,hhmm,bac_permil\n";
    for (int i = 0; i < (int)result.bacPermilByMinute.size(); ++i)
    {
        const int minute = result.simStartMinute + i;
        f << minute << ","
          << TimeUtils::minutesToHHMM(minute) << ","
          << result.bacPermilByMinute[i] << "\n";
    }
    return true;
}

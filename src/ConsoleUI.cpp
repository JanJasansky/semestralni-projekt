#include "ConsoleUI.hpp"
#include "TimeUtils.hpp"
#include <iostream>
#include <limits>
#include <iomanip>
#include <fstream>
#include <thread>
#include <chrono>
#include <termios.h>
#include <unistd.h>

static void clearCin()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static double inputValidatedDouble(const std::string& prompt, double minValue, double maxValue)
{
    double value;
    while (true)
    {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail() || value < minValue || value > maxValue)
        {
            std::cout << "Neplatny vstup. Zadejte cislo v rozsahu " << minValue << " - " << maxValue << ".\n";
            clearCin();
        }
        else
        {
            clearCin();
            return value;
        }
    }
}

static int inputValidatedInt(const std::string& prompt, int minValue, int maxValue)
{
    int value;
    while (true)
    {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail() || value < minValue || value > maxValue)
        {
            std::cout << "Neplatny vstup. Zadejte cele cislo v rozsahu " << minValue << " - " << maxValue << ".\n";
            clearCin();
        }
        else
        {
            clearCin();
            return value;
        }
    }
}

static void setTerminalMode(bool enable)
{
    static struct termios oldt, newt;
    if (enable)
    {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    }
    else
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
}

static int inputImmediateInt(const std::string& prompt, int minValue, int maxValue)
{
    std::cout << prompt;
    setTerminalMode(true);
    int value = 0;
    char c;
    while (true)
    {
        c = getchar();
        if (c >= '0' && c <= '9')
        {
            value = value * 10 + (c - '0');
            std::cout << c << std::flush;
        }
        else if (c == '\n' || c == '\r')
        {
            if (value >= minValue && value <= maxValue)
            {
                std::cout << "\n";
                break;
            }
            else
            {
                std::cout << "\nNeplatny vstup. Zadejte cislo v rozsahu " << minValue << " - " << maxValue << ".\n";
                value = 0;
                std::cout << prompt;
            }
        }
        else if (c == '\b' || c == 127) // Backspace
        {
            if (value > 0)
            {
                value /= 10;
                std::cout << "\b \b" << std::flush;
            }
        }
    }
    setTerminalMode(false);
    return value;
}

static std::string inputImmediateString(const std::string& prompt)
{
    std::cout << prompt;
    setTerminalMode(true);
    std::string value;
    char c;
    while (true)
    {
        c = getchar();
        if (c == '\n' || c == '\r')
        {
            std::cout << "\n";
            break;
        }
        else if (c == '\b' || c == 127) // Backspace
        {
            if (!value.empty())
            {
                value.pop_back();
                std::cout << "\b \b" << std::flush;
            }
        }
        else
        {
            value += c;
            std::cout << c << std::flush;
        }
    }
    setTerminalMode(false);
    return value;
}

static std::string inputImmediateTime(const std::string& prompt)
{
    std::cout << prompt;
    setTerminalMode(true);
    std::string value;
    char c;
    while (true)
    {
        c = getchar();
        if (c == '\n' || c == '\r')
        {
            std::cout << "\n";
            break;
        }
        else if (c == '\b' || c == 127) // Backspace
        {
            if (!value.empty())
            {
                value.pop_back();
                std::cout << "\b \b" << std::flush;
            }
        }
        else
        {
            value += c;
            std::cout << c << std::flush;
        }
    }
    setTerminalMode(false);
    return value;
}

static void printWithDelay(const std::string& text, int delayMs = 50)
{
    for (char c : text)
    {
        if (std::cin.peek() == ' ') // Kontrola, zda uživatel stiskl mezerník
        {
            std::cin.ignore(); // Ignorování mezerníku
            std::cout << text.substr(&c - &text[0]) << std::flush; // Okamžité vypsání zbytku textu
            return;
        }
        std::cout << c << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    }
    std::cout << "\n";
}

Person ConsoleUI::inputPerson()
{
    Person p;

    printWithDelay("========================================");
    printWithDelay("=== Vitejte v Alkoholicke kalkulacce ===");
    printWithDelay("========================================\n");
    printWithDelay("Tento program vam pomuze odhadnout hladinu alkoholu v krvi.");
    printWithDelay("Zadejte sve udaje a informace o tom, co jste pili.\n");

    while (true)
    {
        std::cout << "Zadej pohlavi (m/z): ";
        char s;
        std::cin >> s;
        clearCin();

        if (s == 'z' || s == 'Z')
        {
            p.sex = Sex::Female;
            p.eliminationRatePermilPerHour = 0.1; // Zeny maji obvykle nizsi odbouravani
            break;
        }
        else if (s == 'm' || s == 'M')
        {
            p.sex = Sex::Male;
            p.eliminationRatePermilPerHour = 0.15; // Muzi maji obvykle vyssi odbouravani
            break;
        }
        else
        {
            std::cout << "Neplatny vstup. Zadejte 'm' pro muze nebo 'z' pro zenu.\n";
        }
    }

    p.weightKg = inputValidatedDouble("Zadej vahu (kg): ", 30.0, 300.0);
    p.heightCm = inputValidatedDouble("Zadej vysku (cm): ", 100.0, 250.0);

    return p;
}

std::vector<Drink> ConsoleUI::inputDrinks()
{
    std::vector<Drink> drinks;

    int n = inputImmediateInt("Kolik ruznych napoju jsi mel? (napr. 3): ", 1, 100);

    for (int i = 0; i < n; ++i)
    {
        Drink d;
        while (true)
        {
            std::cout << "\n--- Napoj #" << (i + 1) << " ---\n";
            std::cout << "Vyber napoj:\n";
            std::cout << "1. Pivo 10° (500 ml, 4.0%)\n";
            std::cout << "2. Pivo 11° (500 ml, 4.5%)\n";
            std::cout << "3. Pivo 12° (500 ml, 5.0%)\n";
            std::cout << "4. Bile vino (200 ml, 11.0%)\n";
            std::cout << "5. Cervene vino (200 ml, 12.5%)\n";
            std::cout << "6. Rum (50 ml, 40.0%)\n";
            std::cout << "7. Vlastni napoj\n";
            int choice = inputImmediateInt("Zadej cislo: ", 1, 7);

            switch (choice)
            {
            case 1:
                d.name = "Pivo 10°";
                d.volumeMl = 500;
                d.abvPercent = 4.0;
                break;
            case 2:
                d.name = "Pivo 11°";
                d.volumeMl = 500;
                d.abvPercent = 4.5;
                break;
            case 3:
                d.name = "Pivo 12°";
                d.volumeMl = 500;
                d.abvPercent = 5.0;
                break;
            case 4:
                d.name = "Bile vino";
                d.volumeMl = 200;
                d.abvPercent = 11.0;
                break;
            case 5:
                d.name = "Cervene vino";
                d.volumeMl = 200;
                d.abvPercent = 12.5;
                break;
            case 6:
                d.name = "Rum";
                d.volumeMl = 50;
                d.abvPercent = 40.0;
                break;
            case 7:
                std::cout << "Nazev (napr. Pivo, Vino, Rum...): ";
                std::getline(std::cin, d.name);
                if (d.name.empty()) d.name = "Napoj";

                d.volumeMl = inputValidatedDouble("Objem v ml (napr. 500): ", 10.0, 2000.0);
                d.abvPercent = inputValidatedDouble("Alkohol % (ABV) (napr. 5): ", 0.1, 100.0);
                break;
            }

            drinks.push_back(d);
            break;
        }
    }

    return drinks;
}

void ConsoleUI::printSummary(const Person& p, const std::vector<Drink>& drinks, const BacResult& r)
{
    printWithDelay("\n================ VYSLEDKY ================\n");
    printWithDelay("Osoba: " + std::string((p.sex == Sex::Male) ? "muz" : "zena") +
                   ", " + std::to_string(p.weightKg) + " kg" +
                   ", " + std::to_string(p.heightCm) + " cm" +
                   ", odbouravani " + std::to_string(p.eliminationRatePermilPerHour) + " ‰/h\n\n");

    printWithDelay("Zadane piti:\n");
    for (const auto& d : drinks)
    {
        printWithDelay(" - " + d.name +
                       " | " + std::to_string(static_cast<int>(d.volumeMl * 10) / 10.0) + " ml" +
                       " | " + std::to_string(static_cast<int>(d.abvPercent * 10) / 10.0) + " %" +
                       " | " + TimeUtils::minutesToHHMM(d.startMinute) +
                       " - " + TimeUtils::minutesToHHMM(d.endMinute) +
                       " | alkohol ~ " + std::to_string(static_cast<int>(d.pureAlcoholGrams() * 10) / 10.0) + " g\n");
    }

    printWithDelay("\nPeak: " + std::to_string(static_cast<int>(r.peakBacPermil * 100) / 100.0) + " ‰ v " + TimeUtils::minutesToHHMM(r.peakMinute) + "\n");

    auto showTimeOrNA = [&](const char* label, int minute)
    {
        if (minute < 0)
            printWithDelay(std::string(label) + ": nenalezeno v rozsahu simulace\n");
        else
            printWithDelay(std::string(label) + ": " + TimeUtils::minutesToHHMM(minute) + "\n");
    };

    showTimeOrNA("Pod 0.5 ‰", r.soberMinute_0_5);
    showTimeOrNA("Pod 0.2 ‰", r.soberMinute_0_2);
    showTimeOrNA("0.0 ‰ (odhad \"strizlivy\")", r.soberMinute_0_0);

    printWithDelay("==========================================\n\n");
}

void ConsoleUI::printAsciiGraph(const BacResult& r, int width, int stepMinutes)
{
    const double maxBac = std::max(0.01, r.peakBacPermil);
    int zeroCount = 0; // Counter for consecutive zero BAC rows

    for (int minute = r.simStartMinute; minute <= r.simEndMinute; minute += stepMinutes)
    {
        const int idx = minute - r.simStartMinute;
        if (idx < 0 || idx >= (int)r.bacPermilByMinute.size()) continue;

        const double bac = r.bacPermilByMinute[idx];
        const int bars = (int)std::round((bac / maxBac) * width);

        std::cout << TimeUtils::minutesToHHMM(minute) << " | ";
        for (int i = 0; i < bars; ++i) std::cout << '#';
        std::cout << " " << std::fixed << std::setprecision(1) << bac << " ‰\n";

        if (bac == 0.0)
        {
            zeroCount++;
            if (zeroCount >= 3) break; // Stop after 3 consecutive zero BAC rows
        }
        else
        {
            zeroCount = 0; // Reset counter if BAC is not zero
        }
    }

    std::cout << "\n";
}

void ConsoleUI::saveResultsToFile(const BacResult& r, const std::string& filePath)
{
    std::ofstream file(filePath);
    if (!file)
    {
        std::cout << "Chyba: Nelze otevřít soubor pro zápis: " << filePath << "\n";
        return;
    }

    file << "Minuta,BAC (\u2030)\n";
    for (size_t i = 0; i < r.bacPermilByMinute.size(); ++i)
    {
        file << (r.simStartMinute + i) << "," << r.bacPermilByMinute[i] << "\n";
    }

    std::cout << "Výsledky byly úspěšně uloženy do souboru: " << filePath << "\n";
}

void ConsoleUI::displayDrinkDetails(const std::vector<Drink>& drinks)
{
    std::cout << "\nPodrobnosti o zadaných nápojích:\n";
    for (const auto& drink : drinks)
    {
        std::cout << " - Název: " << drink.name
                  << ", Objem: " << drink.volumeMl << " ml"
                  << ", Alkohol: " << drink.abvPercent << " %"
                  << ", Čas: " << TimeUtils::minutesToHHMM(drink.startMinute)
                  << " - " << TimeUtils::minutesToHHMM(drink.endMinute) << "\n";
    }
}

void ConsoleUI::showHelpMenu()
{
    std::cout << "\n=== Nápověda ===\n";
    std::cout << "1. Zadejte své údaje (pohlaví, váha, výška, rychlost odbourávání).\n";
    std::cout << "2. Zadejte informace o nápojích (název, objem, obsah alkoholu, čas).\n";
    std::cout << "3. Program vypočítá hladinu alkoholu a zobrazí výsledky.\n";
    std::cout << "4. Výsledky můžete uložit do souboru nebo zobrazit graf.\n";
    std::cout << "=================\n\n";
}

void ConsoleUI::run()
{
    Person p = inputPerson();
    auto drinks = inputDrinks();

    if (drinks.empty())
    {
        std::cout << "Nebyl zadan zadny napoj. Koncim.\n";
        return;
    }

    std::string startTime = inputImmediateTime("Zadej cas, od kdy jsi pil (HH:MM, napr. 20:00): ");
    int startMinute = TimeUtils::parseHHMMToMinutes(startTime);

    while (startMinute < 0)
    {
        startTime = inputImmediateTime("Neplatny cas. Zadej cas, od kdy jsi pil (HH:MM, napr. 20:00): ");
        startMinute = TimeUtils::parseHHMMToMinutes(startTime);
    }

    std::string endTime = inputImmediateTime("Zadej cas, do kdy jsi pil (HH:MM, napr. 22:00): ");
    int endMinute = TimeUtils::parseHHMMToMinutes(endTime);

    while (endMinute < 0 || endMinute <= startMinute)
    {
        endTime = inputImmediateTime("Neplatny cas. Zadej cas, do kdy jsi pil (HH:MM, napr. 22:00): ");
        endMinute = TimeUtils::parseHHMMToMinutes(endTime);
    }

    int totalMinutes = endMinute - startMinute;
    int intervalMinutes = totalMinutes / std::max(1, (int)drinks.size());

    for (size_t i = 0; i < drinks.size(); ++i)
    {
        drinks[i].startMinute = startMinute + i * intervalMinutes;
        drinks[i].endMinute = drinks[i].startMinute + intervalMinutes;
    }

    const auto result = BacModel::simulate(p, drinks);
    printSummary(p, drinks, result);
    printAsciiGraph(result);

    std::cout << "\nPamatuj, ze tyto vypocty jsou pouze orientacni. Bud opatrny a nerid pod vlivem alkoholu.\n";
}

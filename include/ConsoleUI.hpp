#pragma once
#include <vector>
#include "Person.hpp"
#include "Drink.hpp"
#include "BacModel.hpp"

class ConsoleUI
{
public:
    void run();

private:
    Person inputPerson();
    std::vector<Drink> inputDrinks();

    void printSummary(const Person& p, const std::vector<Drink>& drinks, const BacResult& r);
    void printAsciiGraph(const BacResult& r, int width = 60, int stepMinutes = 10);
    void saveResultsToFile(const BacResult& r, const std::string& filePath);
    void displayDrinkDetails(const std::vector<Drink>& drinks);
    void showHelpMenu();
};

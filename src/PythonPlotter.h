#pragma once
#include <string>

class PythonPlotter {
public:
    PythonPlotter() = default;
    ~PythonPlotter() = default;

    std::string generatePlotPng(const std::string& dataFile, const std::string& outputImage);
};

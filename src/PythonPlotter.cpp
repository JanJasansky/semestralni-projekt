#include "PythonPlotter.h"
#include <cstdlib>

std::string PythonPlotter::generatePlotPng(const std::string& dataFile, const std::string& outputImage) {
#ifdef _WIN32
    std::string cmd = "python python\\plot_bac.py \"" + dataFile + "\" \"" + outputImage + "\"";
#else
    std::string cmd = "python3 python/plot_bac.py \"" + dataFile + "\" \"" + outputImage + "\"";
#endif
    std::system(cmd.c_str());
    return outputImage;
}

#pragma once
#include <string>

// Práce s časem (HH:MM) -> minuty, a zpět
namespace TimeUtils
{
    // "20:35" -> 1235
    // vrací -1 při chybě
    int parseHHMMToMinutes(const std::string& hhmm);

    // 1235 -> "20:35"
    std::string minutesToHHMM(int minutes);
}

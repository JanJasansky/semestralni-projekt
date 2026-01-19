#include "TimeUtils.hpp"
#include <sstream>
#include <iomanip>

namespace TimeUtils
{
    int parseHHMMToMinutes(const std::string& hhmm)
    {
        int h = -1, m = -1;
        char colon = '\0';

        std::istringstream iss(hhmm);
        iss >> h >> colon >> m;

        if (!iss || colon != ':') return -1;
        if (h < 0 || h > 23) return -1;
        if (m < 0 || m > 59) return -1;

        return h * 60 + m;
    }

    std::string minutesToHHMM(int minutes)
    {
        if (minutes < 0) minutes = 0;
        minutes %= (24 * 60);

        const int h = minutes / 60;
        const int m = minutes % 60;

        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << h
            << ":"
            << std::setw(2) << std::setfill('0') << m;
        return oss.str();
    }
}

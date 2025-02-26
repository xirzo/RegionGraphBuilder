#ifndef COUNTRY_H
#define COUNTRY_H

#include <string>
#include <vector>

struct country {
    std::string name;
    std::string iso_code;
    std::string capital;
    std::vector<std::string> neighboring_countries_iso;
};

#endif  // !COUNTRY_H

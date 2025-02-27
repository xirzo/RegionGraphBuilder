#ifndef COUNTRY_H
#define COUNTRY_H

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

struct capital_coordinates {
    double latitude;
    double longitude;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(capital_coordinates, latitude, longitude)
};

struct country {
    std::string name;
    std::string iso_code;
    std::string capital;
    struct capital_coordinates capital_coords;
    std::vector<std::string> neighboring_countries_iso;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(country, name, iso_code, capital, capital_coords,
                                   neighboring_countries_iso)
};

#endif  // !COUNTRY_H

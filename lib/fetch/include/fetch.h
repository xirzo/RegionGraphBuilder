#ifndef FETCH_H
#define FETCH_H

#include <string>

namespace fetch {
void fetch_neighboring_countries(const std::string& api_key,
                                 const std::string& country_iso3166_2_code,
                                 const std::string& format = "json");
}

#endif  // !FETCH_H

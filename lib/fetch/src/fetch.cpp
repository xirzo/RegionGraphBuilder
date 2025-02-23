#include "fetch.h"

#include <cpr/cpr.h>

#include <iostream>

using namespace cpr;

namespace fetch {

void fetch_neighboring_countries(const std::string& api_key,
                                 const std::string& country_iso3166_2_code,
                                 const std::string& format) {
    Response response = Get(Url{"https://api.geodatasource.com/v2/neighboring-countries"},
                            Parameters{{"key", api_key},
                                       {"country_code", country_iso3166_2_code},
                                       {"format", format}});

    std::cout << response.status_code << std::endl;

    std::cout << response.text << std::endl;
}

}  // namespace fetch

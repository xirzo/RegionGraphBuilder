#include "fetch.h"

#include <cpr/cpr.h>

#include <expected>
#include <string>

using namespace cpr;

namespace fetch {

std::expected<std::string, std::string> fetch_country(const std::string& iso_code) {
    Response response = Get(Url{"https://restcountries.com/v3.1/alpha/" + iso_code});

    if (response.status_code != 200) {
        return std::unexpected("Error: fetch country failed with code " +
                               std::to_string(response.status_code));
    }

    return response.text;
}

std::expected<std::string, std::string> fetch_region_codes(const std::string& region) {
    Response response = Get(Url{"https://restcountries.com/v3.1/region/" + region});

    if (response.status_code != 200) {
        return std::unexpected("Error: fetch neighboring countries failed with code " +
                               std::to_string(response.status_code));
    }

    return response.text;
}

std::expected<std::string, std::string> fetch_neighboring_countries(
    const std::string& api_key, const std::string& country_iso3166_2_code,
    const std::string& format) {
    Response response = Get(Url{"https://api.geodatasource.com/v2/neighboring-countries"},
                            Parameters{{"key", api_key},
                                       {"country_code", country_iso3166_2_code},
                                       {"format", format}});

    if (response.status_code != 200) {
        return std::unexpected("Error: fetch neighboring countries failed with code " +
                               std::to_string(response.status_code));
    }

    return response.text;
}

}  // namespace fetch

#include "json_file.h"

#include <fstream>

#include "nlohmann/json_fwd.hpp"

namespace json_file {

std::expected<void, write_error> write(const std::string& filename, nlohmann::json j) {
    std::ofstream output(filename);

    if (!output.is_open()) {
        return std::unexpected(write_error::cannot_open_file);
    }

    output << j.dump(4);

    return {};
}

std::expected<nlohmann::json, read_error> read(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        return std::unexpected(read_error::cannot_open_file);
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    nlohmann::json j;

    try {
        j = nlohmann::json::parse(content);
    } catch (const nlohmann::json::parse_error& e) {
        return read_error::failed_parsing;
    }

    return j;
}

}  // namespace json_file

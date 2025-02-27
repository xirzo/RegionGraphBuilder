#ifndef JSON_FILE_H
#define JSON_FILE_H

#include <expected>
#include <string>
#include <nlohmann/json.hpp>
#include "nlohmann/json_fwd.hpp"

namespace json_file {

struct error_info {
    enum class code {
        cannot_open_file,
        failed_parsing
    };

    code error_code;
    std::string message;     
    std::string operation;    
    std::string filename;     
    std::string details;      
};

inline error_info make_error(
    error_info::code code,
    std::string message,
    std::string operation,
    std::string filename,
    std::string details = "") {
    return error_info{
        .error_code = code,
        .message = std::move(message),
        .operation = std::move(operation),
        .filename = std::move(filename),
        .details = std::move(details)
    };
}

std::expected<void, error_info> write(const std::string& filename, const nlohmann::json& j);
std::expected<nlohmann::json, error_info> read(const std::string& filename);

}  // namespace json_file

#endif  // !JSON_FILE_H

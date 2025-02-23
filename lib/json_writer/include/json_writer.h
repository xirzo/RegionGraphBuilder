#ifndef JSON_WRITER_H
#define JSON_WRITER_H

#include <expected>
#include <string>

namespace json_writer {

std::expected<void, std::string> write_to_file(const std::string& text,
                                               const std::string& filepath);
}

#endif  // !JSON_WRITER_H

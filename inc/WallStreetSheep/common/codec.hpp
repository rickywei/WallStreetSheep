#pragma once

#include <string>

namespace wss {

std::string Encode(const std::string& from, const std::string& to,
                   const std::string& source);

std::string EncodeUtf8(const std::string&& from, const std::string&& source);

}  // namespace wss

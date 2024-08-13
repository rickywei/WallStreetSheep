
#include <spdlog/spdlog.h>
#include <unicode/ucnv.h>
#include <unicode/unistr.h>

#include <ctime>
#include <limits>
#include <memory>
#include <string>

namespace wss {

std::string Encode(const std::string& from, const std::string& to,
                   const std::string& source) {
  int source_len = source.length();
  int target_len = source_len * 4;
  std::unique_ptr<char[]> target(new char[target_len]);
  //   UErrorCode error = U_ZERO_ERROR;
  //   ucnv_convert(to.c_str(), from.c_str(), target.get(), target_len,
  //                source.c_str(), source_len, &error);
  icu::UnicodeString str(source.c_str(), from.c_str());
  int32_t targetsize =
      str.extract(0, str.length(), target.get(), target_len, to.c_str());
  target[targetsize] = 0;
  return std::string(target.get());
}

std::string EncodeUtf8(const std::string&& from, const std::string&& source) {
  std::string to = "utf8";
  return Encode(from, to, source);
}

std::string Date() {
  std::time_t now;
  std::tm* tm;
  char buffer[11];

  std::time(&now);
  tm = std::localtime(&now);

  std::strftime(buffer, 11, "%Y-%m-%d", tm);
  return std::string(buffer);
}

}  // namespace wss

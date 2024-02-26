#pragma once

#include <string>

class FPE {
public:
    static std::string trim_leading_zeros(const std::string &str);
    static std::string trim_trailing_zeros(const std::string &str);
    static std::string encrypt(const std::string &num_str, const std::string &key);
    static std::string encrypt_num(const std::string &num_str, const std::string &key);
    static std::string decrypt(const std::string &num_str, const std::string &key);
    static std::string decrypt_num(std::string &num_str, const std::string &key);

private:
    static const std::string DEFAULT_KEY;
    static const int MIN_LENGTH = 6;
    static const char FIXED_NUM = '1';
    constexpr static const double EXPANDED = 100000.0 * 1000000000.0;
    constexpr static const uint8_t TWEAK[] = {};
};


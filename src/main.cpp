#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>

#include <ubiq/fpe/ff1.h>
#include <iomanip>

class FPETool {

public:
    static std::string encrypt(const std::string &num_str, const std::string &key) {
        int num_str_length = num_str.length();
        std::cout << "encrypt:" << std::endl;
        std::cout << num_str << std::endl;

        std::string fixed_num_str = num_str;
        if (num_str_length < MIN_LENGTH) {
            for (int i = 0; i < MIN_LENGTH - num_str_length; i++) {
                fixed_num_str = '0' + fixed_num_str;
            }
            num_str_length = MIN_LENGTH;
        }

        const std::string fpe_key_str = key.empty() ? FPETool::DEFAULT_KEY : key;
        int fpe_key_length = fpe_key_str.length();

        std::vector<uint8_t> fpe_key(fpe_key_length);
        for (size_t i = 0; i < fpe_key_length; ++i) {
            fpe_key[i] = static_cast<uint8_t>(fpe_key_str[i]);
        }

        int res;
        struct ff1_ctx *ctx;
        char *out;

        out = (char *) calloc(num_str_length + 1, sizeof(char));
        res = ff1_ctx_create(&ctx, fpe_key.data(), fpe_key_length, FPETool::TWEAK, sizeof(FPETool::TWEAK), 0, SIZE_MAX,
                             10);
        if (res == 0) {
            ff1_encrypt(ctx, out, fixed_num_str.c_str(), NULL, 0);
            ff1_ctx_destroy(ctx);
        }

        std::string result(out);
        free(out);

        return result;
    }

    static std::string encrypt_num(double number, const std::string &key) {
        // 判断是否为整数
        bool is_int = (std::floor(number) == number);

        // 数字转换为字符串时，避免精度的丢失
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(PRECISION) << number;
        std::string num_str = stream.str();
        std::cout << num_str << std::endl;

        std::string num_flag;
        if (num_str[0] == '-') {
            num_flag = "-";
        }

        std::string encrypted_dec_part;
        size_t dot_pos = num_str.find('.');
        std::string int_part = num_str.substr(0, dot_pos);
        std::string encrypted_int_Part = encrypt(int_part, key);
        if (!is_int) {
            std::string dec_part = num_str.substr(dot_pos + 1);
            std::cout << "dec_part" << std::endl;
            std::cout << "0." + dec_part << std::endl;
            long long decimal_dec_part = std::stod("0." + dec_part) * EXPANDED;
            std::cout << "decimal_dec_part:" << std::endl;
            std::cout << decimal_dec_part << std::endl;
            dec_part = std::to_string(static_cast<long long>(decimal_dec_part));
            encrypted_dec_part = "." + encrypt(dec_part, key) + FIXED_NUM;
        }

        return num_flag + FIXED_NUM + encrypted_int_Part + encrypted_dec_part;
    }

    static std::string decrypt(const std::string &num_str, const std::string &key) {
        int num_str_length = num_str.length();

        const std::string fpe_key_str = key.empty() ? FPETool::DEFAULT_KEY : key;
        int fpe_key_length = fpe_key_str.length();

        std::vector<uint8_t> fpe_key(fpe_key_length);
        for (size_t i = 0; i < fpe_key_length; ++i) {
            fpe_key[i] = static_cast<uint8_t>(fpe_key_str[i]);
        }

        int res;
        struct ff1_ctx *ctx;
        char *out;

        out = (char *) calloc(num_str_length + 1, sizeof(char));
        res = ff1_ctx_create(&ctx, fpe_key.data(), fpe_key_length, FPETool::TWEAK, sizeof(FPETool::TWEAK), 0, SIZE_MAX,
                             10);
        if (res == 0) {
            ff1_decrypt(ctx, out, num_str.c_str(), NULL, 0);
            ff1_ctx_destroy(ctx);
        }

        std::string result(out);
        free(out);

        return result;
    }

    static std::string decrypt_num(double number, const std::string &key) {
        // 判断是否为整数
        bool is_int = (std::floor(number) == number);

        // 数字转换为字符串时，避免精度的丢失
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(8) << number;
        std::string num_str = stream.str();

        std::string num_flag;
        if (num_str[0] == '-') {
            num_flag = "-";
        }

        std::string encrypted_dec_part;
        size_t dot_pos = num_str.find('.');
        std::string int_part = num_str.substr(1, dot_pos - 1);

        std::cout << "int_part:" << int_part << std::endl;
        std::string decrypted_int_part = std::to_string(std::stoi(decrypt(int_part, key)));
        std::cout << "decrypted_int_part:" << std::stoi(decrypt(int_part, key)) << std::endl;
        std::string decrypted_dec_part;
        if (!is_int) {
            std::string dec_part = num_str.substr(dot_pos + 1, num_str.length() -1);

            std::cout << "dec_part:" << std::endl;
            std::cout << dec_part << std::endl;
            decrypted_dec_part = decrypt(dec_part, key);
            std::cout << "decrypted_dec_part:" << decrypted_dec_part << std::endl;
            long long expanded_value = std::stoll(decrypted_dec_part);
            double decimal_dec_part = static_cast<double>(expanded_value) / EXPANDED;
            decrypted_dec_part = std::to_string(decimal_dec_part);
        }

        std::string decrypted_num = num_flag + decrypted_int_part + decrypted_dec_part;

        return decrypted_num;
    }

private:
    static const int PRECISION = 10;
    static const std::string DEFAULT_KEY;
    static const int MIN_LENGTH = 6;
    static const char FIXED_NUM = '1';
    constexpr static const double EXPANDED = 100;
    constexpr static const uint8_t TWEAK[] = {};
};

const std::string FPETool::DEFAULT_KEY = "abcdefghijk12345abcdefghijk12345";

int main() {

    std::string key = "abcdefghijk12345abcdefghijk12345";

    //double value = 123456789;
//    std::string decrypt_str = FPETool::decrypt(encrypt_str, key);
//    std::cout << "decrypt_str:" << decrypt_str << std::endl;

    double num = 123.45;
    std::cout << "original_str:" << std::to_string(num) << std::endl;

    std::string encrypt_str = FPETool::encrypt_num(num, key);
    std::cout << "encrypt_str:" << std::endl;
    std::cout << encrypt_str << std::endl;

    std::string decrypt_str = FPETool::decrypt_num(std::stod(encrypt_str), key);
    std::cout << "decrypt_str:" << decrypt_str << std::endl;

    return 0;
}

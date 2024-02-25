#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
#include <gmp.h>

#include <ubiq/fpe/ff1.h>
#include <iomanip>

class FPETool {

public:

    static std::string trim_leading_zerors(const std::string& str) {
        size_t start = str.find_first_not_of('0');
        if (start == std::string::npos) {
            return "0";
        }
        return str.substr(start);
    }
    static std::string trim_trailing_zeros(const std::string& str) {
        size_t end = str.find_last_not_of('0');
        if (end == std::string::npos) {
            return str;
        }
        return str.substr(0, end + 1);
    }

    static std::string encrypt(const std::string& num_str, const std::string& key) {
        int num_str_length = num_str.length();
        std::string fixed_num_str = num_str;
        if (num_str_length < MIN_LENGTH) {
            std::string padding(MIN_LENGTH - num_str_length , '0');
            fixed_num_str = padding + fixed_num_str;
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

    static std::string encrypt_num(const std::string& num_str, const std::string& key) {
        std::string num_flag;
        if (num_str[0] == '-') {
            num_flag = "-";
        }

        std::string encrypted_dec_part;
        size_t dot_pos = num_str.find('.');
        std::string int_part = num_str.substr(0, dot_pos);
        std::string encrypted_int_Part = encrypt(int_part, key);
        if (dot_pos != std::string::npos) {
            std::string dec_part = num_str.substr(dot_pos + 1);
            dec_part = "0." + dec_part;
            double dec_part_num = std::stod(dec_part);
            dec_part_num = dec_part_num * EXPANDED;
            auto dec_int_part = static_cast<long long>(dec_part_num);
            encrypted_dec_part = "." + encrypt(std::to_string(dec_int_part), key) + FIXED_NUM;
        }

        return num_flag + FIXED_NUM + encrypted_int_Part + encrypted_dec_part;
    }

    static std::string decrypt(const std::string& num_str, const std::string& key) {
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

    static std::string decrypt_num(std::string& num_str, const std::string& key) {
        std::string num_flag;
        if (num_str[0] == '-') {
            num_flag = "-";
            num_str = num_str.substr(1);
        }

        std::string encrypted_dec_part;
        size_t dot_pos = num_str.find('.');
        std::string int_part = num_str.substr(1, dot_pos - 1);
        std::string decrypted_int_part = trim_leading_zerors(decrypt(int_part, key));

        std::string decrypted_dec_part;
        if (dot_pos != std::string::npos) {
            std::string dec_part = num_str.substr(dot_pos + 1, num_str.length() - dot_pos - 2);
            decrypted_dec_part = decrypt(dec_part, key);
            int expanded_length = std::to_string(static_cast<long long>(EXPANDED)).length() - 1;
            std::string leading_zeros(expanded_length - decrypted_dec_part.length(), '0');
            decrypted_dec_part = leading_zeros + decrypted_dec_part;
            decrypted_dec_part = trim_trailing_zeros(decrypted_dec_part);
            decrypted_dec_part = '.' + decrypted_dec_part;
        }

        std::string decrypted_num = num_flag + decrypted_int_part + decrypted_dec_part;

        return decrypted_num;
    }

private:
    static const int PRECISION = 14;
    static const std::string DEFAULT_KEY;
    static const int MIN_LENGTH = 6;
    static const char FIXED_NUM = '1';
    constexpr static const double EXPANDED = 100000.0 * 1000000000.0;
    constexpr static const uint8_t TWEAK[] = {};
};

const std::string FPETool::DEFAULT_KEY = "abcdefghijk12345abcdefghijk12345";

int main() {

    std::string key = "abcdefghijk12345abcdefghijk12345";;

    std::string num_str = "123123.0102";
    std::cout << "original_str:" << num_str << std::endl;

    std::string encrypt_str = FPETool::encrypt_num(num_str, key);
    std::cout << "encrypt_str:" << std::endl;
    std::cout << encrypt_str << std::endl;

    std::string decrypt_str = FPETool::decrypt_num(encrypt_str, key);
    std::cout << "decrypt_str:" << decrypt_str << std::endl;

    return 0;
}

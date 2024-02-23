#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

#include <ubiq/fpe/ff1.h>

class FPETool {

public:
    static std::string encrypt(const std::string& num_str, const std::string& key) {
        int num_str_length = num_str.length();

        uint8_t fpe_key[] = {};
        const std::string fpe_key_str = key.empty() ? FPETool::DEFAULT_KEY : key;
        for (size_t i = 0; i < fpe_key_str.length(); ++i) {
            fpe_key[i] = static_cast<uint8_t>(fpe_key_str[i]);
        }
        size_t fpe_key_length = fpe_key_str.length();

        int res;
        struct ff1_ctx* ctx;
        char* out;

        out = (char *) calloc(num_str_length, 1);
        res = ff1_ctx_create(&ctx, fpe_key, fpe_key_length, FPETool::TWEAK, sizeof(FPETool::TWEAK), 0, SIZE_MAX, 10);
        if (res == 0) {
            ff1_encrypt(ctx, out, num_str.c_str(), NULL, 0);
            ff1_ctx_destroy(ctx);
        }

        std::string result(out);
        free(out);

        return result;
    }

    static std::string encrypt_num(double number, const std::string& key) {
        std::string num_str;
        if (std::floor(number) == number) {
            num_str = std::to_string(static_cast<long long>(number));
        } else {
            num_str = std::to_string(number);
        }

        std::string num_flag;
        if (num_str[0] == '-') {
            num_flag = "-";
            num_str = num_str.substr(1);
        }

        size_t dot_pos = num_str.find('.');
        std::string int_part = num_str;
        std::string dec_part;

        if (dot_pos != std::string::npos) {
            int_part = num_str.substr(0, dot_pos);
            dec_part = num_str.substr(dot_pos + 1);
            long long dec_part_num = std::stoll(dec_part) * std::pow(10, 9);
            dec_part = std::to_string(dec_part_num);
        }

        std::string encrypted_int_Part = encrypt(int_part, key);
        std::string encrypted_dec_part = dec_part.empty() ? "" : "." + encrypt(dec_part, key);

        return num_flag + encrypted_int_Part + encrypted_dec_part;
    }

    static std::string decrypt(const std::string& num_str, const std::string& key) {
        int num_str_length = num_str.length();

        uint8_t fpe_key[] = {};
        const std::string fpe_key_str = key.empty() ? FPETool::DEFAULT_KEY : key;
        for (size_t i = 0; i < fpe_key_str.length(); ++i) {
            fpe_key[i] = static_cast<uint8_t>(fpe_key_str[i]);
        }
        size_t fpe_key_length = fpe_key_str.length();

        int res;
        struct ff1_ctx* ctx;
        char* out;

        out = (char *) calloc(num_str_length, 1);
        res = ff1_ctx_create(&ctx, fpe_key, fpe_key_length, FPETool::TWEAK, sizeof(FPETool::TWEAK), 0, SIZE_MAX, 10);
        if (res == 0) {
            ff1_decrypt(ctx, out, num_str.c_str(), NULL, 0);
            ff1_ctx_destroy(ctx);
        }

        std::string result(out);
        free(out);

        return result;
    }

    static std::string decrypt_num(double number , const std::string& key) {
        std::string num_str;
        if (std::floor(number) == number) {
            num_str = std::to_string(static_cast<long long>(number));
        } else {
            num_str = std::to_string(number);
        }

        std::string num_flag;
        if (num_str[0] == '-') {
            num_flag = "-";
            num_str = num_str.substr(1);
        }

        size_t dotPos = num_str.find('.');
        std::string int_part = num_str;
        std::string dec_part;
        if (dotPos != std::string::npos) {
            int_part = num_str.substr(0, dotPos);
            dec_part = num_str.substr(dotPos + 1);
            long long dec_part_num = std::stoll(decrypt(dec_part, key));
            dec_part = std::to_string(static_cast<double>(dec_part_num) / std::pow(10, 9));
        }

        std::string decrypted_int_part = decrypt(int_part, key);
        std::string decrypted_num = num_flag + decrypted_int_part + (dec_part.empty() ? "" : "." + dec_part);

        return decrypted_num;
    }

private:
    static const std::string DEFAULT_KEY;
    constexpr static const uint8_t TWEAK[] = {};
};

const std::string FPETool::DEFAULT_KEY = "abcdefghijk12345abcdefghijk12345";

int main() {
    std::string key = "abcdefghijk12345abcdefghijk12345";
    double num = 1234567890;
    std::cout << "original_str:" << std::to_string(num) << std::endl;

    std::string encrypt_str = FPETool::encrypt_num(num, key);
    std::cout << "encrypt_str:" << encrypt_str << std::endl;

    std::string decrypt_str = FPETool::decrypt_num(std::stod(encrypt_str), key);
    std::cout << "decrypt_str:" << decrypt_str << std::endl;

    return 0;
}
